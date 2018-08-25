#include "PCISPHSolver.h"

namespace LiPhEn {
	PCISPHSolver::PCISPHSolver()
	{
		m_minIterations = 4;
		m_maxDensityErrorRatio = 0.1f;

		m_predictedPositionsBuffer = NULL;
		m_predictedHalfVelocitiesBuffer = NULL;
		m_predictedPressureForcesBuffer = NULL;
		m_predictedDensitiesBuffer = NULL;

		m_pciInitKernel = NULL;
		m_pciIntegrateKernel = NULL;
		m_pciHandleCollisionsKernel = NULL;
		m_pciCalcDensityPressureKernel = NULL;
		m_pciCalcPressureForceKernel = NULL;
		m_pciAddPressureForceKernel = NULL;
	}

	PCISPHSolver::~PCISPHSolver()
	{
		delete m_predictedPositionsBuffer;
		delete m_predictedHalfVelocitiesBuffer;
		delete m_predictedPressureForcesBuffer;
		delete m_predictedDensitiesBuffer;

		delete m_pciInitKernel;
		delete m_pciIntegrateKernel;
		delete m_pciHandleCollisionsKernel;
		delete m_pciCalcDensityPressureKernel;
		delete m_pciCalcPressureForceKernel;
		delete m_pciAddPressureForceKernel;
	}

	void PCISPHSolver::addParticle(SPHParticle* particle)
	{
		if (dynamic_cast<PCISPHParticle*>(particle))
		{
			SPHSolver::addParticle(particle);
		}
	}

	void PCISPHSolver::addParticle(PCISPHParticle* particle)
	{
		SPHSolver::addParticle(particle);
	}

	void PCISPHSolver::accumulatePressureForces(float deltaTime)
	{
		// Mapping from Density Error to Pressure
		float delta = calcDelta(deltaTime);
		
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			// PCI Init
			for (int i = 0; i < m_particles.size(); i++) {
				PCISPHParticle* particle = dynamic_cast<PCISPHParticle*>(m_particles[i]);
				particle->setPressure(0.f);
				particle->setPredictedPressureForce(Vector3D(0.f, 0.f, 0.f));
			}

			for (int k = 0; k < m_minIterations; k++)
			{
				// Predict velocity and position
				for (int i = 0; i < m_particles.size(); i++) {
					PCISPHParticle* particle = dynamic_cast<PCISPHParticle*>(m_particles[i]);
					Vector3D halfVelocity = particle->getHalfVelocity();
					Vector3D predictedAcceleration = (particle->getAccumulatedForces() + particle->getPredictedPressureForce()) / particle->getDensity();
					if (particle->getIsFirstTimeStep())
					{
						halfVelocity = particle->getVelocity() - predictedAcceleration * deltaTime / 2.f;
					}

					Vector3D predictedHalfVelocity = halfVelocity + predictedAcceleration * deltaTime;
					Vector3D predictedPosition = particle->getPosition() + predictedHalfVelocity * deltaTime;

					particle->setPredictedHalfVelocity(predictedHalfVelocity);
					particle->setPredictedPosition(predictedPosition);
				}

				// Resolve collisions
				for (int i = 0; i < m_particles.size(); i++) {
					PCISPHParticle* particle = dynamic_cast<PCISPHParticle*>(m_particles[i]);
					Vector3D positionToResolve = particle->getPredictedPosition();
					Vector3D velocityToResolve = particle->getPredictedHalfVelocity();
					handleCollision(&positionToResolve, &velocityToResolve);

					particle->setPredictedPosition(positionToResolve);
					particle->setPredictedHalfVelocity(velocityToResolve);
				}

				// Compute pressure from density error
				for (int i = 0; i < m_particles.size(); i++) {
					PCISPHParticle* particle = dynamic_cast<PCISPHParticle*>(m_particles[i]);

					// Measure the predicted density with particles' predicted locations
					float weightedSum = 0.f;

					for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
					{
						PCISPHParticle* pciNeighborParticle = dynamic_cast<PCISPHParticle*>(neighborParticle);
						float distance = (pciNeighborParticle->getPredictedPosition() - particle->getPredictedPosition()).magnitude();
						weightedSum += m_defaultKernel.getKernelWeight(distance);
					}

					float predictedDensity = m_particleMass * weightedSum;
					float densityError = predictedDensity - m_restDensity;
					float predictedPressure = delta * densityError;

					if (predictedPressure < 0.f)
					{
						densityError *= m_negativePressureFactor;
						predictedPressure *= m_negativePressureFactor;
					}

					particle->setPredictedDensity(predictedDensity);
					particle->setDensityError(densityError);
					particle->setPressure(particle->getPressure() + predictedPressure);
				}

				// Compute pressure gradient force
				for (int i = 0; i < m_particles.size(); i++) {
					PCISPHParticle* particle = dynamic_cast<PCISPHParticle*>(m_particles[i]);
					Vector3D pressureForce;
					float tempFactor = particle->getPressure() / (particle->getPredictedDensity() * particle->getPredictedDensity());

					for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
					{
						PCISPHParticle* pciNeighborParticle = dynamic_cast<PCISPHParticle*>(neighborParticle);
						if (pciNeighborParticle != particle)
						{
							float distance = (pciNeighborParticle->getPredictedPosition() - particle->getPredictedPosition()).magnitude();
							Vector3D direction = (particle->getPredictedPosition() - pciNeighborParticle->getPredictedPosition()) / distance;
							pressureForce += direction * m_pressureKernel.getFirstDerivativeWeight(distance) *
								(tempFactor + neighborParticle->getPressure() / (pciNeighborParticle->getPredictedDensity() * pciNeighborParticle->getPredictedDensity()));
						}
					}
					pressureForce *= -(m_particleMass * particle->getDensity());
					if (isnan(pressureForce.getX()))
						pressureForce = Vector3D(0.f, 0.f, 0.f);

					particle->setPredictedPressureForce(pressureForce);
				}

				float maxDensityError = 0.f;
				for (SPHParticle* particle : m_particles)
				{
					PCISPHParticle* pciParticle = dynamic_cast<PCISPHParticle*>(particle);
					maxDensityError = std::max(maxDensityError, fabs(pciParticle->getDensityError()));
				}

				float densityErrorRatio = maxDensityError / m_restDensity;

				if (fabs(densityErrorRatio) < m_maxDensityErrorRatio)
					break;
			}

			// PCI Add Pressure Force
			for (int i = 0; i < m_particles.size(); i++) {
				PCISPHParticle* particle = dynamic_cast<PCISPHParticle*>(m_particles[i]);
				particle->addForce(particle->getPredictedPressureForce());
			}
		}
		else
		{
			// PCI Init
			m_pciInitKernel->setArgument(0, m_pressuresBuffer);
			m_pciInitKernel->setArgument(1, m_predictedPressureForcesBuffer);
			m_pciInitKernel->setArgument(2, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);

			m_parallelComputationInterface->executeKernel(m_pciInitKernel, m_dummyParticleCount, m_workGroupSize);

			for (int k = 0; k < m_minIterations; k++)
			{
				// PCI Integrate
				m_pciIntegrateKernel->setArgument(0, m_positionsBuffer1);
				m_pciIntegrateKernel->setArgument(1, m_velocitiesBuffer1);
				m_pciIntegrateKernel->setArgument(2, m_halfVelocitiesBuffer1);
				m_pciIntegrateKernel->setArgument(3, m_accumulatedForcesBuffer);
				m_pciIntegrateKernel->setArgument(4, m_predictedPressureForcesBuffer);
				m_pciIntegrateKernel->setArgument(5, m_densitiesBuffer);
				m_pciIntegrateKernel->setArgument(6, m_isFirstTimeStepsBuffer1);
				m_pciIntegrateKernel->setArgument(7, m_predictedHalfVelocitiesBuffer);
				m_pciIntegrateKernel->setArgument(8, m_predictedPositionsBuffer);
				m_pciIntegrateKernel->setArgument(9, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
				m_pciIntegrateKernel->setArgument(10, sizeof(deltaTime), &deltaTime);

				m_parallelComputationInterface->executeKernel(m_pciIntegrateKernel, m_dummyParticleCount, m_workGroupSize);

				// PCI Handle Collisions
				m_pciHandleCollisionsKernel->setArgument(0, m_predictedPositionsBuffer);
				m_pciHandleCollisionsKernel->setArgument(1, m_predictedHalfVelocitiesBuffer);
				m_pciHandleCollisionsKernel->setArgument(2, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
				m_pciHandleCollisionsKernel->setArgument(3, m_collisionBoxesBuffer);
				m_pciHandleCollisionsKernel->setArgument(4, m_collisionSpheresBuffer);

				m_parallelComputationInterface->executeKernel(m_pciHandleCollisionsKernel, m_dummyParticleCount, m_workGroupSize);

				// PCI Calc Density Pressure
				m_pciCalcDensityPressureKernel->setArgument(0, m_positionsBuffer1);
				m_pciCalcDensityPressureKernel->setArgument(1, m_predictedPositionsBuffer);
				m_pciCalcDensityPressureKernel->setArgument(2, m_predictedDensitiesBuffer);
				m_pciCalcDensityPressureKernel->setArgument(3, m_pressuresBuffer);
				m_pciCalcDensityPressureKernel->setArgument(4, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
				m_pciCalcDensityPressureKernel->setArgument(5, m_cellListBuffer);
				m_pciCalcDensityPressureKernel->setArgument(6, m_defaultKernelWeightsBuffer);
				m_pciCalcDensityPressureKernel->setArgument(7, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);
				m_pciCalcDensityPressureKernel->setArgument(8, sizeof(delta), &delta);

				m_parallelComputationInterface->executeKernel(m_pciCalcDensityPressureKernel, m_dummyParticleCount, m_workGroupSize);

				// PCI Calc Pressure Force
				m_pciCalcPressureForceKernel->setArgument(0, m_positionsBuffer1);
				m_pciCalcPressureForceKernel->setArgument(1, m_predictedPositionsBuffer);
				m_pciCalcPressureForceKernel->setArgument(2, m_predictedDensitiesBuffer);
				m_pciCalcPressureForceKernel->setArgument(3, m_pressuresBuffer);
				m_pciCalcPressureForceKernel->setArgument(4, m_predictedPressureForcesBuffer);
				m_pciCalcPressureForceKernel->setArgument(5, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
				m_pciCalcPressureForceKernel->setArgument(6, m_cellListBuffer);
				m_pciCalcPressureForceKernel->setArgument(7, m_pressureKernelFirstDerivativeWeightsBuffer);
				m_pciCalcPressureForceKernel->setArgument(8, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);

				m_parallelComputationInterface->executeKernel(m_pciCalcPressureForceKernel, m_dummyParticleCount, m_workGroupSize);
			}

			// PCI Add Pressure Force
			m_pciAddPressureForceKernel->setArgument(0, m_predictedPressureForcesBuffer);
			m_pciAddPressureForceKernel->setArgument(1, m_accumulatedForcesBuffer);
			m_pciAddPressureForceKernel->setArgument(2, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);

			m_parallelComputationInterface->executeKernel(m_pciAddPressureForceKernel, m_dummyParticleCount, m_workGroupSize);
		}	
	}

	void PCISPHSolver::reinitParallelContext()
	{
		SPHSolver::reinitParallelContext();

		if (m_pciInitKernel)
			delete m_pciInitKernel;
		if (m_pciIntegrateKernel)
			delete m_pciIntegrateKernel;
		if (m_pciHandleCollisionsKernel)
			delete m_pciHandleCollisionsKernel;
		if (m_pciCalcDensityPressureKernel)
			delete m_pciCalcDensityPressureKernel;
		if (m_pciCalcPressureForceKernel)
			delete m_pciCalcPressureForceKernel;
		if (m_pciAddPressureForceKernel)
			delete m_pciAddPressureForceKernel;

		m_pciInitKernel = m_parallelComputationInterface->createKernel("pciInit");
		m_pciIntegrateKernel = m_parallelComputationInterface->createKernel("pciIntegrate");
		m_pciHandleCollisionsKernel = m_parallelComputationInterface->createKernel("pciHandleCollisions");
		m_pciCalcDensityPressureKernel = m_parallelComputationInterface->createKernel("pciCalcDensityPressure");
		m_pciCalcPressureForceKernel = m_parallelComputationInterface->createKernel("pciCalcPressureForce");
		m_pciAddPressureForceKernel = m_parallelComputationInterface->createKernel("pciAddPressureForce");
	}

	void PCISPHSolver::initParallelBuffers()
	{
		SPHSolver::initParallelBuffers();

		if (m_predictedPositionsBuffer)
			delete m_predictedPositionsBuffer;
		if (m_predictedHalfVelocitiesBuffer)
			delete m_predictedHalfVelocitiesBuffer;
		if (m_predictedPressureForcesBuffer)
			delete m_predictedPressureForcesBuffer;
		if (m_predictedDensitiesBuffer)
			delete m_predictedDensitiesBuffer;

		m_predictedPositionsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
		m_predictedHalfVelocitiesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
		m_predictedPressureForcesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
		m_predictedDensitiesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float));
	}

	float PCISPHSolver::calcDelta(float deltaTime)
	{
		float delta = 0.f;

		std::vector<Vector3D> neighborPoints;
		float stepSize = 1.6f * m_particleRadius;

		for (int i = -3; i <= 3; i++)
		{
			for (int j = -3; j <= 3; j++)
			{
				for (int k = -3; k <= 3; k++)
				{
					neighborPoints.push_back(Vector3D(i * stepSize, j * stepSize, k * stepSize));
				}
			}
		}

		float denom = 0.f;
		Vector3D denom1 = Vector3D(0.f, 0.f, 0.f);
		float denom2 = 0.f;

		for (Vector3D neighborPoint : neighborPoints)
		{
			float distance = neighborPoint.magnitude();
			if (distance > 0.f)
			{
				Vector3D direction = neighborPoint / distance;
				Vector3D gradWij = direction * m_pressureKernel.getFirstDerivativeWeight(distance);
				denom1 += gradWij;
				denom2 += gradWij * gradWij;
			}
		}

		denom = -(denom1 * denom1) - denom2;
		float beta = deltaTime * m_particleMass / m_restDensity;
		beta = 2.f * beta * beta;

		if (fabs(denom) > 0.f)
			delta = -1.f / (beta * denom);

		return delta;
	}

	// GETTER
	int PCISPHSolver::getMinIterations() const
	{
		return m_minIterations;
	}

	float PCISPHSolver::getMaxDensityErrorRatio() const
	{
		return m_maxDensityErrorRatio;
	}

	// SETTER
	void PCISPHSolver::setMinIterations(int maxIterations)
	{
		m_minIterations = maxIterations;
	}

	void PCISPHSolver::setMaxDensityErrorRatio(float maxDensityErrorRatio)
	{
		m_maxDensityErrorRatio = maxDensityErrorRatio;
	}
}
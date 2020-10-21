#include "Parallelization/OpenCLInterface.h"
#include "SPHSolver.h"

#include <fstream>
#include "float.h"

namespace LiPhEn {

	SPHSolver::SPHSolver()
	{
		m_gravity = Vector3D(0.f, -9.81f, 0.f);
		m_kernelRadiusFactor = 2.688f;
		m_gridSpacingFactor = 1.f;
		m_restDensity = 998.29f;
		m_viscosityCoefficient = 3.5f;
		m_pressureStiffnessCoefficient = 3.f;
		m_negativePressureFactor = 0.f;
		m_surfaceTensionCoefficient = 0.0728f;
		m_surfaceTensionThreshold = 7.065f;
		m_restitutionCoefficient = 0.5f;
		m_frictionCoefficient = 1.f;

		setParticleRadius(0.017f);

		m_workGroupSize = 64;
		m_radixThreadCount = 256;
		m_radixWidth = 8;
		m_radixBucketCount = 256;	// 8 bit
		m_radixPassCount = 4;		// 4*8 bit = 32 bit = sizeof(unsinged int)
		m_hasParallelContextChanged = true;
		m_hasCollisionObjectDataChanged = true;
		m_hasParticleDataChanged = true;
		m_hasKernelWeightDataChanged = true;

		m_positionsBuffer1 = NULL;
		m_positionsBuffer2 = NULL;
		m_velocitiesBuffer1 = NULL;
		m_velocitiesBuffer2 = NULL;
		m_halfVelocitiesBuffer1 = NULL;
		m_halfVelocitiesBuffer2 = NULL;
		m_isFirstTimeStepsBuffer1 = NULL;
		m_isFirstTimeStepsBuffer2 = NULL;
		m_gridIndicesBuffer1 = NULL;
		m_gridIndicesBuffer2 = NULL;
		m_oldHalfVelocitiesBuffer = NULL;
		m_accumulatedForcesBuffer = NULL;
		m_densitiesBuffer = NULL;
		m_pressuresBuffer = NULL;

		m_defaultKernelWeightsBuffer = NULL;
		m_defaultKernelFirstDerivativeWeightsBuffer = NULL;
		m_defaultKernelSecondDerivativeWeightsBuffer = NULL;
		m_pressureKernelFirstDerivativeWeightsBuffer = NULL;
		m_viscosityKernelSecondDerivativeWeightsBuffer = NULL;
		m_collisionBoxesBuffer = NULL;
		m_collisionSpheresBuffer = NULL;
		m_bucketCountsBuffer = NULL;
		m_cellListBuffer = NULL;

		m_calcGridIndicesKernel = NULL;
		m_countDigitsInBucketsKernel = NULL;
		m_scanBucketsKernel = NULL;
		m_permuteParticlesKernel = NULL;
		m_buildCellListKernel = NULL;
		m_calcDensityPressureKernel = NULL;
		m_accumulateNonPressureForcesKernel = NULL;
		m_accumulatePressureForcesKernel = NULL;
		m_integrateKernel = NULL;
		m_handleCollisionsKernel = NULL;

		m_parallelComputationInterface = new OpenCLInterface();
		m_parallelComputationInterface->initialize(true);

		if (m_parallelComputationInterface->isValid())
		{
			if(m_parallelComputationInterface->hasGPU())
				m_parallelizationType = ParallelizationType::GPU;
			else
				m_parallelizationType = ParallelizationType::CPU;
			reinitParallelContext();
		}
		else
		{
			m_parallelizationType = ParallelizationType::NONE;
		}
	}

	SPHSolver::~SPHSolver()
	{
        cleanUp();

		delete m_positionsBuffer1;
		delete m_positionsBuffer2;
		delete m_velocitiesBuffer1;
		delete m_velocitiesBuffer2;
		delete m_halfVelocitiesBuffer1;
		delete m_halfVelocitiesBuffer2;
		delete m_isFirstTimeStepsBuffer1;
		delete m_isFirstTimeStepsBuffer2;
		delete m_gridIndicesBuffer1;
		delete m_gridIndicesBuffer2;
		delete m_oldHalfVelocitiesBuffer;
		delete m_accumulatedForcesBuffer;
		delete m_densitiesBuffer;
		delete m_pressuresBuffer;

		delete m_defaultKernelWeightsBuffer;
		delete m_defaultKernelFirstDerivativeWeightsBuffer;
		delete m_defaultKernelSecondDerivativeWeightsBuffer;
		delete m_pressureKernelFirstDerivativeWeightsBuffer;
		delete m_viscosityKernelSecondDerivativeWeightsBuffer;
		delete m_collisionBoxesBuffer;
		delete m_collisionSpheresBuffer;
		delete m_bucketCountsBuffer;
		delete m_cellListBuffer;

		delete m_calcGridIndicesKernel;
		delete m_countDigitsInBucketsKernel;
		delete m_scanBucketsKernel;
		delete m_permuteParticlesKernel;
		delete m_buildCellListKernel;
		delete m_calcDensityPressureKernel;
		delete m_accumulateNonPressureForcesKernel;
		delete m_accumulatePressureForcesKernel;
		delete m_integrateKernel;
		delete m_handleCollisionsKernel;
	}

	void SPHSolver::addParticle(SPHParticle* particle)
	{
		m_particles.push_back(particle);

		m_hasParticleDataChanged = true;
		m_parallelSPHParameters.particleCount = m_particles.size();
	}

	void SPHSolver::removeParticles()
	{
		int particleCount = m_particles.size();
		for (int i = 0; i < particleCount; i++)
		{
			SPHParticle* particleToRemove = m_particles.back();
			m_particles.pop_back();
			delete particleToRemove;
		}

        m_spatialGrid.clear();

        // clear cachedNeighborLists
        for (std::vector<SPHParticle*> neightborList : m_cachedNeighborLists)
        {
            neightborList.clear();
        }
        m_cachedNeighborLists.clear();

		m_hasParticleDataChanged = true;
		m_parallelSPHParameters.particleCount = m_particles.size();
	}

	void SPHSolver::addStaticCollisionObject(StaticCollisionObject* collisionObject)
	{
		m_collisionObjects.push_back(collisionObject);

		m_hasCollisionObjectDataChanged = true;
	}

	void SPHSolver::removeStaticCollisionObjects()
	{
		int collisionObjectCount = m_collisionObjects.size();
		for (int i = 0; i < collisionObjectCount; i++)
		{
			StaticCollisionObject* collisionObjectToRemove = m_collisionObjects.back();
			m_collisionObjects.pop_back();
			delete collisionObjectToRemove;
        }

		m_hasCollisionObjectDataChanged = true;
    }

    void SPHSolver::cleanUp()
    {
        removeParticles();
        removeStaticCollisionObjects();
    }

	void SPHSolver::onBeginUpdate()
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			buildCachedNeighborLists();	
		}
		else
		{
			initParallelBuffers();
			buildParallelGrid();
		}

		calcParticleDensityPressure();
	}

	void SPHSolver::accumulateNonPressureForces(float deltaTime)
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			for (int i = 0; i < m_particles.size(); i++) {
				SPHParticle* particle = m_particles[i];

				// compute gravity force
				particle->addForce(m_gravity * particle->getDensity());

				// compute surface tension force
				Vector3D surfaceNormal;
				for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
				{
					Vector3D direction = (particle->getPosition() - neighborParticle->getPosition());
					float distance = (neighborParticle->getPosition() - particle->getPosition()).magnitude();
					surfaceNormal += direction * m_defaultKernel.getFirstDerivativeWeight(distance) / neighborParticle->getDensity();
				}
				surfaceNormal *= m_particleMass;

				float surfaceNormalLength = surfaceNormal.magnitude();
				if (surfaceNormalLength > m_surfaceTensionThreshold)
				{
					float laplacianColor = 0.f;
					for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
					{
						float distance = (particle->getPosition() - neighborParticle->getPosition()).magnitude();
						laplacianColor += m_defaultKernel.getSecondDerivativeWeight(distance) / neighborParticle->getDensity();
					}
					Vector3D surfaceTensionForce = (surfaceNormal / surfaceNormalLength) * (-m_surfaceTensionCoefficient * laplacianColor * m_particleMass);

					particle->addForce(surfaceTensionForce);
				}

				// compute viscosity force
				Vector3D viscosityForce;
				for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
				{
					if (neighborParticle != particle)
					{
						float distance = (particle->getPosition() - neighborParticle->getPosition()).magnitude();
						viscosityForce += ((neighborParticle->getVelocity() - particle->getVelocity()) / neighborParticle->getDensity()) * m_viscosityKernel.getSecondDerivativeWeight(distance);
					}
				}
				viscosityForce *= m_viscosityCoefficient * m_particleMass;
				particle->addForce(viscosityForce);
			}
		}
		else
		{
			m_accumulateNonPressureForcesKernel->setArgument(0, m_positionsBuffer1);
			m_accumulateNonPressureForcesKernel->setArgument(1, m_velocitiesBuffer1);
			m_accumulateNonPressureForcesKernel->setArgument(2, m_densitiesBuffer);
			m_accumulateNonPressureForcesKernel->setArgument(3, m_accumulatedForcesBuffer);
			m_accumulateNonPressureForcesKernel->setArgument(4, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_accumulateNonPressureForcesKernel->setArgument(5, m_cellListBuffer);
			m_accumulateNonPressureForcesKernel->setArgument(6, m_defaultKernelFirstDerivativeWeightsBuffer);
			m_accumulateNonPressureForcesKernel->setArgument(7, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);
			m_accumulateNonPressureForcesKernel->setArgument(8, m_defaultKernelSecondDerivativeWeightsBuffer);
			m_accumulateNonPressureForcesKernel->setArgument(9, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);
			m_accumulateNonPressureForcesKernel->setArgument(10, m_viscosityKernelSecondDerivativeWeightsBuffer);
			m_accumulateNonPressureForcesKernel->setArgument(11, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);

			m_parallelComputationInterface->executeKernel(m_accumulateNonPressureForcesKernel, m_dummyParticleCount, m_workGroupSize);
		}
	}

	void SPHSolver::accumulatePressureForces(float deltaTime)
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			// compute pressure gradient force
			for (int i = 0; i < m_particles.size(); i++) {
				SPHParticle* particle = m_particles[i];

				Vector3D pressureForce;
				float tempFactor = particle->getPressure() / (particle->getDensity() * particle->getDensity());
				for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
				{
					if (neighborParticle != particle)
					{
						float distance = (particle->getPosition() - neighborParticle->getPosition()).magnitude();
						Vector3D direction = (particle->getPosition() - neighborParticle->getPosition()) / distance;
						pressureForce += direction * m_pressureKernel.getFirstDerivativeWeight(distance) *
							(tempFactor + neighborParticle->getPressure() / (neighborParticle->getDensity() * neighborParticle->getDensity()));
					}
				}
				pressureForce *= -(m_particleMass * particle->getDensity());
				particle->addForce(pressureForce);
			}
		}
		else
		{
			m_accumulatePressureForcesKernel->setArgument(0, m_positionsBuffer1);
			m_accumulatePressureForcesKernel->setArgument(1, m_densitiesBuffer);
			m_accumulatePressureForcesKernel->setArgument(2, m_pressuresBuffer);
			m_accumulatePressureForcesKernel->setArgument(3, m_accumulatedForcesBuffer);
			m_accumulatePressureForcesKernel->setArgument(4, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_accumulatePressureForcesKernel->setArgument(5, m_cellListBuffer);
			m_accumulatePressureForcesKernel->setArgument(6, m_pressureKernelFirstDerivativeWeightsBuffer);
			m_accumulatePressureForcesKernel->setArgument(7, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);

			m_parallelComputationInterface->executeKernel(m_accumulatePressureForcesKernel, m_dummyParticleCount, m_workGroupSize);
		}	
	}

	void SPHSolver::accumulateForces(float deltaTime)
	{
		accumulateNonPressureForces(deltaTime);
		accumulatePressureForces(deltaTime);	
	}

	void SPHSolver::integrate(float deltaTime)
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			for (int i = 0; i < m_particles.size(); i++) {
			    m_particles[i]->integrate(deltaTime);
			}
		}
		else
		{
			m_integrateKernel->setArgument(0, m_positionsBuffer1);
			m_integrateKernel->setArgument(1, m_halfVelocitiesBuffer1);
			m_integrateKernel->setArgument(2, m_isFirstTimeStepsBuffer1);
			m_integrateKernel->setArgument(3, m_velocitiesBuffer1);
			m_integrateKernel->setArgument(4, m_accumulatedForcesBuffer);
			m_integrateKernel->setArgument(5, m_densitiesBuffer);
			m_integrateKernel->setArgument(6, m_oldHalfVelocitiesBuffer);
			m_integrateKernel->setArgument(7, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_integrateKernel->setArgument(8, sizeof(deltaTime), &deltaTime);

			m_parallelComputationInterface->executeKernel(m_integrateKernel, m_dummyParticleCount, m_workGroupSize);
		}	
	}

	void SPHSolver::handleCollisions()
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			for (int i = 0; i < m_particles.size(); i++) {
			    SPHParticle* particle = m_particles[i];

			    ParticleCollisionData particleData;
			    particleData.position = particle->getPosition();
				particleData.velocity = particle->getHalfVelocity();

				particleData = handleCollision(particleData);

			    particle->setPosition(particleData.position);
			    particle->setHalfVelocity(particleData.velocity);

			    particle->aproximateVelocity();
			}
		}
		else
		{
			m_handleCollisionsKernel->setArgument(0, m_positionsBuffer1);
			m_handleCollisionsKernel->setArgument(1, m_halfVelocitiesBuffer1);
			m_handleCollisionsKernel->setArgument(2, m_oldHalfVelocitiesBuffer);
			m_handleCollisionsKernel->setArgument(3, m_velocitiesBuffer1);
			m_handleCollisionsKernel->setArgument(4, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_handleCollisionsKernel->setArgument(5, m_collisionBoxesBuffer);
			m_handleCollisionsKernel->setArgument(6, m_collisionSpheresBuffer);

			m_parallelComputationInterface->executeKernel(m_handleCollisionsKernel, m_dummyParticleCount, m_workGroupSize);
		}
	}

	ParticleCollisionData SPHSolver::handleCollision(ParticleCollisionData particleData)
	{
		particleData.radius = m_particleRadius;
		particleData.restitutionCoefficient = m_restitutionCoefficient;
		particleData.frictionCoefficient = m_frictionCoefficient;

		for (StaticCollisionObject* collisionObject : m_collisionObjects)
		{
			particleData = collisionObject->handleCollisionWithParticle(particleData);
		}

		return particleData;
	}

	void SPHSolver::onEndUpdate()
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			// clear grid and neighborlists
			m_spatialGrid.clear();

			// clear cachedNeighborLists
			for (std::vector<SPHParticle*> neightborList : m_cachedNeighborLists)
			{
				neightborList.clear();
			}
			m_cachedNeighborLists.clear();
		}
		else
		{
			// read particle data for rendering
			float4* positionsBuffer = new float4[m_particles.size()];
			float4* velocitesBuffer = new float4[m_particles.size()];
			float4* halfVelocitiesBuffer = new float4[m_particles.size()];
			unsigned int* isFirstTimeStepsBuffer = new unsigned int[m_particles.size()];

			m_parallelComputationInterface->readFromBuffer(m_positionsBuffer1, positionsBuffer, m_particles.size() * sizeof(float4), true);
			m_parallelComputationInterface->readFromBuffer(m_velocitiesBuffer1, velocitesBuffer, m_particles.size() * sizeof(float4), true);
			m_parallelComputationInterface->readFromBuffer(m_halfVelocitiesBuffer1, halfVelocitiesBuffer, m_particles.size() * sizeof(float4), true);
			m_parallelComputationInterface->readFromBuffer(m_isFirstTimeStepsBuffer1, isFirstTimeStepsBuffer, m_particles.size() * sizeof(unsigned int), true);
			m_parallelComputationInterface->waitUntilFinished();

			for (int i = 0; i < m_particles.size(); i++)
			{
				float4 position = positionsBuffer[i];
				float4 velocity = velocitesBuffer[i];
				float4 halfVelocity = halfVelocitiesBuffer[i];
				bool isFirstTimeStep = isFirstTimeStepsBuffer[i];
				m_particles[i]->setPosition(Vector3D(position.x, position.y, position.z));
				m_particles[i]->setVelocity(Vector3D(velocity.x, velocity.y, velocity.z));
				m_particles[i]->setHalfVelocity(Vector3D(halfVelocity.x, halfVelocity.y, halfVelocity.z));
				m_particles[i]->setIsFirstTimeStep(isFirstTimeStep);
			}

			delete[] positionsBuffer;
			delete[] velocitesBuffer;
			delete[] halfVelocitiesBuffer;
			delete[] isFirstTimeStepsBuffer;
		}
	}

	void SPHSolver::buildCachedNeighborLists()
	{
		m_spatialGrid.build(m_particles);

		// build cached neighbor lists
		for (int i = 0; i < m_particles.size(); i++)
		{
			m_cachedNeighborLists.push_back(m_spatialGrid.findNeighborParticles(m_particles[i], m_kernelRadius));
		}
	}

	void SPHSolver::calcParticleDensityPressure()
	{
		if (m_parallelizationType == ParallelizationType::NONE)
		{
			for (int i = 0; i < m_particles.size(); i++) {
				SPHParticle* particle = m_particles[i];

				// Measure the density with particles' current locations
				float weightedSum = 0.f;
				for (SPHParticle* neighborParticle : m_cachedNeighborLists[i])
				{
					float distance = (neighborParticle->getPosition() - particle->getPosition()).magnitude();
					weightedSum += m_defaultKernel.getKernelWeight(distance);
				}
				particle->setDensity(m_particleMass * weightedSum);

				// Compute pressure based on the density
				float pressure = m_pressureStiffnessCoefficient * (particle->getDensity() - m_restDensity);
				if (pressure < 0.f)
					pressure *= m_negativePressureFactor;
				particle->setPressure(pressure);
			}
		}
		else
		{
			//-------- CALC DENSITY AND PRESSURE --------
			m_calcDensityPressureKernel->setArgument(0, m_positionsBuffer1);
			m_calcDensityPressureKernel->setArgument(1, m_densitiesBuffer);
			m_calcDensityPressureKernel->setArgument(2, m_pressuresBuffer);
			m_calcDensityPressureKernel->setArgument(3, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_calcDensityPressureKernel->setArgument(4, m_cellListBuffer);
			m_calcDensityPressureKernel->setArgument(5, m_defaultKernelWeightsBuffer);
			m_calcDensityPressureKernel->setArgument(6, m_parallelSPHParameters.kernelWeightCount * sizeof(float), NULL);

			m_parallelComputationInterface->executeKernel(m_calcDensityPressureKernel, m_dummyParticleCount, m_workGroupSize);
		}
	}

	void SPHSolver::recalcParticleMass()
	{
		//compute mass
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

		float weightedSum = 0.f;
		for (Vector3D neighborPoint : neighborPoints)
		{
			float distance = neighborPoint.magnitude();
			weightedSum += m_defaultKernel.getKernelWeight(distance);
		}
        m_particleMass = m_restDensity / weightedSum;

		m_parallelSPHParameters.particleMass = m_particleMass;
	}

	void SPHSolver::recalcKernelRadius()
	{
		m_hasKernelWeightDataChanged = true;

		m_kernelRadius = m_kernelRadiusFactor * m_particleRadius;
		m_defaultKernel.setRadius(m_kernelRadius);
		m_pressureKernel.setRadius(m_kernelRadius);
		m_viscosityKernel.setRadius(m_kernelRadius);
		m_spatialGrid.setGridSpacing(m_kernelRadius);

		m_parallelSPHParameters.kernelRadius = m_kernelRadius;
		m_parallelSPHParameters.kernelDivisionStep = m_defaultKernel.getDivisionStep();

        recalcParticleMass();
	}

	// GETTER
	ParallelizationType SPHSolver::getParallelizationType() const
	{
		return m_parallelizationType;
	}

    Vector3D SPHSolver::getGravity() const
    {
        return m_gravity;
    }

    int SPHSolver::getParticleCount() const
    {
        return m_particles.size();
    }

	float SPHSolver::getParticleRadius() const
	{
        return m_particleRadius;
    }

    float SPHSolver::getParticleMass() const
    {
        return m_particleMass;
    }

	float SPHSolver::getKernelRadius() const
	{
		return m_kernelRadius;
	}

	float SPHSolver::getKernelRadiusFactor() const
	{
        return m_kernelRadiusFactor;
    }

    float SPHSolver::getRestDensity() const
	{
        return m_restDensity;
	}

	float SPHSolver::getViscosityCoefficient() const
	{
		return m_viscosityCoefficient;
	}

    float SPHSolver::getPressureStiffnessCoefficient() const
    {
        return m_pressureStiffnessCoefficient;
    }

    float SPHSolver::getNegativePressureFactor() const
    {
        return m_negativePressureFactor;
    }

    float SPHSolver::getSurfaceTensionCoefficient() const
    {
        return m_surfaceTensionCoefficient;
    }

    float SPHSolver::getSurfaceTensionThreshold() const
    {
        return m_surfaceTensionThreshold;
    }

	float SPHSolver::getResitutionCoefficient() const
	{
		return m_restitutionCoefficient;
	}

    float SPHSolver::getFrictionCoefficient() const
    {
        return m_frictionCoefficient;
    }

	bool SPHSolver::hasGPU() const
	{
		return m_parallelComputationInterface->hasGPU();
	}

	bool SPHSolver::hasCPU() const
	{
		return m_parallelComputationInterface->hasCPU();
	}
	
	// SETTER
	void SPHSolver::setHasCollisionObjectDataChanged(bool hasCollisionObjectDataChanged)
	{
		m_hasCollisionObjectDataChanged = hasCollisionObjectDataChanged;
	}

	void SPHSolver::setParallelizationType(ParallelizationType parallelizationType)
	{
		m_parallelizationType = parallelizationType;

		if (m_parallelizationType != ParallelizationType::NONE)
		{
			m_hasParallelContextChanged = true;
			reinitParallelContext();
		}
	}

    void SPHSolver::setGravity(const Vector3D& gravity)
    {
        m_gravity = gravity;

		m_parallelSPHParameters.gravity.x = m_gravity.getX();
		m_parallelSPHParameters.gravity.y = m_gravity.getY();
		m_parallelSPHParameters.gravity.z = m_gravity.getZ();
		m_parallelSPHParameters.gravity.w = 0.f;
    }

	void SPHSolver::setParticleRadius(float particleRadius)
	{
		m_particleRadius = particleRadius;

		m_parallelSPHParameters.particleRadius = m_particleRadius;

		recalcKernelRadius();
	}

	void SPHSolver::setKernelRadiusFactor(float kernelRadiusFactor)
	{
		m_kernelRadiusFactor = kernelRadiusFactor;

		recalcKernelRadius();
	}

    void SPHSolver::setRestDensity(float restDensity)
	{
        m_restDensity = restDensity;

		m_parallelSPHParameters.restDensity = m_restDensity;

		recalcParticleMass();
	}

	void SPHSolver::setViscosityCoefficient(float viscosityCoefficient)
	{
		m_viscosityCoefficient = viscosityCoefficient;

		m_parallelSPHParameters.viscosityCoefficient = m_viscosityCoefficient;
	}

    void SPHSolver::setPressureStiffnessCoefficient(float pressureStiffnessCoefficient)
    {
        m_pressureStiffnessCoefficient = pressureStiffnessCoefficient;

		m_parallelSPHParameters.pressureStiffnessCoefficient = m_pressureStiffnessCoefficient;
    }

    void SPHSolver::setNegativePressureFactor(float negativePressureFactor)
    {
        m_negativePressureFactor = negativePressureFactor;

		m_parallelSPHParameters.negativePressureFactor = m_negativePressureFactor;
    }

    void SPHSolver::setSurfaceTensionCoefficient(float surfaceTensionCoefficient)
    {
        m_surfaceTensionCoefficient = surfaceTensionCoefficient;

		m_parallelSPHParameters.surfaceTensionCoefficient = m_surfaceTensionCoefficient;
    }

    void SPHSolver::setSurfaceTensionThreshold(float surfaceTensionThreshold)
    {
        m_surfaceTensionThreshold = surfaceTensionThreshold;

		m_parallelSPHParameters.surfaceTensionThreshold = m_surfaceTensionThreshold;
    }

	void SPHSolver::setRestitutionCoefficient(float resitutionCoefficient)
	{
		m_restitutionCoefficient = resitutionCoefficient;

		m_parallelSPHParameters.restitutionCoefficient = m_restitutionCoefficient;
	}

    void SPHSolver::setFrictionCoefficient(float frictionCoefficient)
    {
        m_frictionCoefficient = frictionCoefficient;

		m_parallelSPHParameters.frictionCoefficient = m_frictionCoefficient;
    }

	// OPEN CL METHODS
	void SPHSolver::reinitParallelContext()
	{
		ParallelDeviceType deviceType;

		if (m_parallelizationType == ParallelizationType::CPU)
		{
			deviceType = ParallelDeviceType::CPU;
		}
		else
		{
			deviceType = ParallelDeviceType::GPU;
		}

		std::ifstream kernelFile("cl_kernels/SPHKernels.cl");
		std::string kernelString(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));
		ParallelSources sources;
		sources.push_back(std::make_pair(kernelString.c_str(), kernelString.length() + 1));

		m_parallelComputationInterface->reinitContext(sources, deviceType, true);
		
		if (m_calcGridIndicesKernel)
			delete m_calcGridIndicesKernel;
		if (m_countDigitsInBucketsKernel)
			delete m_countDigitsInBucketsKernel;
		if (m_scanBucketsKernel)
			delete m_scanBucketsKernel;
		if (m_permuteParticlesKernel)
			delete m_permuteParticlesKernel;
		if (m_buildCellListKernel)
			delete m_buildCellListKernel;
		if (m_calcDensityPressureKernel)
			delete m_calcDensityPressureKernel;
		if (m_accumulateNonPressureForcesKernel)
			delete m_accumulateNonPressureForcesKernel;
		if (m_accumulatePressureForcesKernel)
			delete m_accumulatePressureForcesKernel;
		if (m_integrateKernel)
			delete m_integrateKernel;
		if (m_handleCollisionsKernel)
			delete m_handleCollisionsKernel;

		m_calcGridIndicesKernel = m_parallelComputationInterface->createKernel("calcGridIndices");
		m_countDigitsInBucketsKernel = m_parallelComputationInterface->createKernel("countDigitsInBuckets");
		m_scanBucketsKernel = m_parallelComputationInterface->createKernel("scanBuckets");
		m_permuteParticlesKernel = m_parallelComputationInterface->createKernel("permuteParticles");
		m_buildCellListKernel = m_parallelComputationInterface->createKernel("buildCellList");
		m_calcDensityPressureKernel = m_parallelComputationInterface->createKernel("calcDensityPressure");
		m_accumulateNonPressureForcesKernel = m_parallelComputationInterface->createKernel("accumulateNonPressureForces");
		m_accumulatePressureForcesKernel = m_parallelComputationInterface->createKernel("accumulatePressureForces");
		m_integrateKernel = m_parallelComputationInterface->createKernel("integrate");
		m_handleCollisionsKernel = m_parallelComputationInterface->createKernel("handleCollisions");

		unsigned int kernelWeightsBufferSize = m_defaultKernel.getKernelWeights().size() * sizeof(float);

		if (m_bucketCountsBuffer)
			delete m_bucketCountsBuffer;
		if (m_defaultKernelWeightsBuffer)
			delete m_defaultKernelWeightsBuffer;
		if (m_defaultKernelFirstDerivativeWeightsBuffer)
			delete m_defaultKernelFirstDerivativeWeightsBuffer;
		if (m_defaultKernelSecondDerivativeWeightsBuffer)
			delete m_defaultKernelSecondDerivativeWeightsBuffer;
		if (m_pressureKernelFirstDerivativeWeightsBuffer)
			delete m_pressureKernelFirstDerivativeWeightsBuffer;
		if (m_viscosityKernelSecondDerivativeWeightsBuffer)
			delete m_viscosityKernelSecondDerivativeWeightsBuffer;

		m_bucketCountsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_radixThreadCount * m_radixBucketCount * sizeof(unsigned int));
		m_defaultKernelWeightsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_ONLY, kernelWeightsBufferSize);
		m_defaultKernelFirstDerivativeWeightsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_ONLY, kernelWeightsBufferSize);
		m_defaultKernelSecondDerivativeWeightsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_ONLY, kernelWeightsBufferSize);
		m_pressureKernelFirstDerivativeWeightsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_ONLY, kernelWeightsBufferSize);
		m_viscosityKernelSecondDerivativeWeightsBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_ONLY, kernelWeightsBufferSize);
	}

	void SPHSolver::initParallelBuffers()
	{
		// PARTICLES
		if (m_hasParallelContextChanged || m_hasParticleDataChanged)
		{
			m_hasParticleDataChanged = false;

			// Write particle data into temp buffer
			m_dummyParticleCount = m_particles.size();
			if (m_dummyParticleCount < 512)
			{
				m_dummyParticleCount = 512;
			}
			if (m_dummyParticleCount % m_workGroupSize != 0)
			{
				m_dummyParticleCount += (m_workGroupSize - (m_dummyParticleCount % m_workGroupSize));
			}

			float4* positionsBuffer = new float4[m_dummyParticleCount];
			float4* velocitesBuffer = new float4[m_dummyParticleCount];
			float4* halfVelocitiesBuffer = new float4[m_dummyParticleCount];
			unsigned int* isFirstTimeStepsBuffer = new unsigned int[m_dummyParticleCount];
			for (int i = 0; i < m_particles.size(); i++)
			{
				positionsBuffer[i].x = m_particles[i]->getPosition().getX();
				positionsBuffer[i].y = m_particles[i]->getPosition().getY();
				positionsBuffer[i].z = m_particles[i]->getPosition().getZ();
				positionsBuffer[i].w = 0.f;

				velocitesBuffer[i].x = m_particles[i]->getVelocity().getX();
				velocitesBuffer[i].y = m_particles[i]->getVelocity().getY();
				velocitesBuffer[i].z = m_particles[i]->getVelocity().getZ();
				velocitesBuffer[i].w = 0.f;

				halfVelocitiesBuffer[i].x = m_particles[i]->getHalfVelocity().getX();
				halfVelocitiesBuffer[i].y = m_particles[i]->getHalfVelocity().getY();
				halfVelocitiesBuffer[i].z = m_particles[i]->getHalfVelocity().getZ();
				halfVelocitiesBuffer[i].w = 0.f;

				isFirstTimeStepsBuffer[i] = m_particles[i]->getIsFirstTimeStep();
			}

			if (m_positionsBuffer1)
				delete m_positionsBuffer1;
			if (m_positionsBuffer2)
				delete m_positionsBuffer2;
			if (m_velocitiesBuffer1)
				delete m_velocitiesBuffer1;
			if (m_velocitiesBuffer2)
				delete m_velocitiesBuffer2;
			if (m_halfVelocitiesBuffer1)
				delete m_halfVelocitiesBuffer1;
			if (m_halfVelocitiesBuffer2)
				delete m_halfVelocitiesBuffer2;
			if (m_isFirstTimeStepsBuffer1)
				delete m_isFirstTimeStepsBuffer1;
			if (m_isFirstTimeStepsBuffer2)
				delete m_isFirstTimeStepsBuffer2;
			if (m_gridIndicesBuffer1)
				delete m_gridIndicesBuffer1;
			if (m_gridIndicesBuffer2)
				delete m_gridIndicesBuffer2;
			if (m_oldHalfVelocitiesBuffer)
				delete m_oldHalfVelocitiesBuffer;
			if (m_accumulatedForcesBuffer)
				delete m_accumulatedForcesBuffer;
			if (m_densitiesBuffer)
				delete m_densitiesBuffer;
			if (m_pressuresBuffer)
				delete m_pressuresBuffer;

			// Create new OpenCL buffers because the size might have changed
			m_positionsBuffer1 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_positionsBuffer2 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_velocitiesBuffer1 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_velocitiesBuffer2 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_halfVelocitiesBuffer1 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_halfVelocitiesBuffer2 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_isFirstTimeStepsBuffer1 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(unsigned int));
			m_isFirstTimeStepsBuffer2 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(unsigned int));
			m_gridIndicesBuffer1 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(unsigned int));
			m_gridIndicesBuffer2 = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(unsigned int));
			m_oldHalfVelocitiesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_accumulatedForcesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float4));
			m_densitiesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float));
			m_pressuresBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_dummyParticleCount * sizeof(float));

			// Write data to Multiprocessor Device
			m_parallelComputationInterface->writeToBuffer(m_positionsBuffer1, positionsBuffer, m_dummyParticleCount * sizeof(float4), true);
			m_parallelComputationInterface->writeToBuffer(m_velocitiesBuffer1, velocitesBuffer, m_dummyParticleCount * sizeof(float4), true);
			m_parallelComputationInterface->writeToBuffer(m_halfVelocitiesBuffer1, halfVelocitiesBuffer, m_dummyParticleCount * sizeof(float4), true);
			m_parallelComputationInterface->writeToBuffer(m_isFirstTimeStepsBuffer1, isFirstTimeStepsBuffer, m_dummyParticleCount * sizeof(unsigned int), true);

			// Delete dynamically created temporary arrays
			delete[] positionsBuffer;
			delete[] velocitesBuffer;
			delete[] halfVelocitiesBuffer;
			delete[] isFirstTimeStepsBuffer;
		}

		// KERNEL WEIGHTS
		if (m_hasParallelContextChanged || m_hasKernelWeightDataChanged)
		{
			m_hasKernelWeightDataChanged = false;

			// Write kernel data into temp buffers
			std::vector<float> defaultKernelWeights = m_defaultKernel.getKernelWeights();
			std::vector<float> defaultKernelFirstDerivativeWeights = m_defaultKernel.getFirstDerivativeWeights();
			std::vector<float> defaultKernelSecondDerivativeWeights = m_defaultKernel.getSecondDerivativeWeights();
			std::vector<float> pressureKernelFirstDerivativeWeights = m_pressureKernel.getFirstDerivativeWeights();
			std::vector<float> viscosityKernelSecondDerivativeWeights = m_viscosityKernel.getSecondDerivativeWeights();
			float* defaultKernelWeightsBuffer = new float[defaultKernelWeights.size()];
			float* defaultKernelFirstDerivativeWeightsBuffer = new float[defaultKernelFirstDerivativeWeights.size()];
			float* defaultKernelSecondDerivativeWeightsBuffer = new float[defaultKernelSecondDerivativeWeights.size()];
			float* pressureKernelFirstDerivativeWeightsBuffer = new float[pressureKernelFirstDerivativeWeights.size()];
			float* viscosityKernelSecondDerivativeWeightsBuffer = new float[viscosityKernelSecondDerivativeWeights.size()];
			for (int i = 0; i < defaultKernelWeights.size(); i++)
			{
				defaultKernelWeightsBuffer[i] = defaultKernelWeights[i];
				defaultKernelFirstDerivativeWeightsBuffer[i] = defaultKernelFirstDerivativeWeights[i];
				defaultKernelSecondDerivativeWeightsBuffer[i] = defaultKernelSecondDerivativeWeights[i];
				pressureKernelFirstDerivativeWeightsBuffer[i] = pressureKernelFirstDerivativeWeights[i];
				viscosityKernelSecondDerivativeWeightsBuffer[i] = viscosityKernelSecondDerivativeWeights[i];
			}

			m_parallelSPHParameters.kernelWeightCount = defaultKernelWeights.size();

			// Write data to Multiprocessor Device -> TODO: Only write data to GPU/CPU if data has changed or mode has changed from NONE to GPU or CPU
			unsigned int kernelWeightsBufferSize = defaultKernelWeights.size() * sizeof(float);

			m_parallelComputationInterface->writeToBuffer(m_defaultKernelWeightsBuffer, defaultKernelWeightsBuffer, kernelWeightsBufferSize, true);
			m_parallelComputationInterface->writeToBuffer(m_defaultKernelFirstDerivativeWeightsBuffer, defaultKernelFirstDerivativeWeightsBuffer, kernelWeightsBufferSize, true);
			m_parallelComputationInterface->writeToBuffer(m_defaultKernelSecondDerivativeWeightsBuffer, defaultKernelSecondDerivativeWeightsBuffer, kernelWeightsBufferSize, true);
			m_parallelComputationInterface->writeToBuffer(m_pressureKernelFirstDerivativeWeightsBuffer, pressureKernelFirstDerivativeWeightsBuffer, kernelWeightsBufferSize, true);
			m_parallelComputationInterface->writeToBuffer(m_viscosityKernelSecondDerivativeWeightsBuffer, viscosityKernelSecondDerivativeWeightsBuffer, kernelWeightsBufferSize, true);

			// Delete dynamically created temporary arrays
			delete[] defaultKernelWeightsBuffer;
			delete[] defaultKernelFirstDerivativeWeightsBuffer;
			delete[] defaultKernelSecondDerivativeWeightsBuffer;
			delete[] pressureKernelFirstDerivativeWeightsBuffer;
			delete[] viscosityKernelSecondDerivativeWeightsBuffer;
		}

		// COLLISION OBJECTS
		if (m_hasParallelContextChanged || m_hasCollisionObjectDataChanged)
		{
			m_hasCollisionObjectDataChanged = false;

			// Write collision object data into temp buffers
			std::vector<StaticCollisionBox*> collisionBoxes;
			std::vector<StaticCollisionSphere*> collisionSpheres;
			for (int i = 0; i < m_collisionObjects.size(); i++)
			{
				if (dynamic_cast<StaticCollisionBox*>(m_collisionObjects[i]))
				{
					collisionBoxes.push_back(dynamic_cast<StaticCollisionBox*>(m_collisionObjects[i]));
				}
				else if (dynamic_cast<StaticCollisionSphere*>(m_collisionObjects[i]))
				{
					collisionSpheres.push_back(dynamic_cast<StaticCollisionSphere*>(m_collisionObjects[i]));
				}
			}

			ParallelSPHCollisionBox* collisionBoxesBuffer = new ParallelSPHCollisionBox[collisionBoxes.size()];
			for (int i = 0; i < collisionBoxes.size(); i++)
			{
				collisionBoxesBuffer[i].position.x = collisionBoxes[i]->getPosition().getX();
				collisionBoxesBuffer[i].position.y = collisionBoxes[i]->getPosition().getY();
				collisionBoxesBuffer[i].position.z = collisionBoxes[i]->getPosition().getZ();
				collisionBoxesBuffer[i].position.w = 0.f;

				collisionBoxesBuffer[i].halfDimensions.x = collisionBoxes[i]->getHalfDimensions().getX();
				collisionBoxesBuffer[i].halfDimensions.y = collisionBoxes[i]->getHalfDimensions().getY();
				collisionBoxesBuffer[i].halfDimensions.z = collisionBoxes[i]->getHalfDimensions().getZ();
				collisionBoxesBuffer[i].halfDimensions.w = 0.f;

				if (collisionBoxes[i]->getType() == StaticCollisionObjectType::BOUNDARY)
					collisionBoxesBuffer[i].isBoundary = true;
				else
					collisionBoxesBuffer[i].isBoundary = false;
			}

			ParallelSPHCollisionSphere* collisionSpheresBuffer = new ParallelSPHCollisionSphere[collisionSpheres.size()];
			for (int i = 0; i < collisionSpheres.size(); i++)
			{
				collisionSpheresBuffer[i].position.x = collisionSpheres[i]->getPosition().getX();
				collisionSpheresBuffer[i].position.y = collisionSpheres[i]->getPosition().getY();
				collisionSpheresBuffer[i].position.z = collisionSpheres[i]->getPosition().getZ();
				collisionSpheresBuffer[i].position.w = 0.f;

				collisionSpheresBuffer[i].radius = collisionSpheres[i]->getRadius();

				if (collisionSpheres[i]->getType() == StaticCollisionObjectType::BOUNDARY)
					collisionSpheresBuffer[i].isBoundary = true;
				else
					collisionSpheresBuffer[i].isBoundary = false;
			}

			m_parallelSPHParameters.collisionBoxCount = collisionBoxes.size();
			m_parallelSPHParameters.collisionSphereCount = collisionSpheres.size();

			if (m_collisionBoxesBuffer)
				delete m_collisionBoxesBuffer;
			if (m_collisionSpheresBuffer)
				delete m_collisionSpheresBuffer;

			m_collisionBoxesBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, collisionBoxes.size() * sizeof(ParallelSPHCollisionBox));
			m_collisionSpheresBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, collisionSpheres.size() * sizeof(ParallelSPHCollisionSphere));

			m_parallelComputationInterface->writeToBuffer(m_collisionBoxesBuffer, collisionBoxesBuffer, collisionBoxes.size() * sizeof(ParallelSPHCollisionBox), true);
			m_parallelComputationInterface->writeToBuffer(m_collisionSpheresBuffer, collisionSpheresBuffer, collisionSpheres.size() * sizeof(ParallelSPHCollisionSphere), true);

			delete[] collisionBoxesBuffer;
			delete[] collisionSpheresBuffer;
		}

		m_hasParallelContextChanged = false;
	}

	void SPHSolver::buildParallelGrid()
	{
		//-------- BUILD GRID --------#
		// Calc new grid size
		float minX, maxX, minY, maxY, minZ, maxZ;
		minX = minY = minZ = FLT_MAX;
		maxX = maxY = maxZ = FLT_MIN;

		for (SPHParticle* particle : m_particles)
		{
			float xPos = particle->getPosition().getX();
			float yPos = particle->getPosition().getY();
			float zPos = particle->getPosition().getZ();

			if (xPos < minX) minX = xPos;
			if (yPos < minY) minY = yPos;
			if (zPos < minZ) minZ = zPos;

			if (xPos > maxX) maxX = xPos;
			if (yPos > maxY) maxY = yPos;
			if (zPos > maxZ) maxZ = zPos;
		}

		m_parallelSPHParameters.gridSpacing = m_kernelRadius * m_gridSpacingFactor;
		m_parallelSPHParameters.gridOffset.x = -minX;
		m_parallelSPHParameters.gridOffset.y = -minY;
		m_parallelSPHParameters.gridOffset.z = -minZ;
		m_parallelSPHParameters.gridOffset.w = 0.f;
		m_parallelSPHParameters.gridSize.x = (int)((maxX - minX) / m_parallelSPHParameters.gridSpacing) + 1;
		m_parallelSPHParameters.gridSize.y = (int)((maxY - minY) / m_parallelSPHParameters.gridSpacing) + 1;
		m_parallelSPHParameters.gridSize.z = (int)((maxZ - minZ) / m_parallelSPHParameters.gridSpacing) + 1;
		m_parallelSPHParameters.gridSize.w = 0;
		m_parallelSPHParameters.cellCount = m_parallelSPHParameters.gridSize.x * m_parallelSPHParameters.gridSize.y * m_parallelSPHParameters.gridSize.z;

		// Calc grid indices for particles
		m_calcGridIndicesKernel->setArgument(0, m_positionsBuffer1);
		m_calcGridIndicesKernel->setArgument(1, m_gridIndicesBuffer1);
		m_calcGridIndicesKernel->setArgument(2, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);

		m_parallelComputationInterface->executeKernel(m_calcGridIndicesKernel, m_dummyParticleCount, m_workGroupSize);

		// Sort Particles by grid index with Radix Sort
		unsigned int* bucketCountBuffer = new unsigned int[m_radixThreadCount * m_radixBucketCount];

		for (int pass = 0; pass < m_radixPassCount; pass++)
		{
			// Count digits of grid index in buckets
			m_countDigitsInBucketsKernel->setArgument(0, m_gridIndicesBuffer1);
			m_countDigitsInBucketsKernel->setArgument(1, m_bucketCountsBuffer);
			m_countDigitsInBucketsKernel->setArgument(2, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_countDigitsInBucketsKernel->setArgument(3, sizeof(m_radixThreadCount), &m_radixThreadCount);
			m_countDigitsInBucketsKernel->setArgument(4, sizeof(pass), &pass);
			m_countDigitsInBucketsKernel->setArgument(5, sizeof(m_radixWidth), &m_radixWidth);

			m_parallelComputationInterface->fillBuffer(m_bucketCountsBuffer, 0, m_radixThreadCount * m_radixBucketCount * sizeof(unsigned int));

			m_parallelComputationInterface->executeKernel(m_countDigitsInBucketsKernel, m_radixThreadCount);

			m_parallelComputationInterface->readFromBuffer(m_bucketCountsBuffer, bucketCountBuffer, m_radixThreadCount * m_radixBucketCount * sizeof(unsigned int), true);
			m_parallelComputationInterface->waitUntilFinished();

			// Scan buckets
			unsigned int sum = 0;
			for (int i = 0; i < m_radixThreadCount * m_radixBucketCount; i++)
			{
				unsigned int val = bucketCountBuffer[i];
				bucketCountBuffer[i] = sum;
				sum += val;
			}

			m_parallelComputationInterface->writeToBuffer(m_bucketCountsBuffer, bucketCountBuffer, m_radixThreadCount * m_radixBucketCount * sizeof(unsigned int), true);

			// Permute particles
			m_permuteParticlesKernel->setArgument(0, m_positionsBuffer1);
			m_permuteParticlesKernel->setArgument(1, m_positionsBuffer2);
			m_permuteParticlesKernel->setArgument(2, m_velocitiesBuffer1);
			m_permuteParticlesKernel->setArgument(3, m_velocitiesBuffer2);
			m_permuteParticlesKernel->setArgument(4, m_halfVelocitiesBuffer1);
			m_permuteParticlesKernel->setArgument(5, m_halfVelocitiesBuffer2);
			m_permuteParticlesKernel->setArgument(6, m_isFirstTimeStepsBuffer1);
			m_permuteParticlesKernel->setArgument(7, m_isFirstTimeStepsBuffer2);
			m_permuteParticlesKernel->setArgument(8, m_gridIndicesBuffer1);
			m_permuteParticlesKernel->setArgument(9, m_gridIndicesBuffer2);
			m_permuteParticlesKernel->setArgument(10, m_bucketCountsBuffer);
			m_permuteParticlesKernel->setArgument(11, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
			m_permuteParticlesKernel->setArgument(12, sizeof(m_radixThreadCount), &m_radixThreadCount);
			m_permuteParticlesKernel->setArgument(13, sizeof(pass), &pass);
			m_permuteParticlesKernel->setArgument(14, sizeof(m_radixWidth), &m_radixWidth);

			m_parallelComputationInterface->executeKernel(m_permuteParticlesKernel, m_radixThreadCount);

			// Swap Particle Buffers
			ParallelBuffer* temp = m_positionsBuffer1;
			m_positionsBuffer1 = m_positionsBuffer2;
			m_positionsBuffer2 = temp;

			temp = m_velocitiesBuffer1;
			m_velocitiesBuffer1 = m_velocitiesBuffer2;
			m_velocitiesBuffer2 = temp;

			temp = m_halfVelocitiesBuffer1;
			m_halfVelocitiesBuffer1 = m_halfVelocitiesBuffer2;
			m_halfVelocitiesBuffer2 = temp;

			temp = m_isFirstTimeStepsBuffer1;
			m_isFirstTimeStepsBuffer1 = m_isFirstTimeStepsBuffer2;
			m_isFirstTimeStepsBuffer2 = temp;

			temp = m_gridIndicesBuffer1;
			m_gridIndicesBuffer1 = m_gridIndicesBuffer2;
			m_gridIndicesBuffer2 = temp;
		}
		delete bucketCountBuffer;

		// Build cell list
		if (m_cellListBuffer)
			delete m_cellListBuffer;
		m_cellListBuffer = m_parallelComputationInterface->createBuffer(ParallelBufferType::READ_WRITE, m_parallelSPHParameters.cellCount * sizeof(unsigned int));

		m_buildCellListKernel->setArgument(0, m_gridIndicesBuffer1);
		m_buildCellListKernel->setArgument(1, sizeof(m_parallelSPHParameters), &m_parallelSPHParameters);
		m_buildCellListKernel->setArgument(2, m_cellListBuffer);

		m_parallelComputationInterface->executeKernel(m_buildCellListKernel, m_dummyParticleCount, m_workGroupSize);
	}
}

#pragma once

#include "SPHSolver.h"
#include "Particles/PCISPHParticle.h"

namespace LiPhEn {
	class PCISPHSolver : public SPHSolver
	{
	public:
		PCISPHSolver();
		~PCISPHSolver();

		virtual void addParticle(SPHParticle* particle);
		void addParticle(PCISPHParticle* particle);

		int getMinIterations() const;
		float getMaxDensityErrorRatio() const;

		void setMinIterations(int maxIterations);
		void setMaxDensityErrorRatio(float maxDensityErrorRatio);

	protected:
		virtual void accumulatePressureForces(float deltaTime);
		virtual void reinitParallelContext();
		virtual void initParallelBuffers();

	private:
		float calcDelta(float deltaTime);

		int m_minIterations;
		float m_maxDensityErrorRatio;

		ParallelBuffer* m_predictedPositionsBuffer;
		ParallelBuffer* m_predictedHalfVelocitiesBuffer;
		ParallelBuffer* m_predictedPressureForcesBuffer;
		ParallelBuffer* m_predictedDensitiesBuffer;

		ParallelKernel* m_pciInitKernel;
		ParallelKernel* m_pciIntegrateKernel;
		ParallelKernel* m_pciHandleCollisionsKernel;
		ParallelKernel* m_pciCalcDensityPressureKernel;
		ParallelKernel* m_pciCalcPressureForceKernel;
		ParallelKernel* m_pciAddPressureForceKernel;
	};
}
#pragma once

#include "PhysicSolver.h"
#include "Particles/SPHParticle.h"
#include "Kernels/DefaultKernel.h"
#include "Kernels/PressureKernel.h"
#include "Kernels/ViscosityKernel.h"
#include "SPHSpatialGrid.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "Collision/StaticCollisionBox.h"
#include "Collision/StaticCollisionSphere.h"
#include "Parallelization/ParallelComputationInterface.h"
#include "Parallelization/ParallelSPHStructs.h"
#include <iostream>

namespace LiPhEn {
	enum class ParallelizationType {
		NONE,
		CPU,
		GPU
	};

    class SPHSolver : public PhysicSolver
	{
	public:
		SPHSolver();
		~SPHSolver();

		virtual void addParticle(SPHParticle* particle);
		void removeParticles();

		void addStaticCollisionObject(StaticCollisionObject* collisionObject);
		void removeStaticCollisionObjects();

        void cleanUp();

		ParallelizationType getParallelizationType() const;
        Vector3D getGravity() const;
        int getParticleCount() const;
		float getParticleRadius() const;
        float getParticleMass() const;
		float getKernelRadius() const;
		float getKernelRadiusFactor() const;
        float getRestDensity() const;
		float getViscosityCoefficient() const;
        float getPressureStiffnessCoefficient() const;
        float getNegativePressureFactor() const;
        float getSurfaceTensionCoefficient() const;
        float getSurfaceTensionThreshold() const;
		float getResitutionCoefficient() const;
        float getFrictionCoefficient() const;
		bool hasGPU() const;
		bool hasCPU() const;

		void setHasCollisionObjectDataChanged(bool hasCollisionObjectDataChanged);
		void setParallelizationType(ParallelizationType parallelizationType);
        void setGravity(const Vector3D& gravity);
		void setParticleRadius(float particleRadius);
		void setKernelRadiusFactor(float kernelRadiusFactor);
        void setRestDensity(float restDensity);
		void setViscosityCoefficient(float viscosityCoefficient);
        void setPressureStiffnessCoefficient(float pressureStiffnessCoefficient);
        void setNegativePressureFactor(float negativePressureFactor);
        void setSurfaceTensionCoefficient(float surfaceTensionCoefficient);
        void setSurfaceTensionThreshold(float surfaceTensionThreshold);
        void setRestitutionCoefficient(float resitutionCoefficient);
        void setFrictionCoefficient(float frictionCoefficient);

	protected:
		virtual void onBeginUpdate();
		void accumulateNonPressureForces(float deltaTime);
		virtual void accumulatePressureForces(float deltaTime);
		virtual void integrate(float deltaTime);
		virtual void handleCollisions();
		virtual void onEndUpdate();

		ParticleCollisionData handleCollision(ParticleCollisionData particleData);

		std::vector<SPHParticle*> m_particles;
		SPHSpatialGrid m_spatialGrid;
		std::vector<std::vector<SPHParticle*>> m_cachedNeighborLists;
		DefaultKernel m_defaultKernel;
		PressureKernel m_pressureKernel;
		ViscosityKernel m_viscosityKernel;
		float m_particleRadius;
		float m_kernelRadius;
		float m_kernelRadiusFactor;
		float m_gridSpacingFactor;
        float m_restDensity;
		float m_particleMass;
        float m_pressureStiffnessCoefficient;
		float m_negativePressureFactor;
		float m_viscosityCoefficient;
		float m_surfaceTensionCoefficient;
		float m_surfaceTensionThreshold;
		float m_restitutionCoefficient;
        float m_frictionCoefficient;
        Vector3D m_gravity;

		// OpenCL
		virtual void reinitParallelContext();
		virtual void initParallelBuffers();
		void buildParallelGrid();

		ParallelizationType m_parallelizationType;

		ParallelComputationInterface* m_parallelComputationInterface;

		ParallelBuffer* m_positionsBuffer1;
		ParallelBuffer* m_positionsBuffer2;
		ParallelBuffer* m_velocitiesBuffer1;
		ParallelBuffer* m_velocitiesBuffer2;
		ParallelBuffer* m_halfVelocitiesBuffer1;
		ParallelBuffer* m_halfVelocitiesBuffer2;
		ParallelBuffer* m_isFirstTimeStepsBuffer1;
		ParallelBuffer* m_isFirstTimeStepsBuffer2;
		ParallelBuffer* m_gridIndicesBuffer1;
		ParallelBuffer* m_gridIndicesBuffer2;
		ParallelBuffer* m_oldHalfVelocitiesBuffer;
		ParallelBuffer* m_accumulatedForcesBuffer;
		ParallelBuffer* m_densitiesBuffer;
		ParallelBuffer* m_pressuresBuffer;

		ParallelBuffer* m_defaultKernelWeightsBuffer;
		ParallelBuffer* m_defaultKernelFirstDerivativeWeightsBuffer;
		ParallelBuffer* m_defaultKernelSecondDerivativeWeightsBuffer;
		ParallelBuffer* m_pressureKernelFirstDerivativeWeightsBuffer;
		ParallelBuffer* m_viscosityKernelSecondDerivativeWeightsBuffer;
		ParallelBuffer* m_collisionBoxesBuffer;
		ParallelBuffer* m_collisionSpheresBuffer;
		ParallelBuffer* m_bucketCountsBuffer;
		ParallelBuffer* m_cellListBuffer;

		ParallelKernel* m_calcGridIndicesKernel;
		ParallelKernel* m_countDigitsInBucketsKernel;
		ParallelKernel* m_scanBucketsKernel;
		ParallelKernel* m_permuteParticlesKernel;
		ParallelKernel* m_buildCellListKernel;
		ParallelKernel* m_calcDensityPressureKernel;
		ParallelKernel* m_accumulateNonPressureForcesKernel;
		ParallelKernel* m_accumulatePressureForcesKernel;
		ParallelKernel* m_integrateKernel;
		ParallelKernel* m_handleCollisionsKernel;

		ParallelSPHParameters m_parallelSPHParameters;
		unsigned int m_radixThreadCount;
		unsigned int m_radixWidth;
		unsigned int m_radixBucketCount;
		unsigned int m_radixPassCount;
		unsigned int m_workGroupSize;
		unsigned int m_dummyParticleCount;

		bool m_hasParallelContextChanged;
		bool m_hasCollisionObjectDataChanged;
		bool m_hasParticleDataChanged;
		bool m_hasKernelWeightDataChanged;

	private:
		virtual void accumulateForces(float deltaTime);
		void buildCachedNeighborLists();
		void calcParticleDensityPressure();
		void recalcParticleMass();
		void recalcKernelRadius();

		std::vector<StaticCollisionObject*> m_collisionObjects;
	};
}

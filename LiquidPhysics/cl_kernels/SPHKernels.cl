#define cl_float float
#define cl_float4 float4
#define cl_uint unsigned int
#define cl_uint4 uint4
#define cl_int int
#define cl_bool unsigned int

typedef struct {
	cl_float4 gravity;						// 16 Byte
	cl_float4 gridOffset;					// 32 Byte
	cl_uint4 gridSize;						// 48 Byte
	cl_float particleMass;					// 52 Byte
	cl_float particleRadius;				// 56 Byte
	cl_float kernelRadius;					// 60 Byte
	cl_float kernelDivisionStep;			// 64 Byte
	cl_float gridSpacing;					// 68 Byte
	cl_float restDensity;					// 72 Byte
	cl_float pressureStiffnessCoefficient;	// 76 Byte
	cl_float negativePressureFactor;		// 80 Byte
	cl_float viscosityCoefficient;			// 84 Byte
	cl_float surfaceTensionCoefficient;		// 88 Byte
	cl_float surfaceTensionThreshold;		// 92 Byte
	cl_float restitutionCoefficient;		// 96 Byte
	cl_float frictionCoefficient;			// 100 Byte
	cl_float particleCount;					// 104 Byte
	cl_uint cellCount;						// 108 Byte
	cl_uint collisionBoxCount;				// 112 Byte
	cl_uint collisionSphereCount;			// 116 Byte
	cl_uint kernelWeightCount;				// 120 Byte
	cl_float dummy1, dummy2;				// 128 Byte
} ParallelSPHParameters;

typedef struct {
	cl_float4 position;					// 16 Byte
	cl_float4 halfDimensions;			// 32 Byte
	cl_bool isBoundary;					// 36 Byte
	cl_float dummy1, dummy2, dummy3;	// 48 Byte
	cl_float4 dummy4;					// 64 Byte
} ParallelSPHCollisionBox;

typedef struct {
	cl_float4 position;					// 16 Byte
	cl_float radius;					// 20 Byte
	cl_bool isBoundary;					// 24 Byte
	cl_float dummy1, dummy2;			// 32 Btye
} ParallelSPHCollisionSphere;

void handleCollisionWithBox(cl_float4* position,
	cl_float4* velocity,
	const ParallelSPHParameters params,
	const ParallelSPHCollisionBox collisionBox);

void handleCollisionWithSphere(cl_float4* position,
	cl_float4* velocity,
	const ParallelSPHParameters params,
	const ParallelSPHCollisionSphere collisionSphere);

void resolveCollision(cl_float4* position,
	cl_float4* velocity,
	const ParallelSPHParameters params,
	const cl_float4 collisionNormal,
	const cl_float4 collisionPoint);

// ----------- BUILD GRID --------------

__kernel void calcGridIndices(__global const cl_float4* inPositions,
							  __global cl_uint* outGridIndices,
							  const ParallelSPHParameters params)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		cl_float4 position = inPositions[i];

		cl_uint xGrid = trunc((position.x + params.gridOffset.x) / params.gridSpacing);
		cl_uint yGrid = trunc((position.y + params.gridOffset.y) / params.gridSpacing);
		cl_uint zGrid = trunc((position.z + params.gridOffset.z) / params.gridSpacing);

		outGridIndices[i] = xGrid + params.gridSize.x * yGrid + params.gridSize.x * params.gridSize.y * zGrid;
	}	
}

__kernel void countDigitsInBuckets(__global const cl_uint* inGridIndices, 
								   __global cl_uint* bucketCounts,
								   const ParallelSPHParameters params,
								   const cl_uint threadCount,
								   const cl_uint passNumber,
								   const cl_uint radixWidth)
{
	const cl_uint i = get_global_id(0);

	cl_uint particlesPerThread = params.particleCount / threadCount;
	cl_uint startIndex = particlesPerThread * i;
	cl_uint endIndex = startIndex + particlesPerThread - 1;

	if (i == threadCount - 1) {
		endIndex = params.particleCount - 1;
	}

	for (cl_uint j = startIndex; j <= endIndex; j++) {
		cl_uint bucket = (inGridIndices[j] & (0xFF << (passNumber * radixWidth))) >> (passNumber * radixWidth);

		++(bucketCounts[bucket * threadCount + i]);
	}
}

__kernel void permuteParticles(__global const cl_float4* inPositions,
							   __global cl_float4* outPositions,
							   __global const cl_float4* inVelocities,
							   __global cl_float4* outVelocities,
							   __global const cl_float4* inHalfVelocities,
							   __global cl_float4* outHalfVelocities,
							   __global const cl_bool* inIsFirstTimeSteps,
							   __global cl_bool* outIsFirstTimeSteps,
							   __global const cl_uint* inGridIndices,
							   __global cl_uint* outGridIndices,
							   __global cl_uint* scannedBuckets,
							   const ParallelSPHParameters params,
							   const cl_uint threadCount,
							   const cl_uint passNumber,
							   const cl_uint radixWidth)
{
	const cl_uint i = get_global_id(0);

	cl_uint particlesPerThread = params.particleCount / threadCount;
	cl_uint startIndex = particlesPerThread * i;
	cl_uint endIndex = startIndex + particlesPerThread - 1;

	if (i == threadCount - 1) {
		endIndex = params.particleCount - 1;
	}

	for (cl_uint j = startIndex; j <= endIndex; j++) {
		cl_uint bucket = (inGridIndices[j] & (0xFF << (passNumber * radixWidth))) >> (passNumber * radixWidth);

		cl_uint sortedIndex = scannedBuckets[bucket * threadCount + i];
		++(scannedBuckets[bucket * threadCount + i]);

		outPositions[sortedIndex] = inPositions[j];
		outVelocities[sortedIndex] = inVelocities[j];
		outHalfVelocities[sortedIndex] = inHalfVelocities[j];
		outIsFirstTimeSteps[sortedIndex] = inIsFirstTimeSteps[j];
		outGridIndices[sortedIndex] = inGridIndices[j];
	}
}

__kernel void buildCellList(__global const cl_uint* inGridIndices,
							const ParallelSPHParameters params,
							__global cl_int* cellList)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		cl_uint particleGridIndex = inGridIndices[i];

		if (i == 0)
		{
			for (cl_uint j = 0; j <= particleGridIndex; j++)
			{
				cellList[j] = i;
			}
		}
		else
		{
			cl_uint prevParticleGridIndex = inGridIndices[i - 1];

			if (particleGridIndex != prevParticleGridIndex)
			{
				for (cl_uint j = particleGridIndex; j > prevParticleGridIndex; j--)
				{
					cellList[j] = i;
				}
			}

			if (i == params.particleCount - 1)
			{
				for (cl_uint j = particleGridIndex + 1; j < params.cellCount; j++)
				{
					cellList[j] = i + 1;
				}
			}
		}
	}
}

// ---------------- SPH KERNELS ------------------

__kernel void calcDensityPressure(__global const cl_float4* inPositions,
								  __global cl_float* outDensities,
								  __global cl_float* outPressures,
								  const ParallelSPHParameters parameters,
								  __global const cl_int* cellList,
								  __global const cl_float* globalDefaultKernelWeights,
								  __local cl_float* defaultKernelWeights)
{
	const cl_uint i = get_global_id(0);
	const cl_uint workGroupSize = get_local_size(0);
	const cl_uint localIndex = get_local_id(0);

	ParallelSPHParameters params = parameters;

	for (cl_uint kernelWeightIndex = 0; kernelWeightIndex < params.kernelWeightCount; kernelWeightIndex += workGroupSize)
	{
		if (kernelWeightIndex + localIndex < params.kernelWeightCount)
		{
			defaultKernelWeights[kernelWeightIndex + localIndex] = globalDefaultKernelWeights[kernelWeightIndex + localIndex];
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if (i < params.particleCount)
	{
		cl_float4 currentPosition = inPositions[i];

		cl_float weightedSum = 0.f;

		cl_int xGrid = trunc((currentPosition.x + params.gridOffset.x) / params.gridSpacing);
		cl_int yGrid = trunc((currentPosition.y + params.gridOffset.y) / params.gridSpacing);
		cl_int zGrid = trunc((currentPosition.z + params.gridOffset.z) / params.gridSpacing);
		for (cl_int z = zGrid - 1; z <= zGrid + 1; z++) {
			for (cl_int y = yGrid - 1; y <= yGrid + 1; y++) {
				for (cl_int x = xGrid - 1; x <= xGrid + 1; x++) {
					if ((x >= 0 && x < params.gridSize.x) &&
						(y >= 0 && y < params.gridSize.y) &&
						(z >= 0 && z < params.gridSize.z))
					{
						cl_uint gridIndex = x + params.gridSize.x * y + params.gridSize.x * params.gridSize.y * z;
						cl_uint neighborEnd = (gridIndex + 1 < params.cellCount) ? cellList[gridIndex + 1] : params.particleCount;
						for (cl_uint j = cellList[gridIndex]; j < neighborEnd; j++)
						{
							cl_float4 neighborPosition = inPositions[j];
							cl_float particleDistance = distance(neighborPosition, currentPosition);
							if (isless(particleDistance, params.kernelRadius))
							{
								cl_uint index = trunc(particleDistance / params.kernelDivisionStep);
								weightedSum += defaultKernelWeights[index];
							}
						}
					}
				}
			}
		}
		cl_float density = params.particleMass * weightedSum;

		cl_float pressure = params.pressureStiffnessCoefficient * (density - params.restDensity);
		if (isless(pressure, 0.f))
			pressure *= params.negativePressureFactor;

		outDensities[i] = density;
		outPressures[i] = pressure;
	}
}

__kernel void accumulateNonPressureForces(__global const cl_float4* inPositions,
							   __global const cl_float4* inVelocities,
							   __global const cl_float* inDensities,
							   __global cl_float4* outAccumulatedForces,
							   const ParallelSPHParameters parameters,
							   __global const cl_int* cellList,
							   __global const cl_float* globalDefaultKernelFirstDerivativeWeights,
							   __local cl_float* defaultKernelFirstDerivativeWeights,
							   __global const cl_float* globalDefaultKernelSecondDerivativeWeights,
							   __local cl_float* defaultKernelSecondDerivativeWeights,
							   __global const cl_float* globalViscosityKernelSecondDerivativeWeights,
							   __local cl_float* viscosityKernelSecondDerivativeWeights)
{
	const cl_uint i = get_global_id(0);
	const cl_uint workGroupSize = get_local_size(0);
	const cl_uint localIndex = get_local_id(0);

	ParallelSPHParameters params = parameters;

	for (cl_uint kernelWeightIndex = 0; kernelWeightIndex < params.kernelWeightCount; kernelWeightIndex += workGroupSize)
	{
		if (kernelWeightIndex + localIndex < params.kernelWeightCount)
		{
			defaultKernelFirstDerivativeWeights[kernelWeightIndex + localIndex] = globalDefaultKernelFirstDerivativeWeights[kernelWeightIndex + localIndex];
			defaultKernelSecondDerivativeWeights[kernelWeightIndex + localIndex] = globalDefaultKernelSecondDerivativeWeights[kernelWeightIndex + localIndex];
			viscosityKernelSecondDerivativeWeights[kernelWeightIndex + localIndex] = globalViscosityKernelSecondDerivativeWeights[kernelWeightIndex + localIndex];
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if (i < params.particleCount)
	{
		cl_float4 currentPosition = inPositions[i];
		cl_float4 currentVelocity = inVelocities[i];
		cl_float currentDensity = inDensities[i];

		// Gravity Force
		cl_float4 accumulatedForce = params.gravity * currentDensity;

		cl_float4 surfaceNormal = (cl_float4)(0.f);
		cl_float laplacianColor = 0.f;
		cl_float4 viscosityForce = (cl_float4)(0.f);

		cl_int xGrid = trunc((currentPosition.x + params.gridOffset.x) / params.gridSpacing);
		cl_int yGrid = trunc((currentPosition.y + params.gridOffset.y) / params.gridSpacing);
		cl_int zGrid = trunc((currentPosition.z + params.gridOffset.z) / params.gridSpacing);
		for (cl_int z = zGrid - 1; z <= zGrid + 1; z++) {
			for (cl_int y = yGrid - 1; y <= yGrid + 1; y++) {
				for (cl_int x = xGrid - 1; x <= xGrid + 1; x++) {
					if ((x >= 0 && x < params.gridSize.x) &&
						(y >= 0 && y < params.gridSize.y) &&
						(z >= 0 && z < params.gridSize.z))
					{
						cl_uint gridIndex = x + params.gridSize.x * y + params.gridSize.x * params.gridSize.y * z;
						cl_uint neighborEnd = (gridIndex + 1 < params.cellCount) ? cellList[gridIndex + 1] : params.particleCount;
						for (cl_uint j = cellList[gridIndex]; j < neighborEnd; j++)
						{
							cl_float4 neighborPosition = inPositions[j];
							cl_float4 neighborVelocity = inVelocities[j];
							cl_float neighborDensity = inDensities[j];

							cl_float particleDistance = distance(neighborPosition, currentPosition);
							if (isless(particleDistance, params.kernelRadius))
							{
								// Surface Tension Force
								cl_float4 direction = currentPosition - neighborPosition;
								cl_uint index = trunc(particleDistance / params.kernelDivisionStep);
								surfaceNormal += direction * defaultKernelFirstDerivativeWeights[index] / neighborDensity;
								laplacianColor += defaultKernelSecondDerivativeWeights[index] / neighborDensity;

								if (i != j)
								{
									// Viscosity Force
									viscosityForce += (neighborVelocity - currentVelocity) * viscosityKernelSecondDerivativeWeights[index] / neighborDensity;
								}
							}
						}
					}
				}
			}
		}
	
		surfaceNormal *= params.particleMass;
		cl_float surfaceNormalLength = length(surfaceNormal);
		if (isgreater(surfaceNormalLength, params.surfaceTensionThreshold))
		{
			accumulatedForce += (surfaceNormal / surfaceNormalLength) * (-params.surfaceTensionCoefficient * laplacianColor * params.particleMass);
		}

		accumulatedForce += viscosityForce * params.viscosityCoefficient * params.particleMass;

		outAccumulatedForces[i] = accumulatedForce;
	}
}

__kernel void accumulatePressureForces(__global const cl_float4* inPositions,
							   __global const cl_float* inDensities,
							   __global const cl_float* inPressures,
							   __global cl_float4* inOutAccumulatedForces,
							   const ParallelSPHParameters parameters,
							   __global const cl_int* cellList,
							   __global const cl_float* globalPressureKernelFirstDerivativeWeights,
							   __local cl_float* pressureKernelFirstDerivativeWeights)
{
	const cl_uint i = get_global_id(0);
	const cl_uint workGroupSize = get_local_size(0);
	const cl_uint localIndex = get_local_id(0);

	ParallelSPHParameters params = parameters;

	for (cl_uint kernelWeightIndex = 0; kernelWeightIndex < params.kernelWeightCount; kernelWeightIndex += workGroupSize)
	{
		if (kernelWeightIndex + localIndex < params.kernelWeightCount)
		{
			pressureKernelFirstDerivativeWeights[kernelWeightIndex + localIndex] = globalPressureKernelFirstDerivativeWeights[kernelWeightIndex + localIndex];
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if (i < params.particleCount)
	{
		cl_float4 currentPosition = inPositions[i];
		cl_float currentDensity = inDensities[i];
		cl_float currentPressure = inPressures[i];

		// Gravity Force
		cl_float4 pressureForce = (cl_float4)(0.f);
		cl_float tempFactor = currentPressure / pown(currentDensity, 2);

		cl_int xGrid = trunc((currentPosition.x + params.gridOffset.x) / params.gridSpacing);
		cl_int yGrid = trunc((currentPosition.y + params.gridOffset.y) / params.gridSpacing);
		cl_int zGrid = trunc((currentPosition.z + params.gridOffset.z) / params.gridSpacing);
		for (cl_int z = zGrid - 1; z <= zGrid + 1; z++) {
			for (cl_int y = yGrid - 1; y <= yGrid + 1; y++) {
				for (cl_int x = xGrid - 1; x <= xGrid + 1; x++) {
					if ((x >= 0 && x < params.gridSize.x) &&
						(y >= 0 && y < params.gridSize.y) &&
						(z >= 0 && z < params.gridSize.z))
					{
						cl_uint gridIndex = x + params.gridSize.x * y + params.gridSize.x * params.gridSize.y * z;
						cl_uint neighborEnd = (gridIndex + 1 < params.cellCount) ? cellList[gridIndex + 1] : params.particleCount;
						for (cl_uint j = cellList[gridIndex]; j < neighborEnd; j++)
						{
							cl_float4 neighborPosition = inPositions[j];
							cl_float neighborDensity = inDensities[j];
							cl_float neighborPressure = inPressures[j];

							cl_float particleDistance = distance(neighborPosition, currentPosition);
							if (isless(particleDistance, params.kernelRadius))
							{
								cl_float4 direction = (currentPosition - neighborPosition) / particleDistance;
								cl_uint index = trunc(particleDistance / params.kernelDivisionStep);

								if (i != j)
								{
									// Pressure Force
									pressureForce += direction * (tempFactor + neighborPressure / pown(neighborDensity, 2)) * pressureKernelFirstDerivativeWeights[index];
								}
							}
						}
					}
				}
			}
		}
		inOutAccumulatedForces[i] += (-pressureForce) * params.particleMass * currentDensity;
	}
}

__kernel void integrate(__global cl_float4* inOutPositions,
						__global cl_float4* inOutHalfVelocities,
						__global cl_bool* inOutIsFirstTimeSteps,
						__global const cl_float4* inVelocities,
						__global const cl_float4* inAccumulatedForces,
						__global const cl_float* inDensities,
						__global cl_float4* outOldHalfVelocities,
						const ParallelSPHParameters params,
						const cl_float deltaTime)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		cl_float4 halfVelocity = inOutHalfVelocities[i];
		cl_float4 position = inOutPositions[i];

		cl_float4 acceleration = inAccumulatedForces[i] / inDensities[i];
		if (inOutIsFirstTimeSteps[i])
		{
			halfVelocity = inVelocities[i] - acceleration * deltaTime / 2.f;
			inOutIsFirstTimeSteps[i] = false;
		}

		outOldHalfVelocities[i] = halfVelocity;
		halfVelocity += acceleration * deltaTime;
		position += halfVelocity * deltaTime;

		inOutHalfVelocities[i] = halfVelocity;
		inOutPositions[i] = position;
	}
}

__kernel void handleCollisions(__global cl_float4* inOutPositions,
							   __global cl_float4* inOutHalfVelocities,
							   __global const cl_float4* inOldHalfVelocities,
							   __global cl_float4* outVelocities,
							   const ParallelSPHParameters params,
							   __global const ParallelSPHCollisionBox* collisionBoxes,
							   __global const ParallelSPHCollisionSphere* collisionSpheres)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		cl_float4 halfVelocity = inOutHalfVelocities[i];
		cl_float4 position = inOutPositions[i];

		for (int j = 0; j < params.collisionBoxCount; j++)
		{
			handleCollisionWithBox(&position, &halfVelocity, params, collisionBoxes[j]);
		}

		for (int j = 0; j < params.collisionSphereCount; j++)
		{
			handleCollisionWithSphere(&position, &halfVelocity, params, collisionSpheres[j]);
		}

		inOutHalfVelocities[i] = halfVelocity;
		inOutPositions[i] = position;
		outVelocities[i] = (inOldHalfVelocities[i] + halfVelocity) / 2.f;
	}
}

void handleCollisionWithBox(cl_float4* position,
							cl_float4* velocity,
							const ParallelSPHParameters params,
							const ParallelSPHCollisionBox collisionBox)
{
	// Collision Detection
	cl_bool isColliding = false;
	cl_float4 collisionNormal = (cl_float4)(0.f);
	cl_float4 collisionPoint = (cl_float4)(0.f);
	cl_float4 penetration = (cl_float4)(0.f);
	cl_float4 distanceVec = *position - collisionBox.position;
	distanceVec.x = fabs(distanceVec.x);
	distanceVec.y = fabs(distanceVec.y);
	distanceVec.z = fabs(distanceVec.z);
	distanceVec.w = 0.f;

	if (!collisionBox.isBoundary)
	{
		penetration = (collisionBox.halfDimensions - distanceVec);
		penetration.x += params.particleRadius;
		penetration.y += params.particleRadius;
		penetration.z += params.particleRadius;
		if (isgreater(penetration.x, 0.f) && isgreater(penetration.y, 0.f) && isgreater(penetration.z, 0.f))
		{
			isColliding = true;
			collisionPoint = *position;

			// Check if penetration on X-Axis is the smallest
			if (islessequal(penetration.x, penetration.y) && islessequal(penetration.x, penetration.z))
			{
				if (isgreaterequal((*position).x - collisionBox.position.x, 0.f))
					collisionNormal.x = 1.f;
				else
					collisionNormal.x = -1.f;

				collisionPoint.x = (collisionBox.position.x + collisionNormal.x * (collisionBox.halfDimensions.x + params.particleRadius));
			}
			// Check if penetration on Y-Axis is the smallest
			else if (islessequal(penetration.y, penetration.x) && islessequal(penetration.y, penetration.z))
			{
				if (isgreaterequal((*position).y - collisionBox.position.y, 0.f))
					collisionNormal.y = 1.f;
				else
					collisionNormal.y = -1.f;

				collisionPoint.y = (collisionBox.position.y + collisionNormal.y * (collisionBox.halfDimensions.y + params.particleRadius));
			}
			// Check if penetration on Z-Axis is the smallest
			else if (islessequal(penetration.z, penetration.y) && islessequal(penetration.z, penetration.x))
			{
				if (isgreaterequal((*position).z - collisionBox.position.z, 0.f))
					collisionNormal.z = 1.f;
				else
					collisionNormal.z = -1.f;

				collisionPoint.z = (collisionBox.position.z + collisionNormal.z * (collisionBox.halfDimensions.z + params.particleRadius));
			}
		}
	}
	else
	{
		penetration = (distanceVec - collisionBox.halfDimensions);
		penetration.x += params.particleRadius;
		penetration.y += params.particleRadius;
		penetration.z += params.particleRadius;
		if (isgreater(penetration.x, 0.f) || isgreater(penetration.y, 0.f) || isgreater(penetration.z, 0.f))
		{
			isColliding = true;
			collisionPoint = *position;

			// Check if there is penetration on X-Axis
			if (isgreater(penetration.x, 0.f))
			{
				if (islessequal((*position).x - collisionBox.position.x, 0.f))
					collisionNormal.x = 1.f;
				else
					collisionNormal.x = -1.f;

				collisionPoint.x = (collisionBox.position.x - collisionNormal.x * (collisionBox.halfDimensions.x - params.particleRadius));
			}
			// Check if there is penetration on Y-Axis
			if (isgreater(penetration.y, 0.f))
			{
				if (islessequal((*position).y - collisionBox.position.y, 0.f))
					collisionNormal.y = 1.f;
				else
					collisionNormal.y = -1.f;

				collisionPoint.y = (collisionBox.position.y - collisionNormal.y * (collisionBox.halfDimensions.y - params.particleRadius));
			}
			// Check if there is penetration on Z-Axis
			if (isgreater(penetration.z, 0.f))
			{
				if (islessequal((*position).z - collisionBox.position.z, 0.f))
					collisionNormal.z = 1.f;
				else
					collisionNormal.z = -1.f;

				collisionPoint.z = (collisionBox.position.z - collisionNormal.z * (collisionBox.halfDimensions.z - params.particleRadius));
			}

			collisionNormal = normalize(collisionNormal);
		}
	}

	if (isColliding)
	{
		// Resolve Collision
		resolveCollision(position, velocity, params, collisionNormal, collisionPoint);
	}
}

void handleCollisionWithSphere(cl_float4* position,
							   cl_float4* velocity,
							   const ParallelSPHParameters params,
							   const ParallelSPHCollisionSphere collisionSphere)
{
	// Collision Detection
	cl_bool isColliding = false;
	cl_float4 collisionNormal = (cl_float4)(0.f);
	cl_float4 collisionPoint = (cl_float4)(0.f);
	cl_float penetration;
	cl_float distanceValue = distance(collisionSphere.position, *position);

	if (!collisionSphere.isBoundary)
	{
		penetration = (collisionSphere.radius + params.particleRadius) - distanceValue;
		if (isgreater(penetration, 0.f))
		{
			isColliding = true;
			collisionNormal = (*position - collisionSphere.position) / distanceValue;
			collisionPoint = collisionSphere.position + collisionNormal * (collisionSphere.radius + params.particleRadius);
		}
	}
	else
	{
		penetration = distanceValue - (collisionSphere.radius - params.particleRadius);
		if (isgreater(penetration, 0.f))
		{
			isColliding = true;
			collisionNormal = (collisionSphere.position - *position) / distanceValue;
			collisionPoint = collisionSphere.position - collisionNormal * (collisionSphere.radius - params.particleRadius);
		}
	}

	if (isColliding)
	{
		// Resolve Collision
		resolveCollision(position, velocity, params, collisionNormal, collisionPoint);
	}
}

void resolveCollision(cl_float4* position,
					  cl_float4* velocity,
					  const ParallelSPHParameters params, 
					  const cl_float4 collisionNormal,
					  const cl_float4 collisionPoint)
{
	cl_float separatingVelocity = dot(*velocity, collisionNormal);
	// Check if velocity is facing opposite direction of the contactNormal
	if (isless(separatingVelocity, 0.f))
	{
		cl_float4 separatingVelocityN = collisionNormal * separatingVelocity;
		cl_float4 separatingVelocityT = *velocity - separatingVelocityN;
		// resolve velocity
		separatingVelocityN *= -params.restitutionCoefficient;
		separatingVelocityT *= params.frictionCoefficient;
		*velocity = separatingVelocityN + separatingVelocityT;
	}
	// resolve position
	*position = collisionPoint;
}

__kernel void pciInit(__global cl_float* outPressures,
	__global cl_float4* outPredictedPressureForces,
	const ParallelSPHParameters params)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		outPressures[i] = 0.f;
		outPredictedPressureForces[i] = (cl_float4)(0.f);
	}
}

// ---------- PCISPH KERNELS -----------

__kernel void pciIntegrate(__global const cl_float4* inPositions,
	__global const cl_float4* inVelocities,
	__global const cl_float4* inHalfVelocities,
	__global const cl_float4* inAccumulatedForces,
	__global const cl_float4* inPredictedPressureForces,
	__global const cl_float* inDensities,
	__global const cl_bool* inIsFirstTimeSteps,
	__global cl_float4* outPredictedHalfVelocities,
	__global cl_float4* outPredictedPositions,
	const ParallelSPHParameters params,
	const cl_float deltaTime)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		cl_float4 halfVelocity = inHalfVelocities[i];
		cl_float4 position = inPositions[i];

		cl_float4 acceleration = (inAccumulatedForces[i] + inPredictedPressureForces[i]) / inDensities[i];
		if (inIsFirstTimeSteps[i])
		{
			halfVelocity = inVelocities[i] - acceleration * deltaTime / 2.f;
		}

		halfVelocity += acceleration * deltaTime;
		position += halfVelocity * deltaTime;

		outPredictedHalfVelocities[i] = halfVelocity;
		outPredictedPositions[i] = position;
	}
}

__kernel void pciHandleCollisions(__global cl_float4* inOutPredictedPositions,
	__global cl_float4* inOutPredictedHalfVelocities,
	const ParallelSPHParameters params,
	__global const ParallelSPHCollisionBox* collisionBoxes,
	__global const ParallelSPHCollisionSphere* collisionSpheres)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		cl_float4 halfVelocity = inOutPredictedHalfVelocities[i];
		cl_float4 position = inOutPredictedPositions[i];

		for (int j = 0; j < params.collisionBoxCount; j++)
		{
			handleCollisionWithBox(&position, &halfVelocity, params, collisionBoxes[j]);
		}

		for (int j = 0; j < params.collisionSphereCount; j++)
		{
			handleCollisionWithSphere(&position, &halfVelocity, params, collisionSpheres[j]);
		}

		inOutPredictedHalfVelocities[i] = halfVelocity;
		inOutPredictedPositions[i] = position;
	}
}

__kernel void pciCalcDensityPressure(__global const cl_float4* inPositions,
	__global const cl_float4* inPredictedPositions,
	__global cl_float* outPredictedDensities,
	__global cl_float* inOutPressures,
	const ParallelSPHParameters parameters,
	__global const cl_int* cellList,
	__global const cl_float* globalDefaultKernelWeights,
	__local cl_float* defaultKernelWeights,
	const cl_float delta)
{
	const cl_uint i = get_global_id(0);
	const cl_uint workGroupSize = get_local_size(0);
	const cl_uint localIndex = get_local_id(0);

	ParallelSPHParameters params = parameters;

	for (cl_uint kernelWeightIndex = 0; kernelWeightIndex < params.kernelWeightCount; kernelWeightIndex += workGroupSize)
	{
		if (kernelWeightIndex + localIndex < params.kernelWeightCount)
		{
			defaultKernelWeights[kernelWeightIndex + localIndex] = globalDefaultKernelWeights[kernelWeightIndex + localIndex];
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if (i < params.particleCount)
	{
		cl_float4 currentPosition = inPositions[i];
		cl_float4 currentPredictedPosition = inPredictedPositions[i];

		cl_float weightedSum = 0.f;

		cl_int xGrid = trunc((currentPosition.x + params.gridOffset.x) / params.gridSpacing);
		cl_int yGrid = trunc((currentPosition.y + params.gridOffset.y) / params.gridSpacing);
		cl_int zGrid = trunc((currentPosition.z + params.gridOffset.z) / params.gridSpacing);
		for (cl_int z = zGrid - 1; z <= zGrid + 1; z++) {
			for (cl_int y = yGrid - 1; y <= yGrid + 1; y++) {
				for (cl_int x = xGrid - 1; x <= xGrid + 1; x++) {
					if ((x >= 0 && x < params.gridSize.x) &&
						(y >= 0 && y < params.gridSize.y) &&
						(z >= 0 && z < params.gridSize.z))
					{
						cl_uint gridIndex = x + params.gridSize.x * y + params.gridSize.x * params.gridSize.y * z;
						cl_uint neighborEnd = (gridIndex + 1 < params.cellCount) ? cellList[gridIndex + 1] : params.particleCount;
						for (cl_uint j = cellList[gridIndex]; j < neighborEnd; j++)
						{
							cl_float4 neighborPredictedPosition = inPredictedPositions[j];
							cl_float particleDistance = distance(neighborPredictedPosition, currentPredictedPosition);
							if (isless(particleDistance, params.kernelRadius))
							{
								cl_uint index = trunc(particleDistance / params.kernelDivisionStep);
								weightedSum += defaultKernelWeights[index];
							}
						}
					}
				}
			}
		}

		cl_float predictedDensity = params.particleMass * weightedSum;
		cl_float predictedPressure = delta * (predictedDensity - params.restDensity);
		if (isless(predictedPressure, 0.f))
			predictedPressure *= params.negativePressureFactor;

		outPredictedDensities[i] = predictedDensity;
		inOutPressures[i] += predictedPressure;
	}
}

__kernel void pciCalcPressureForce(__global const cl_float4* inPositions,
	__global const cl_float4* inPredictedPositions,
	__global const cl_float* inPredictedDensities,
	__global const cl_float* inPressures,
	__global cl_float4* outPredictedPressureForces,
	const ParallelSPHParameters parameters,
	__global const cl_int* cellList,
	__global const cl_float* globalPressureKernelFirstDerivativeWeights,
	__local cl_float* pressureKernelFirstDerivativeWeights)
{
	const cl_uint i = get_global_id(0);
	const cl_uint workGroupSize = get_local_size(0);
	const cl_uint localIndex = get_local_id(0);

	ParallelSPHParameters params = parameters;

	for (cl_uint kernelWeightIndex = 0; kernelWeightIndex < params.kernelWeightCount; kernelWeightIndex += workGroupSize)
	{
		if (kernelWeightIndex + localIndex < params.kernelWeightCount)
		{
			pressureKernelFirstDerivativeWeights[kernelWeightIndex + localIndex] = globalPressureKernelFirstDerivativeWeights[kernelWeightIndex + localIndex];
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	if (i < params.particleCount)
	{
		cl_float4 currentPosition = inPositions[i];
		cl_float4 currentPredictedPosition = inPredictedPositions[i];
		cl_float currentPredictedDensity = inPredictedDensities[i];
		cl_float currentPressure = inPressures[i];

		cl_float4 pressureForce = (cl_float4)(0.f);
		cl_float tempFactor = currentPressure / pown(currentPredictedDensity, 2);

		cl_int xGrid = trunc((currentPosition.x + params.gridOffset.x) / params.gridSpacing);
		cl_int yGrid = trunc((currentPosition.y + params.gridOffset.y) / params.gridSpacing);
		cl_int zGrid = trunc((currentPosition.z + params.gridOffset.z) / params.gridSpacing);
		for (cl_int z = zGrid - 1; z <= zGrid + 1; z++) {
			for (cl_int y = yGrid - 1; y <= yGrid + 1; y++) {
				for (cl_int x = xGrid - 1; x <= xGrid + 1; x++) {
					if ((x >= 0 && x < params.gridSize.x) &&
						(y >= 0 && y < params.gridSize.y) &&
						(z >= 0 && z < params.gridSize.z))
					{
						cl_uint gridIndex = x + params.gridSize.x * y + params.gridSize.x * params.gridSize.y * z;
						cl_uint neighborEnd = (gridIndex + 1 < params.cellCount) ? cellList[gridIndex + 1] : params.particleCount;
						for (cl_uint j = cellList[gridIndex]; j < neighborEnd; j++)
						{
							cl_float4 neighborPredictedPosition = inPredictedPositions[j];
							cl_float neighborPredictedDensity = inPredictedDensities[j];
							cl_float neighborPressure = inPressures[j];

							cl_float particleDistance = distance(neighborPredictedPosition, currentPredictedPosition);
							if (isless(particleDistance, params.kernelRadius))
							{
								if (i != j)
								{
									// Pressure Force
									cl_float4 direction = (currentPredictedPosition - neighborPredictedPosition) / particleDistance;
									cl_uint index = trunc(particleDistance / params.kernelDivisionStep);
									pressureForce += direction * (tempFactor + neighborPressure / pown(neighborPredictedDensity, 2)) * pressureKernelFirstDerivativeWeights[index];
								}
							}
						}
					}
				}
			}
		}

		outPredictedPressureForces[i] = (-pressureForce) * params.particleMass * currentPredictedDensity;
	}
}

__kernel void pciAddPressureForce(__global const cl_float4* inPredictedPressureForces,
	__global cl_float4* inOutAccumulatedForces,
	const ParallelSPHParameters params)
{
	const cl_uint i = get_global_id(0);

	if (i < params.particleCount)
	{
		inOutAccumulatedForces[i] += inPredictedPressureForces[i];
	}
}
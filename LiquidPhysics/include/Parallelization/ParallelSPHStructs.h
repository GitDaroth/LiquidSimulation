#pragma once

struct float4 {
	float x;
	float y;
	float z;
	float w;
};

struct uint4 {
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;
};

typedef struct {
	float4 gravity;						// 16 Byte
	float4 gridOffset;					// 32 Byte
	uint4 gridSize;						// 48 Byte
	float particleMass;					// 52 Byte
	float particleRadius;				// 56 Byte
	float kernelRadius;					// 60 Byte
	float kernelDivisionStep;			// 64 Byte
	float gridSpacing;					// 68 Byte
	float restDensity;					// 72 Byte
	float pressureStiffnessCoefficient;	// 76 Byte
	float negativePressureFactor;		// 80 Byte
	float viscosityCoefficient;			// 84 Byte
	float surfaceTensionCoefficient;	// 88 Byte
	float surfaceTensionThreshold;		// 92 Byte
	float restitutionCoefficient;		// 96 Byte
	float frictionCoefficient;			// 100 Byte
	float particleCount;				// 104 Byte
	unsigned int cellCount;				// 108 Byte
	unsigned int collisionBoxCount;		// 112 Byte
	unsigned int collisionSphereCount;	// 116 Byte
	unsigned int kernelWeightCount;		// 120 Byte
	float dummy1, dummy2;				// 128 Byte
} ParallelSPHParameters;

typedef struct {
	float4 position;				// 16 Byte
	float4 halfDimensions;			// 32 Byte
	unsigned int isBoundary;		// 36 Byte
	float dummy1, dummy2, dummy3;	// 48 Byte
	float4 dummy4;					// 64 Byte
} ParallelSPHCollisionBox;

typedef struct {
	float4 position;				// 16 Byte
	float radius;					// 20 Byte
	unsigned int isBoundary;		// 24 Byte
	float dummy1, dummy2;			// 32 Btye
} ParallelSPHCollisionSphere;
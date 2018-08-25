#include "SPHParticleEmitter.h"

namespace LiPhEn {
	SPHParticleEmitter::SPHParticleEmitter()
	{
	}


	SPHParticleEmitter::~SPHParticleEmitter()
	{
	}

	std::vector<Vector3D> SPHParticleEmitter::spawnCube(Vector3D position, Vector3D halfSizes, float particleRadius)
	{
		std::vector<Vector3D> spawnedParticles;

		float stepSize = 1.6f * particleRadius;
		float sizeX = (2.f * halfSizes.getX() - 2.f * particleRadius) / stepSize;
		float sizeY = (2.f * halfSizes.getY() - 2.f * particleRadius) / stepSize;
		float sizeZ = (2.f * halfSizes.getZ() - 2.f * particleRadius) / stepSize;
		int resolutionX = int(sizeX) + 1;
		int resolutionY = int(sizeY) + 1;
		int resolutionZ = int(sizeZ) + 1;
		float remainderX = sizeX - int(sizeX);
		float remainderY = sizeY - int(sizeY);
		float remainderZ = sizeZ - int(sizeZ);

		for (int x = 0; x < resolutionX; x++)
		{
			for (int y = 0; y < resolutionY; y++)
			{
				for (int z = 0; z < resolutionZ; z++)
				{
					Vector3D particlePosition = position - halfSizes +
												Vector3D((1.f + remainderX / 2.f) * particleRadius, (1.f + remainderY / 2.f) * particleRadius, (1.f + remainderZ / 2.f) * particleRadius) +
												Vector3D(x * stepSize, y * stepSize, z * stepSize);
					spawnedParticles.push_back(particlePosition);
				}
			}
		}

		return spawnedParticles;
	}

	std::vector<Vector3D> SPHParticleEmitter::spawnSphere(Vector3D position, float sphereRadius, float particleRadius)
	{
		std::vector<Vector3D> spawnedParticles;

		float stepSize = 1.6f * particleRadius;
		float size = (2.f * sphereRadius - 2.f * particleRadius) / stepSize;
		int resolution = int(size) + 1;
		float remainder = size - int(size);

		for (int x = 0; x < resolution; x++)
		{
			for (int y = 0; y < resolution; y++)
			{
				for (int z = 0; z < resolution; z++)
				{
					float distance = (Vector3D((1.f + remainder / 2.f) * particleRadius) +
									Vector3D(x * stepSize, y * stepSize, z * stepSize) - Vector3D(sphereRadius)).magnitude() + particleRadius;
					if (distance <= sphereRadius)
					{
						Vector3D particlePosition = position - Vector3D(sphereRadius) +
													Vector3D((1.f + remainder / 2.f) * particleRadius) +
													Vector3D(x * stepSize, y * stepSize, z * stepSize);
						spawnedParticles.push_back(particlePosition);
					}
				}
			}
		}

		return spawnedParticles;
	}

	std::vector<Vector3D> SPHParticleEmitter::spawnCircleX(Vector3D position, float circleRadius, float particleRadius)
	{
		std::vector<Vector3D> spawnedParticles;

		float stepSize = 1.6f * particleRadius;
		float size = (2.f * circleRadius - 2.f * particleRadius) / stepSize;
		int resolution = int(size) + 1;
		float remainder = size - int(size);

		for (int y = 0; y < resolution; y++)
		{
			for (int z = 0; z < resolution; z++)
			{
				float distance = (Vector3D(0.f, (1.f + remainder / 2.f) * particleRadius, (1.f + remainder / 2.f) * particleRadius) +
								Vector3D(0.f, y * stepSize, z * stepSize) - Vector3D(0.f, circleRadius, circleRadius)).magnitude() + particleRadius;
				if (distance <= circleRadius)
				{
					Vector3D particlePosition = position - Vector3D(0.f, circleRadius, circleRadius) +
												Vector3D(0.f, (1.f + remainder / 2.f) * particleRadius, (1.f + remainder / 2.f) * particleRadius) +
												Vector3D(0.f, y * stepSize, z * stepSize);
					spawnedParticles.push_back(particlePosition);
				}
			}
		}

		return spawnedParticles;
	}

	std::vector<Vector3D> SPHParticleEmitter::spawnCircleY(Vector3D position, float circleRadius, float particleRadius)
	{
		std::vector<Vector3D> spawnedParticles;

		float stepSize = 1.6f * particleRadius;
		float size = (2.f * circleRadius - 2.f * particleRadius) / stepSize;
		int resolution = int(size) + 1;
		float remainder = size - int(size);

		for (int x = 0; x < resolution; x++)
		{
			for (int z = 0; z < resolution; z++)
			{
				float distance = (Vector3D((1.f + remainder / 2.f) * particleRadius, 0.f, (1.f + remainder / 2.f) * particleRadius) +
								Vector3D(x * stepSize, 0.f, z * stepSize) - Vector3D(circleRadius, 0.f, circleRadius)).magnitude() + particleRadius;
				if (distance <= circleRadius)
				{
					Vector3D particlePosition = position - Vector3D(circleRadius, 0.f, circleRadius) +
												Vector3D((1.f + remainder / 2.f) * particleRadius, 0.f, (1.f + remainder / 2.f) * particleRadius) +
												Vector3D(x * stepSize, 0.f, z * stepSize);
					spawnedParticles.push_back(particlePosition);
				}
			}
		}

		return spawnedParticles;
	}

	std::vector<Vector3D> SPHParticleEmitter::spawnCircleZ(Vector3D position, float circleRadius, float particleRadius)
	{
		std::vector<Vector3D> spawnedParticles;

		float stepSize = 1.6f * particleRadius;
		float size = (2.f * circleRadius - 2.f * particleRadius) / stepSize;
		int resolution = int(size) + 1;
		float remainder = size - int(size);

		for (int x = 0; x < resolution; x++)
		{
			for (int y = 0; y < resolution; y++)
			{
				float distance = (Vector3D((1.f + remainder / 2.f) * particleRadius, (1.f + remainder / 2.f) * particleRadius, 0.f) +
								Vector3D(x * stepSize, y * stepSize, 0.f) - Vector3D(circleRadius, circleRadius, 0.f)).magnitude() + particleRadius;
				if (distance <= circleRadius)
				{
					Vector3D particlePosition = position - Vector3D(circleRadius, circleRadius, 0.f) +
												Vector3D((1.f + remainder / 2.f, (1.f + remainder / 2.f) * particleRadius, 0.f) * particleRadius) +
												Vector3D(x * stepSize, y * stepSize, 0.f);
					spawnedParticles.push_back(particlePosition);
				}
			}
		}

		return spawnedParticles;
	}
}
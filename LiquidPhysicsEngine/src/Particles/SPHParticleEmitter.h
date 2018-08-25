#pragma once

#include "../Math/Vector3D.h"
#include <vector>

namespace LiPhEn {
	class SPHParticleEmitter
	{
	public:
		SPHParticleEmitter();
		~SPHParticleEmitter();

		static std::vector<Vector3D> spawnCube(Vector3D position, Vector3D halfSizes, float particleRadius);
		static std::vector<Vector3D> spawnSphere(Vector3D position, float sphereRadius, float particleRadius);
		static std::vector<Vector3D> spawnCircleX(Vector3D position, float circleRadius, float particleRadius);
		static std::vector<Vector3D> spawnCircleY(Vector3D position, float circleRadius, float particleRadius);
		static std::vector<Vector3D> spawnCircleZ(Vector3D position, float circleRadius, float particleRadius);
	};
}
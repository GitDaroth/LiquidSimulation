#pragma once

#include "../Math/Vector3D.h"

namespace LiPhEn {
	enum class StaticCollisionObjectType {
		OBSTACLE,
		BOUNDARY
	};

	struct StaticCollisionInfo {
		Vector3D collisionNormal;
		Vector3D collisionPoint;
		bool foundCollision;
	};

	struct ParticleCollisionData {
		Vector3D position;
		Vector3D velocity;
		float radius;
		float restitutionCoefficient;
		float frictionCoefficient;
	};

	class StaticCollisionObject
	{
	protected:
		Vector3D m_position;
		StaticCollisionObjectType m_type;

	public:
		explicit StaticCollisionObject(Vector3D position, StaticCollisionObjectType type = StaticCollisionObjectType::OBSTACLE);

		ParticleCollisionData handleCollisionWithParticle(ParticleCollisionData particleData) const;

		Vector3D getPosition() const;
		StaticCollisionObjectType getType() const;

		void setPosition(const Vector3D& position);
		void setType(StaticCollisionObjectType type);

	protected:
		virtual StaticCollisionInfo detectCollisionWithParticle(ParticleCollisionData particleData) const = 0;

	private:
		ParticleCollisionData resolveCollisionWithParticle(ParticleCollisionData particleData, StaticCollisionInfo collisionInfo) const;
	};
}

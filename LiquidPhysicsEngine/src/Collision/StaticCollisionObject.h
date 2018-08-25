#pragma once

#include "../Math/Vector3D.h"
#include <math.h>

namespace LiPhEn {
	enum class StaticCollisionObjectType {
		OBSTACLE,
		BOUNDARY
	};

	struct StaticCollisionInfo {
		Vector3D collisionNormal;
		Vector3D collisionPoint;
	};

	class StaticCollisionObject
	{
	public:
		StaticCollisionObject(Vector3D position, StaticCollisionObjectType type = StaticCollisionObjectType::OBSTACLE);
		~StaticCollisionObject();

        void handleCollisionWithParticle(Vector3D* particlePosition, Vector3D* particleVelocity, float particleRadius, float restitutionCoefficient, float frictionCoefficient) const;

		Vector3D getPosition() const;
		void setPosition(const Vector3D& position);

        StaticCollisionObjectType getType() const;

	protected:
		virtual StaticCollisionInfo* detectCollisionWithParticle(const Vector3D& particlePosition, float particleRadius) const = 0;
        void resolveCollisionWithParticle(Vector3D* particlePosition, Vector3D* particleVelocity, float restitutionCoefficient, float frictionCoefficient, StaticCollisionInfo* collisionInfo) const;

		Vector3D m_position;
		StaticCollisionObjectType m_type;
	};
}

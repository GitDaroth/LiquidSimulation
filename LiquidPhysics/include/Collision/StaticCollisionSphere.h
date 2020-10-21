#pragma once

#include "Collision/StaticCollisionObject.h"

namespace LiPhEn {
	class StaticCollisionSphere : public StaticCollisionObject
	{
	protected:
		float m_radius;

	public:
		StaticCollisionSphere(Vector3D position, float radius, StaticCollisionObjectType type = StaticCollisionObjectType::OBSTACLE);

        float getRadius() const;
        void setRadius(float radius);

	private:
		virtual StaticCollisionInfo detectCollisionWithParticle(ParticleCollisionData particleData) const;
		float calcPenetration(ParticleCollisionData particleData, float distance) const;
		StaticCollisionInfo calcCollisionInfo(ParticleCollisionData particleData, float penetration) const;
	};
}

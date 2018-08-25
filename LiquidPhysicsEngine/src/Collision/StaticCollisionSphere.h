#pragma once

#include "StaticCollisionObject.h"

namespace LiPhEn {
	class StaticCollisionSphere : public StaticCollisionObject
	{
	public:
		StaticCollisionSphere(Vector3D position, float radius, StaticCollisionObjectType type = StaticCollisionObjectType::OBSTACLE);
		~StaticCollisionSphere();

        float getRadius() const;
        void setRadius(float radius);

	protected:
		virtual StaticCollisionInfo* detectCollisionWithParticle(const Vector3D& particlePosition, float particleRadius) const;

		float m_radius;
	};
}

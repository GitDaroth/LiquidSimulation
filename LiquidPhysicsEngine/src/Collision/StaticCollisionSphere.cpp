#include "StaticCollisionSphere.h"

namespace LiPhEn {
	StaticCollisionSphere::StaticCollisionSphere(Vector3D position, float radius, StaticCollisionObjectType type) :
		m_radius(radius),
		StaticCollisionObject(position, type)
	{
	}

	StaticCollisionSphere::~StaticCollisionSphere()
    {
    }

    float StaticCollisionSphere::getRadius() const
    {
        return m_radius;
    }

    void StaticCollisionSphere::setRadius(float radius)
    {
        m_radius = radius;
    }

	StaticCollisionInfo* StaticCollisionSphere::detectCollisionWithParticle(const Vector3D& particlePosition, float particleRadius) const
	{
		StaticCollisionInfo* collisionInfo = NULL;
		
		float penetration;
		float distance = (m_position - particlePosition).magnitude();;

		if (m_type == StaticCollisionObjectType::OBSTACLE)
		{
			penetration = (m_radius + particleRadius) - distance;
			if (penetration > 0.f)
			{
				collisionInfo = new StaticCollisionInfo();
				collisionInfo->collisionNormal = (particlePosition - m_position) / distance;
				collisionInfo->collisionPoint = m_position + collisionInfo->collisionNormal * (m_radius + particleRadius);
			}
		}
		else if (m_type == StaticCollisionObjectType::BOUNDARY)
		{
			penetration = distance - (m_radius - particleRadius);
			if (penetration > 0.f)
			{
				collisionInfo = new StaticCollisionInfo();
				collisionInfo->collisionNormal = (m_position - particlePosition) / distance;
				collisionInfo->collisionPoint = m_position - collisionInfo->collisionNormal * (m_radius - particleRadius);
			}
		}
		return collisionInfo;
	}
}

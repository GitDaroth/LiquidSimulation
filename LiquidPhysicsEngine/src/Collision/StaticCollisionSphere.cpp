#include "StaticCollisionSphere.h"

namespace LiPhEn {
	StaticCollisionSphere::StaticCollisionSphere(Vector3D position, float radius, StaticCollisionObjectType type) :
		m_radius(radius),
		StaticCollisionObject(position, type)
	{
	}

	StaticCollisionInfo StaticCollisionSphere::detectCollisionWithParticle(ParticleCollisionData particleData) const
	{
		float distance = (m_position - particleData.position).magnitude();

		float penetration = calcPenetration(particleData, distance);
		StaticCollisionInfo collisionInfo = calcCollisionInfo(particleData, penetration);

		return collisionInfo;
	}

	float StaticCollisionSphere::calcPenetration(ParticleCollisionData particleData, float distance) const
	{
		float penetration = 0.f;

		if (m_type == StaticCollisionObjectType::OBSTACLE)
			penetration = (m_radius + particleData.radius) - distance;
		else if (m_type == StaticCollisionObjectType::BOUNDARY)
			penetration = distance - (m_radius - particleData.radius);

		return penetration;
	}

	StaticCollisionInfo StaticCollisionSphere::calcCollisionInfo(ParticleCollisionData particleData, float penetration) const
	{
		StaticCollisionInfo collisionInfo;
		collisionInfo.collisionNormal = Vector3D(0.f, 0.f, 0.f);
		collisionInfo.collisionPoint = Vector3D(0.f, 0.f, 0.f);
		collisionInfo.foundCollision = false;

		if (penetration > 0.f)
		{
			collisionInfo.foundCollision = true;
			if (m_type == StaticCollisionObjectType::OBSTACLE)
			{
				collisionInfo.collisionNormal = (particleData.position - m_position);
				collisionInfo.collisionPoint = m_position + collisionInfo.collisionNormal * (m_radius + particleData.radius);
			}
			else if (m_type == StaticCollisionObjectType::BOUNDARY)
			{
				collisionInfo.collisionNormal = (m_position - particleData.position);
				collisionInfo.collisionPoint = m_position - collisionInfo.collisionNormal * (m_radius - particleData.radius);
			}
			collisionInfo.collisionNormal.normalize();
		}

		return collisionInfo;
	}

	float StaticCollisionSphere::getRadius() const
	{
		return m_radius;
	}

	void StaticCollisionSphere::setRadius(float radius)
	{
		m_radius = radius;
	}
}

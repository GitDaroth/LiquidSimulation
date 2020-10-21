#include "Collision/StaticCollisionObject.h"

namespace LiPhEn {
	StaticCollisionObject::StaticCollisionObject(Vector3D position, StaticCollisionObjectType type) :
		m_position(position),
		m_type(type)
	{
	}

	ParticleCollisionData StaticCollisionObject::handleCollisionWithParticle(ParticleCollisionData particleData) const
	{
		StaticCollisionInfo collisionInfo = detectCollisionWithParticle(particleData);
		if (collisionInfo.foundCollision)
		{
			particleData = resolveCollisionWithParticle(particleData, collisionInfo);
		}

		return particleData;
	}

	ParticleCollisionData StaticCollisionObject::resolveCollisionWithParticle(ParticleCollisionData particleData, StaticCollisionInfo collisionInfo) const
	{
		float separatingVelocity = particleData.velocity * collisionInfo.collisionNormal;
		// Check if velocity is facing opposite direction of the contactNormal
		if (separatingVelocity < 0.f)
		{
			Vector3D separatingVelocityN = collisionInfo.collisionNormal * separatingVelocity;
			Vector3D separatingVelocityT = particleData.velocity - separatingVelocityN;
			// resolve velocity
			separatingVelocityN *= -particleData.restitutionCoefficient;
            separatingVelocityT *= particleData.frictionCoefficient;
			particleData.velocity = separatingVelocityN + separatingVelocityT;
		}
		// resolve m_position
		particleData.position = collisionInfo.collisionPoint;

		return particleData;
	}

	/// GETTERS
	Vector3D StaticCollisionObject::getPosition() const
	{
		return m_position;
	}

	StaticCollisionObjectType StaticCollisionObject::getType() const
	{
		return m_type;
	}

	/// SETTERS
	void StaticCollisionObject::setPosition(const Vector3D& position)
	{
        m_position = position;
    }

	void StaticCollisionObject::setType(StaticCollisionObjectType type)
	{
		m_type = type;
	}
}

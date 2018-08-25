#include "StaticCollisionObject.h"

namespace LiPhEn {
	StaticCollisionObject::StaticCollisionObject(Vector3D position, StaticCollisionObjectType type) :
		m_position(position),
		m_type(type)
	{
	}

	StaticCollisionObject::~StaticCollisionObject()
	{
	}

    void StaticCollisionObject::handleCollisionWithParticle(Vector3D* particlePosition, Vector3D* particleVelocity, float particleRadius, float restitutionCoefficient, float frictionCoefficient) const
	{
		StaticCollisionInfo* collisionInfo;
		collisionInfo = detectCollisionWithParticle(*particlePosition, particleRadius);
		if (collisionInfo != NULL)
		{
            resolveCollisionWithParticle(particlePosition, particleVelocity, restitutionCoefficient, frictionCoefficient, collisionInfo);
		}
	}

    void StaticCollisionObject::resolveCollisionWithParticle(Vector3D* particlePosition, Vector3D* particleVelocity, float restitutionCoefficient, float frictionCoefficient, StaticCollisionInfo* collisionInfo) const
	{
		float separatingVelocity = *particleVelocity * collisionInfo->collisionNormal;
		// Check if velocity is facing opposite direction of the contactNormal
		if (separatingVelocity < 0.f)
		{
			Vector3D separatingVelocityN = collisionInfo->collisionNormal * separatingVelocity;
			Vector3D separatingVelocityT = *particleVelocity - separatingVelocityN;
			// resolve velocity
			separatingVelocityN *= -restitutionCoefficient;
            separatingVelocityT *= frictionCoefficient;
			*particleVelocity = separatingVelocityN + separatingVelocityT;
		}
		// resolve position
		*particlePosition = collisionInfo->collisionPoint;

		delete collisionInfo;
	}


	Vector3D StaticCollisionObject::getPosition() const
	{
		return m_position;
	}

	void StaticCollisionObject::setPosition(const Vector3D& position)
	{
        m_position = position;
    }

    StaticCollisionObjectType StaticCollisionObject::getType() const
    {
        return m_type;
    }
}

#include "StaticCollisionBox.h"

namespace LiPhEn {
	StaticCollisionBox::StaticCollisionBox(Vector3D position, Vector3D halfDimensions, StaticCollisionObjectType type) :
		m_halfDimensions(halfDimensions),
		StaticCollisionObject(position, type)
	{
	}

	StaticCollisionBox::~StaticCollisionBox()
	{
	}

	StaticCollisionInfo* StaticCollisionBox::detectCollisionWithParticle(const Vector3D& particlePosition, float particleRadius) const
	{
		StaticCollisionInfo* collisionInfo = NULL;

		Vector3D penetration;
		Vector3D distance = (m_position - particlePosition).abs();

		if (m_type == StaticCollisionObjectType::OBSTACLE)
		{
			penetration = (m_halfDimensions - distance).addScalar(particleRadius);
			if (penetration.getX() > 0.f && penetration.getY() > 0.f && penetration.getZ() > 0.f)
			{
				collisionInfo = new StaticCollisionInfo();
				collisionInfo->collisionPoint = particlePosition;

				// Check if penetration on X-Axis is the smallest
				if (penetration.getX() <= penetration.getY() && penetration.getX() <= penetration.getZ())
				{
					if (particlePosition.getX() - m_position.getX() >= 0.f)
                        collisionInfo->collisionNormal = Vector3D(1.f, 0.f, 0.f);
					else
                        collisionInfo->collisionNormal = Vector3D(-1.f, 0.f, 0.f);

					collisionInfo->collisionPoint.setX(m_position.getX() + collisionInfo->collisionNormal.getX() * (m_halfDimensions.getX() + particleRadius));
				}
				// Check if penetration on Y-Axis is the smallest
				else if (penetration.getY() <= penetration.getX() && penetration.getY() <= penetration.getZ())
				{
                    if (particlePosition.getY() - m_position.getY() >= 0.f)
                        collisionInfo->collisionNormal = Vector3D(0.f, 1.f, 0.f);
					else
                        collisionInfo->collisionNormal = Vector3D(0.f, -1.f, 0.f);

					collisionInfo->collisionPoint.setY(m_position.getY() + collisionInfo->collisionNormal.getY() * (m_halfDimensions.getY() + particleRadius));
				}
				// Check if penetration on Z-Axis is the smallest
				else if (penetration.getZ() <= penetration.getY() && penetration.getZ() <= penetration.getX())
				{
                    if (particlePosition.getZ() - m_position.getZ() >= 0.f)
                        collisionInfo->collisionNormal = Vector3D(0.f, 0.f, 1.f);
					else
                        collisionInfo->collisionNormal = Vector3D(0.f, 0.f, -1.f);

					collisionInfo->collisionPoint.setZ(m_position.getZ() + collisionInfo->collisionNormal.getZ() * (m_halfDimensions.getZ() + particleRadius));
				}
			}
		}
		else if (m_type == StaticCollisionObjectType::BOUNDARY)
		{
			penetration = (distance - m_halfDimensions).addScalar(particleRadius);
			if (penetration.getX() > 0.f || penetration.getY() > 0.f || penetration.getZ() > 0.f)
			{
				collisionInfo = new StaticCollisionInfo();
				collisionInfo->collisionPoint = particlePosition;
				collisionInfo->collisionNormal = Vector3D(0.f, 0.f, 0.f);

				// Check if there is penetration on X-Axis
				if (penetration.getX() > 0.f)
				{
					if (particlePosition.getX() - m_position.getX() <= 0.f)
						collisionInfo->collisionNormal.setX(1.f);
					else
						collisionInfo->collisionNormal.setX(-1.f);

					collisionInfo->collisionPoint.setX(m_position.getX() - collisionInfo->collisionNormal.getX() * (m_halfDimensions.getX() - particleRadius));
				}
				// Check if there is penetration on Y-Axis
				if (penetration.getY() > 0.f)
				{
					if (particlePosition.getY() - m_position.getY() <= 0.f)
						collisionInfo->collisionNormal.setY(1.f);
					else
						collisionInfo->collisionNormal.setY(-1.f);

					collisionInfo->collisionPoint.setY(m_position.getY() - collisionInfo->collisionNormal.getY() * (m_halfDimensions.getY() - particleRadius));
				}
				// Check if there is penetration on Z-Axis
				if (penetration.getZ() > 0.f)
				{
					if (particlePosition.getZ() - m_position.getZ() <= 0.f)
						collisionInfo->collisionNormal.setZ(1.f);
					else
						collisionInfo->collisionNormal.setZ(-1.f);

					collisionInfo->collisionPoint.setZ(m_position.getZ() - collisionInfo->collisionNormal.getZ() * (m_halfDimensions.getZ() - particleRadius));
				}

				collisionInfo->collisionNormal.normalize();
			}
		}
		return collisionInfo;
	}

	Vector3D StaticCollisionBox::getHalfDimensions() const
	{
		return m_halfDimensions;
	}

	void StaticCollisionBox::setHalfDimensions(const Vector3D& halfDimensions)
	{
		m_halfDimensions = halfDimensions;
	}
}

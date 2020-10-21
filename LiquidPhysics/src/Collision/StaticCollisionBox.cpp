#include "Collision/StaticCollisionBox.h"

namespace LiPhEn {
	StaticCollisionBox::StaticCollisionBox(Vector3D position, Vector3D halfDimensions, StaticCollisionObjectType type) :
		m_halfDimensions(halfDimensions),
		StaticCollisionObject(position, type)
	{
	}

	StaticCollisionInfo StaticCollisionBox::detectCollisionWithParticle(ParticleCollisionData particleData) const
	{
		Vector3D distance = (m_position - particleData.position).abs();

		Vector3D penetration = calcPenetration(particleData, distance);
		StaticCollisionInfo collisionInfo = calcCollisionInfo(particleData, penetration);

		return collisionInfo;
	}

	Vector3D StaticCollisionBox::calcPenetration(ParticleCollisionData particleData, Vector3D distance) const
	{
		Vector3D penetration = Vector3D(0.f, 0.f, 0.f);

		if (m_type == StaticCollisionObjectType::OBSTACLE)
			penetration = (m_halfDimensions - distance).addScalar(particleData.radius);
		else if (m_type == StaticCollisionObjectType::BOUNDARY)
			penetration = (distance - m_halfDimensions).addScalar(particleData.radius);

		return penetration;
	}

	StaticCollisionInfo StaticCollisionBox::calcCollisionInfo(ParticleCollisionData particleData, Vector3D penetration) const
	{
		StaticCollisionInfo collisionInfo;
		collisionInfo.collisionNormal = Vector3D(0.f, 0.f, 0.f);
		collisionInfo.collisionPoint = particleData.position;
		collisionInfo.foundCollision = false;

		if (m_type == StaticCollisionObjectType::OBSTACLE)
		{
			if (penetration.getX() > 0.f && penetration.getY() > 0.f && penetration.getZ() > 0.f)
			{
				collisionInfo.foundCollision = true;

				// Check if penetration on X-Axis is the smallest
				if (penetration.getX() <= penetration.getY() && penetration.getX() <= penetration.getZ())
				{
					if (particleData.position.getX() - m_position.getX() >= 0.f)
						collisionInfo.collisionNormal.setX(1.f);
					else
						collisionInfo.collisionNormal.setX(-1.f);

					collisionInfo.collisionPoint.setX(m_position.getX() + collisionInfo.collisionNormal.getX() * (m_halfDimensions.getX() + particleData.radius));
				}
					// Check if penetration on Y-Axis is the smallest
				else if (penetration.getY() <= penetration.getX() && penetration.getY() <= penetration.getZ())
				{
					if (particleData.position.getY() - m_position.getY() >= 0.f)
						collisionInfo.collisionNormal.setY(1.f);
					else
						collisionInfo.collisionNormal.setY(-1.f);

					collisionInfo.collisionPoint.setY(m_position.getY() + collisionInfo.collisionNormal.getY() * (m_halfDimensions.getY() + particleData.radius));
				}
					// Check if penetration on Z-Axis is the smallest
				else if (penetration.getZ() <= penetration.getY() && penetration.getZ() <= penetration.getX())
				{
					if (particleData.position.getZ() - m_position.getZ() >= 0.f)
						collisionInfo.collisionNormal.setZ(1.f);
					else
						collisionInfo.collisionNormal.setZ(-1.f);

					collisionInfo.collisionPoint.setZ(m_position.getZ() + collisionInfo.collisionNormal.getZ() * (m_halfDimensions.getZ() + particleData.radius));
				}
			}
		}
		else if (m_type == StaticCollisionObjectType::BOUNDARY)
		{
			if (penetration.getX() > 0.f || penetration.getY() > 0.f || penetration.getZ() > 0.f)
			{
				collisionInfo.foundCollision = true;

				// Check if there is penetration on X-Axis
				if (penetration.getX() > 0.f)
				{
					if (particleData.position.getX() - m_position.getX() <= 0.f)
						collisionInfo.collisionNormal.setX(1.f);
					else
						collisionInfo.collisionNormal.setX(-1.f);

					collisionInfo.collisionPoint.setX(m_position.getX() - collisionInfo.collisionNormal.getX() * (m_halfDimensions.getX() - particleData.radius));
				}
				// Check if there is penetration on Y-Axis
				if (penetration.getY() > 0.f)
				{
					if (particleData.position.getY() - m_position.getY() <= 0.f)
						collisionInfo.collisionNormal.setY(1.f);
					else
						collisionInfo.collisionNormal.setY(-1.f);

					collisionInfo.collisionPoint.setY(m_position.getY() - collisionInfo.collisionNormal.getY() * (m_halfDimensions.getY() - particleData.radius));
				}
				// Check if there is penetration on Z-Axis
				if (penetration.getZ() > 0.f)
				{
					if (particleData.position.getZ() - m_position.getZ() <= 0.f)
						collisionInfo.collisionNormal.setZ(1.f);
					else
						collisionInfo.collisionNormal.setZ(-1.f);

					collisionInfo.collisionPoint.setZ(m_position.getZ() - collisionInfo.collisionNormal.getZ() * (m_halfDimensions.getZ() - particleData.radius));
				}
				collisionInfo.collisionNormal.normalize();
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

#pragma once

#include "Collision/StaticCollisionObject.h"

namespace LiPhEn {
	class StaticCollisionBox : public StaticCollisionObject
	{
	private:
		Vector3D m_halfDimensions;

	public:
		StaticCollisionBox(Vector3D position, Vector3D halfDimensions, StaticCollisionObjectType type = StaticCollisionObjectType::OBSTACLE);

		Vector3D getHalfDimensions() const;
		void setHalfDimensions(const Vector3D& halfDimensions);

	private:
		virtual StaticCollisionInfo detectCollisionWithParticle(ParticleCollisionData particleData) const;
		Vector3D calcPenetration(ParticleCollisionData particleData, Vector3D distance) const;
		StaticCollisionInfo calcCollisionInfo(ParticleCollisionData particleData, Vector3D penetration) const;
	};
}


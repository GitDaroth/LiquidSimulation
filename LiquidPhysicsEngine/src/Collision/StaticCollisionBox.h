#pragma once

#include "StaticCollisionObject.h"

namespace LiPhEn {
	class StaticCollisionBox : public StaticCollisionObject
	{
	public:
		StaticCollisionBox(Vector3D position, Vector3D halfDimensions, StaticCollisionObjectType type = StaticCollisionObjectType::OBSTACLE);
		~StaticCollisionBox();

		Vector3D getHalfDimensions() const;
		void setHalfDimensions(const Vector3D& halfDimensions);

	protected:
		virtual StaticCollisionInfo* detectCollisionWithParticle(const Vector3D& particlePosition, float particleRadius) const;

		Vector3D m_halfDimensions;
	};
}


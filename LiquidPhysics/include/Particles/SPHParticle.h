#pragma once

#include "Math/Vector3D.h"

namespace LiPhEn {
	class SPHParticle
	{
	public:
		SPHParticle();
		virtual ~SPHParticle();

		void clearAccumulatedForces();              
		void addForce(const Vector3D& force);
		void integrate(float deltaTime);
		void aproximateVelocity();

		Vector3D getPosition() const;
		Vector3D getVelocity() const;
		Vector3D getHalfVelocity() const;
		Vector3D getAccumulatedForces() const;
		float getDensity() const;
		float getPressure() const;
		bool getIsFirstTimeStep() const;

		void setPosition(const Vector3D& position);
		void setVelocity(const Vector3D& velocity);
		void setHalfVelocity(const Vector3D& halfVelocity);
		void setOldHalfVelocity(const Vector3D& oldHalfVelocity);
		void setDensity(const float density);
		void setPressure(const float pressure);
		void setIsFirstTimeStep(const bool isFirstTimeStep);

	protected:
		Vector3D m_position;
		Vector3D m_velocity;
		Vector3D m_halfVelocity;
		Vector3D m_oldHalfVelocity;
		Vector3D m_accumulatedForces;
		float m_density;
		float m_pressure;
		bool m_isFirstTimeStep;
	};
}

#pragma once

#include "SPHParticle.h"

namespace LiPhEn {
	class PCISPHParticle : public SPHParticle
	{
	public:
		PCISPHParticle();
		~PCISPHParticle();

		float getPredictedDensity() const;
		float getDensityError() const;
		Vector3D getPredictedPosition() const;
		Vector3D getPredictedHalfVelocity() const;
		Vector3D getPredictedPressureForce() const;

		void setPredictedDensity(const float predictedDensity);
		void setDensityError(const float densityError);
		void setPredictedPosition(const Vector3D& predictedPosition);
		void setPredictedHalfVelocity(const Vector3D& predictedVelocity);
		void setPredictedPressureForce(const Vector3D& predictedPressureForce);

	protected:
		float m_predictedDensity;
		float m_densityError;
		Vector3D m_predictedPosition;
		Vector3D m_predictedHalfVelocity;
		Vector3D m_predictedPressureForce;
	};
}

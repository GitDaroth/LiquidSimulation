#include "PCISPHParticle.h"

namespace LiPhEn {
	PCISPHParticle::PCISPHParticle() :
		m_predictedPosition(Vector3D(0.f, 0.f, 0.f)),
		m_predictedHalfVelocity(Vector3D(0.f, 0.f, 0.f)),
		m_predictedPressureForce(Vector3D(0.f, 0.f, 0.f)),
		m_predictedDensity(0.f),
		m_densityError(0.f)
	{
	}

	PCISPHParticle::~PCISPHParticle()
	{
	}

	// GETTER
	float PCISPHParticle::getPredictedDensity() const
	{
		return m_predictedDensity;
	}

	float PCISPHParticle::getDensityError() const
	{
		return m_densityError;
	}

	Vector3D PCISPHParticle::getPredictedPosition() const
	{
		return m_predictedPosition;
	}

	Vector3D PCISPHParticle::getPredictedHalfVelocity() const
	{
		return m_predictedHalfVelocity;
	}

	Vector3D PCISPHParticle::getPredictedPressureForce() const
	{
		return m_predictedPressureForce;
	}

	// SETTER
	void PCISPHParticle::setPredictedDensity(const float predictedDensity)
	{
		m_predictedDensity = predictedDensity;
	}

	void PCISPHParticle::setDensityError(const float densityError)
	{
		m_densityError = densityError;
	}

	void PCISPHParticle::setPredictedPosition(const Vector3D& predictedPosition)
	{
		m_predictedPosition = predictedPosition;
	}

	void PCISPHParticle::setPredictedHalfVelocity(const Vector3D& predictedVelocity)
	{
		m_predictedHalfVelocity = predictedVelocity;
	}

	void PCISPHParticle::setPredictedPressureForce(const Vector3D& predictedPressureForce)
	{
		m_predictedPressureForce = predictedPressureForce;
	}

}

#include "Particles/SPHParticle.h"

namespace LiPhEn {
	SPHParticle::SPHParticle() :
		m_isFirstTimeStep(true),
		m_position(Vector3D(0.f, 0.f, 0.f)),
		m_velocity(Vector3D(0.f, 0.f, 0.f)),
		m_halfVelocity(Vector3D(0.f, 0.f, 0.f)),
		m_oldHalfVelocity(Vector3D(0.f, 0.f, 0.f)),
		m_accumulatedForces(Vector3D(0.f, 0.f, 0.f)),
		m_density(0.f),
		m_pressure(0.f)
	{
	}

	SPHParticle::~SPHParticle()
	{
	}

	void SPHParticle::clearAccumulatedForces()
	{
		m_accumulatedForces.clear();
	}

	void SPHParticle::addForce(const Vector3D& force)
	{
		m_accumulatedForces += force;
	}

	void SPHParticle::integrate(float deltaTime)
	{
		Vector3D acceleration = m_accumulatedForces / m_density;
		if (m_isFirstTimeStep)
		{
			m_halfVelocity = m_velocity - acceleration * deltaTime / 2.f;
			m_isFirstTimeStep = false;
		}

		m_oldHalfVelocity = m_halfVelocity;
		m_halfVelocity.addScaledVector(acceleration, deltaTime);
		m_position.addScaledVector(m_halfVelocity, deltaTime);

		clearAccumulatedForces();
	}

	void SPHParticle::aproximateVelocity()
	{
		m_velocity = (m_oldHalfVelocity + m_halfVelocity) / 2.f;
	}

	// GETTERS
	Vector3D SPHParticle::getPosition() const
	{
		return m_position;
	}

	Vector3D SPHParticle::getVelocity() const
	{
		return m_velocity;
	}

	Vector3D SPHParticle::getHalfVelocity() const
	{
		return m_halfVelocity;
	}

	Vector3D SPHParticle::getAccumulatedForces() const
	{
		return m_accumulatedForces;
	}

	float SPHParticle::getDensity() const
	{
		return m_density;
	}

	float SPHParticle::getPressure() const
	{
		return m_pressure;
	}

	bool SPHParticle::getIsFirstTimeStep() const
	{
		return m_isFirstTimeStep;
	}

	// SETTERS
	void SPHParticle::setPosition(const Vector3D& position)
	{
		m_position = position;
	}

	void SPHParticle::setVelocity(const Vector3D& velocity)
	{
		m_velocity = velocity;
	}

	void SPHParticle::setHalfVelocity(const Vector3D& halfVelocity)
	{
		m_halfVelocity = halfVelocity;
	}

	void SPHParticle::setOldHalfVelocity(const Vector3D& oldHalfVelocity)
	{
		m_oldHalfVelocity = oldHalfVelocity;
	}

	void SPHParticle::setDensity(const float density)
	{
		m_density = density;
	}

	void SPHParticle::setPressure(const float pressure)
	{
		m_pressure = pressure;
	}

	void SPHParticle::setIsFirstTimeStep(const bool isFirstTimeStep)
	{
		m_isFirstTimeStep = isFirstTimeStep;
	}
}
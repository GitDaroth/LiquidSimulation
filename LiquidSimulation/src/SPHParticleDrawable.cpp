#include "SPHParticleDrawable.h"

SPHParticleDrawable::SPHParticleDrawable(SPHParticle* particle, InstancedDrawable* instancedDrawable) :
    m_particle(particle),
    m_instancedDrawable(instancedDrawable)
{

}

void SPHParticleDrawable::update(float radius)
{
    float velocitySquared = m_particle->getVelocity().squareMagnitude();
    float velocityRatio = std::min(velocitySquared / 6.f, 1.f);

	if (std::isnan(velocityRatio))
		velocityRatio = 0.f;

	int r = (int)(velocityRatio * 181);
	int g = 66 + (int)(velocityRatio * 181);
	int b = 174 + (int)(velocityRatio * 81);

	m_instancedDrawable->color = QColor(r, g, b);
    m_instancedDrawable->scale = QVector3D(radius, radius, radius);
    m_instancedDrawable->translation = QVector3D(m_particle->getPosition().getX(),
                                                 m_particle->getPosition().getY(),
                                                 m_particle->getPosition().getZ());
}

SPHParticle* SPHParticleDrawable::getParticle()
{
    return m_particle;
}

InstancedDrawable* SPHParticleDrawable::getInstancedDrawable()
{
    return m_instancedDrawable;
}

void SPHParticleDrawable::setParticle(SPHParticle* particle)
{
    m_particle = particle;
}

void SPHParticleDrawable::setInstancedDrawable(InstancedDrawable* instancedDrawable)
{
    m_instancedDrawable = instancedDrawable;
}

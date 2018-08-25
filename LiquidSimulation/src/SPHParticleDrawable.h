#ifndef SPHPARTICLEDRAWABLE_H
#define SPHPARTICLEDRAWABLE_H

#include "Particles/PCISPHParticle.h"
#include "Rendering/OpenGLWidget.h"

using namespace LiPhEn;

class SPHParticleDrawable
{
public:
    SPHParticleDrawable(SPHParticle* particle, InstancedDrawable* instancedDrawable);

    void update(float radius);

    SPHParticle* getParticle();
    InstancedDrawable* getInstancedDrawable();

    void setParticle(SPHParticle* particle);
    void setInstancedDrawable(InstancedDrawable* instancedDrawable);

private:
    SPHParticle* m_particle;
    InstancedDrawable* m_instancedDrawable;
};

#endif // SPHPARTICLEDRAWABLE_H

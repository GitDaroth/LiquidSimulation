#include "SPHLiquidWorld.h"
#include <QElapsedTimer>

SPHLiquidWorld::SPHLiquidWorld(SPHSolver* sphSolver, OpenGLWidget* root) :
    m_sphSolver(sphSolver),
    m_root(root)
{

}

void SPHLiquidWorld::addSPHParticleDrawable(SPHParticleDrawable* particleDrawable)
{
    m_sphSolver->addParticle(particleDrawable->getParticle());
    m_root->addInstancedDrawable(particleDrawable->getInstancedDrawable());

    m_particleDrawables.append(particleDrawable);

    particleDrawable->update(m_sphSolver->getParticleRadius());
}

void SPHLiquidWorld::addStaticCollisionObjectDrawable(StaticCollisionObjectDrawable* collisionObjectDrawable)
{
    m_sphSolver->addStaticCollisionObject(collisionObjectDrawable->getCollisionObject());
    m_root->addDrawable(collisionObjectDrawable->getDrawable());

    m_collisionObjectDrawables.append(collisionObjectDrawable);

    collisionObjectDrawable->update();
}

void SPHLiquidWorld::update(float deltaTime)
{
    m_sphSolver->update(deltaTime);

    for(SPHParticleDrawable* particleDrawable : m_particleDrawables)
    {
        particleDrawable->update(m_sphSolver->getParticleRadius());
    }

    for(StaticCollisionObjectDrawable* collisionObjectDrawable : m_collisionObjectDrawables)
    {
        collisionObjectDrawable->update();
    }
}

void SPHLiquidWorld::cleanUp()
{
    m_collisionObjectDrawables.clear();
    m_particleDrawables.clear();
    m_sphSolver->cleanUp();
    m_root->cleanUp();
}

SPHSolver* SPHLiquidWorld::getSPHSolver()
{
    return m_sphSolver;
}

OpenGLWidget* SPHLiquidWorld::getRoot()
{
    return m_root;
}

void SPHLiquidWorld::setSPHSolver(SPHSolver* solver)
{
	m_sphSolver = solver;
}

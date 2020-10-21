#ifndef SPHLIQUIDWORLD_H
#define SPHLIQUIDWORLD_H

#include "SPHParticleDrawable.h"
#include "StaticCollisionObjectDrawable.h"
#include <PCISPHSolver.h>
#include <Collision/StaticCollisionBox.h>

class SPHLiquidWorld
{
public:
    SPHLiquidWorld(SPHSolver* sphSolver, OpenGLWidget* root);

    void addSPHParticleDrawable(SPHParticleDrawable* particleDrawable);
    void addStaticCollisionObjectDrawable(StaticCollisionObjectDrawable* collisionObjectDrawable);
    void update(float deltaTime);
    void cleanUp();

    SPHSolver* getSPHSolver();
    OpenGLWidget* getRoot();

	void setSPHSolver(SPHSolver* solver);

private:
    SPHSolver* m_sphSolver;
    OpenGLWidget* m_root;

    QVector<SPHParticleDrawable*> m_particleDrawables;
    QVector<StaticCollisionObjectDrawable*> m_collisionObjectDrawables;
};

#endif // SPHLIQUIDWORLD_H

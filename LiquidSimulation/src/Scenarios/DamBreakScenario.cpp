#include "DamBreakScenario.h"

DamBreakScenario::DamBreakScenario(QString name, SPHLiquidWorld* sphFluidWorld) :
	LiquidScenario(name, sphFluidWorld)
{
	m_specificScenarioWidget = new QWidget();
}

void DamBreakScenario::initScenario()
{
	std::vector<Vector3D> cubeParticles = SPHParticleEmitter::spawnCube(Vector3D(-0.6f, -0.2f, 0.f), Vector3D(0.4f, 0.4f, 0.6f), m_sphLiquidWorld->getSPHSolver()->getParticleRadius());

	for (Vector3D particlePosition : cubeParticles)
	{
		SPHParticle* sphParticle = new PCISPHParticle();
		sphParticle->setPosition(particlePosition);

		SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
		m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
	}

	Vector3D halfDimensions(1.f, 0.6f, 0.6f);

	StaticCollisionBox* m_boundaryBox = new StaticCollisionBox(Vector3D(0.f, 0.f, 0.f), halfDimensions, StaticCollisionObjectType::BOUNDARY);
	StaticCollisionObjectDrawable* collisionObjectDrawable = new StaticCollisionObjectDrawable(m_boundaryBox, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(collisionObjectDrawable);

	StaticCollisionBox* obstacleBox = new StaticCollisionBox(Vector3D(0.4f, -0.2f, -0.4f), Vector3D(0.2f, 0.6f, 0.2f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable = new StaticCollisionObjectDrawable(obstacleBox, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable);

	StaticCollisionSphere* obstacleSphere = new StaticCollisionSphere(Vector3D(0.4f, -0.25f, 0.6f), 0.3f, StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleSphereDrawable = new StaticCollisionObjectDrawable(obstacleSphere, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleSphereDrawable);
}

void DamBreakScenario::updateScenario(float deltaTime)
{

}

void DamBreakScenario::buildScenarioWidget()
{

}

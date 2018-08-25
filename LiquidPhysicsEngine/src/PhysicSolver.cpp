#include "PhysicSolver.h"

namespace LiPhEn {
	PhysicSolver::PhysicSolver()
	{
	}

	PhysicSolver::~PhysicSolver()
	{
	}

	void PhysicSolver::update(float deltaTime)
	{
		onBeginUpdate();

		accumulateForces(deltaTime);
		integrate(deltaTime);
		handleCollisions();

		onEndUpdate();
	}
}
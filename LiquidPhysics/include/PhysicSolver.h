#pragma once

namespace LiPhEn {
	class PhysicSolver
	{
	public:
		PhysicSolver();
		virtual ~PhysicSolver();

		void update(float deltaTime);

	protected:
		virtual void onBeginUpdate() = 0;
		virtual void accumulateForces(float deltaTime) = 0;
		virtual void integrate(float deltaTime) = 0;
		virtual void handleCollisions() = 0;
		virtual void onEndUpdate() = 0;
	};
}

#pragma once

#include "SPHKernel.h"

namespace LiPhEn {
	class ViscosityKernel : public SPHKernel
	{
	public:
		ViscosityKernel();
		~ViscosityKernel();

		virtual float calcKernelWeight(float distance) const;
		virtual float calcFirstDerivativeWeight(float distance) const;
		virtual float calcSecondDerivativeWeight(float distance) const;

	protected:
		virtual void recalcKernelCoefficient();
		virtual void recalcFirstDerivativeCoefficient();
		virtual void recalcSecondDerivativeCoefficient();
	};
}


#pragma once

#include "SPHKernel.h"

namespace LiPhEn {
	class PressureKernel : public SPHKernel
	{
	public:
		PressureKernel();
		~PressureKernel();

		virtual float calcKernelWeight(float distance) const;
		virtual float calcFirstDerivativeWeight(float distance) const;
		virtual float calcSecondDerivativeWeight(float distance) const;

	protected:
		virtual void recalcKernelCoefficient();
		virtual void recalcFirstDerivativeCoefficient();
		virtual void recalcSecondDerivativeCoefficient();
	};
}
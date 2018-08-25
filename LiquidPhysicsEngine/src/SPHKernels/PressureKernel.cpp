#include "PressureKernel.h"

namespace LiPhEn {
	PressureKernel::PressureKernel()
	{
	}

	PressureKernel::~PressureKernel()
	{
	}

	float PressureKernel::calcKernelWeight(float distance) const
	{
		float temp = m_radius - distance;
		return m_kernelCoefficient * temp * temp * temp;
	}

	float PressureKernel::calcFirstDerivativeWeight(float distance) const
	{
		float temp = m_radius - distance;
		return m_firstDerivativeCoefficient * temp * temp;
	}

	float PressureKernel::calcSecondDerivativeWeight(float distance) const
	{
		return m_secondDerivativeCoefficient * (m_radius - distance) * (m_radius - 2.f * distance) / m_radius;
	}

	void PressureKernel::recalcKernelCoefficient()
	{
		m_kernelCoefficient = 15.f / (M_PI * m_radius6);
	}

	void PressureKernel::recalcFirstDerivativeCoefficient()
	{
		m_firstDerivativeCoefficient = -45.f / (M_PI * m_radius6);
	}

	void PressureKernel::recalcSecondDerivativeCoefficient()
	{
		m_secondDerivativeCoefficient = -90.f / (M_PI * m_radius6);
	}
}

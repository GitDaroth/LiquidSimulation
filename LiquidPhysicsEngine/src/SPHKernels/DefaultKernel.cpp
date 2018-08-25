#include "DefaultKernel.h"

namespace LiPhEn {
	DefaultKernel::DefaultKernel()
	{
	}

	DefaultKernel::~DefaultKernel()
	{
	}

	float DefaultKernel::calcKernelWeight(float distance) const
	{
		float temp = m_radius2 - distance * distance;
		return m_kernelCoefficient * temp * temp * temp;
	}

	float DefaultKernel::calcFirstDerivativeWeight(float distance) const
	{
		float temp = m_radius2 - distance * distance;
		return m_firstDerivativeCoefficient * temp * temp;
	}

	float DefaultKernel::calcSecondDerivativeWeight(float distance) const
	{
		return m_secondDerivativeCoefficient * (m_radius2 - distance * distance) * (3.f * m_radius2 - 7.f * distance * distance);
	}

	void DefaultKernel::recalcKernelCoefficient()
	{
		m_kernelCoefficient = 315.f / (64.f * M_PI * m_radius9);
	}

	void DefaultKernel::recalcFirstDerivativeCoefficient()
	{
		m_firstDerivativeCoefficient = -945.f / (32.f * M_PI * m_radius9);
	}

	void DefaultKernel::recalcSecondDerivativeCoefficient()
	{
		m_secondDerivativeCoefficient = -945.f / (32.f * M_PI * m_radius9);
	}
}

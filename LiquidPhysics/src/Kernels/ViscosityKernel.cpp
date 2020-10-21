#include "Kernels/ViscosityKernel.h"

namespace LiPhEn {
	ViscosityKernel::ViscosityKernel()
	{
	}

	ViscosityKernel::~ViscosityKernel()
	{
	}

	float ViscosityKernel::calcKernelWeight(float distance) const
	{
		return m_kernelCoefficient * ((distance * distance) / m_radius2 - (distance * distance * distance) / (2.f * m_radius3) + m_radius / (2.f * distance) - 1.f);
	}

	float ViscosityKernel::calcFirstDerivativeWeight(float distance) const
	{
		return m_firstDerivativeCoefficient * (2.f / m_radius2 - (3.f * distance) / (2.f * m_radius3) - m_radius / (2.f * distance * distance * distance));
	}

	float ViscosityKernel::calcSecondDerivativeWeight(float distance) const
	{
		return m_secondDerivativeCoefficient * (m_radius - distance);
	}

	void ViscosityKernel::recalcKernelCoefficient()
	{
		m_kernelCoefficient = 15.f / (2.f * M_PI * m_radius3);
	}

	void ViscosityKernel::recalcFirstDerivativeCoefficient()
	{
		m_firstDerivativeCoefficient = 15.f / (2.f * M_PI * m_radius3);
	}

	void ViscosityKernel::recalcSecondDerivativeCoefficient()
	{
		m_secondDerivativeCoefficient = 45.f / (M_PI * m_radius6);
	}
}

#include "Kernels/SPHKernel.h"

namespace LiPhEn {
	SPHKernel::SPHKernel() :
		m_subdivision(1000)
	{
	}

	SPHKernel::~SPHKernel()
	{
	}

	float SPHKernel::getKernelWeight(float distance) const
	{
		if (distance >= m_radius)
			return 0.f;
		else
		{
			int key = distance / m_divisionStep;
			return m_cachedKernelWeights.at(key);
		}
	}

	float SPHKernel::getFirstDerivativeWeight(float distance) const
	{
		if (distance >= m_radius)
			return 0.f;
		else
		{
			int key = distance / m_divisionStep;
			return m_cachedFirstDerivativeWeights.at(key);
		}
	}

	float SPHKernel::getSecondDerivativeWeight(float distance) const
	{
		if (distance >= m_radius)
			return 0.f;
		else
		{
			int key = distance / m_divisionStep;
			return m_cachedSecondDerivativeWeights.at(key);
		}
	}

	// GETTER
	float SPHKernel::getRadius() const
	{
		return m_radius;
	}

	int SPHKernel::getSubdivision() const
	{
		return m_subdivision;
	}

	float SPHKernel::getDivisionStep() const
	{
		return m_divisionStep;
	}

	std::vector<float> SPHKernel::getKernelWeights()
	{
		return m_cachedKernelWeights;
	}

	std::vector<float> SPHKernel::getFirstDerivativeWeights()
	{
		return m_cachedFirstDerivativeWeights;
	}

	std::vector<float> SPHKernel::getSecondDerivativeWeights()
	{
		return m_cachedSecondDerivativeWeights;
	}

	float SPHKernel::getKernelCoefficient()
	{
		return m_kernelCoefficient;
	}

	float SPHKernel::getFirstDerivativeCoefficient()
	{
		return m_firstDerivativeCoefficient;
	}

	float SPHKernel::getSecondDerivativeCoefficient()
	{
		return m_secondDerivativeCoefficient;
	}

	// SETTER
	void SPHKernel::setRadius(float radius)
	{
		m_radius = radius;
		m_radius2 = m_radius * m_radius;
		m_radius3 = m_radius2 * m_radius;
		m_radius4 = m_radius3 * m_radius;
		m_radius5 = m_radius4 * m_radius;
		m_radius6 = m_radius5 * m_radius;
		m_radius9 = m_radius6 * m_radius3;

		recalcCachedWeights();
	}

	void SPHKernel::setSubdivision(int hashSubdivision)
	{
		m_subdivision = hashSubdivision;

		recalcCachedWeights();
	}

	void SPHKernel::recalcCachedWeights()
	{
        m_cachedKernelWeights.clear();
        m_cachedFirstDerivativeWeights.clear();
        m_cachedSecondDerivativeWeights.clear();

		recalcDivisionStep();
		recalcKernelCoefficient();
		recalcFirstDerivativeCoefficient();
		recalcSecondDerivativeCoefficient();

		for (int i = 0; i <= m_subdivision; i++)
		{
			float distance = i * m_divisionStep;

            m_cachedKernelWeights.push_back(calcKernelWeight(distance));
            m_cachedFirstDerivativeWeights.push_back(calcFirstDerivativeWeight(distance));
            m_cachedSecondDerivativeWeights.push_back(calcSecondDerivativeWeight(distance));
		}
	}

	void SPHKernel::recalcDivisionStep()
	{
		m_divisionStep = m_radius / m_subdivision;
	}
}

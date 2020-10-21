#pragma once

#define _USE_MATH_DEFINES

#include <unordered_map>
#include <vector>
#include <math.h>

namespace LiPhEn {
	class SPHKernel 
	{
	public:
		SPHKernel();
		virtual ~SPHKernel();

		float getKernelWeight(float distance) const;
		float getFirstDerivativeWeight(float distance) const;
		float getSecondDerivativeWeight(float distance) const;
		virtual float calcKernelWeight(float distance) const = 0;
		virtual float calcFirstDerivativeWeight(float distance) const = 0;
		virtual float calcSecondDerivativeWeight(float distance) const = 0;

		float getRadius() const;
		int getSubdivision() const;
		float getDivisionStep() const;
		std::vector<float> getKernelWeights();
		std::vector<float> getFirstDerivativeWeights();
		std::vector<float> getSecondDerivativeWeights();
		float getKernelCoefficient();
		float getFirstDerivativeCoefficient();
		float getSecondDerivativeCoefficient();

		void setRadius(float radius);
		void setSubdivision(int hashSubdivision);

	protected:
		virtual void recalcKernelCoefficient() = 0;
		virtual void recalcFirstDerivativeCoefficient() = 0;
		virtual void recalcSecondDerivativeCoefficient() = 0;

		float m_radius;
		float m_radius2;
		float m_radius3;
		float m_radius4;
		float m_radius5;
		float m_radius6;
		float m_radius9;

		float m_kernelCoefficient;
		float m_firstDerivativeCoefficient;
		float m_secondDerivativeCoefficient; 

        std::vector<float> m_cachedKernelWeights;
        std::vector<float> m_cachedFirstDerivativeWeights;
        std::vector<float> m_cachedSecondDerivativeWeights;
		int m_subdivision;
		float m_divisionStep;

	private:
		void recalcCachedWeights();
		void recalcDivisionStep();
	};
}

#pragma once

#include <unordered_map>
#include <vector>
#include "Particles/SPHParticle.h"

namespace LiPhEn {
	class SPHSpatialGrid
	{
	public:
		SPHSpatialGrid();
		SPHSpatialGrid(float gridSpacing);
		~SPHSpatialGrid();

		void build(std::vector<SPHParticle*> particles);
		void clear();
		std::vector<SPHParticle*> findNeighborParticles(SPHParticle* particle, float searchRadius);

		float getGridSpacing() const;
		void setGridSpacing(float gridSpacing);

	private:
		int calcHashValue(SPHParticle* particle);
		int calcHashValue(int i, int j, int k);

		std::unordered_map<int, std::vector<SPHParticle*>> m_hashedGrid;
		float m_gridSpacing;
	};
}
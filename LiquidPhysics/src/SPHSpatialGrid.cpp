#include "SPHSpatialGrid.h"

namespace LiPhEn {
	SPHSpatialGrid::SPHSpatialGrid() :
		SPHSpatialGrid(0.1f)
	{
	}

	SPHSpatialGrid::SPHSpatialGrid(float gridSize) :
		m_gridSpacing(gridSize)
	{
	}


	SPHSpatialGrid::~SPHSpatialGrid()
	{
	}

	void SPHSpatialGrid::build(std::vector<SPHParticle*> particles)
	{
		for (SPHParticle* particle : particles)
		{
			int hashValue = calcHashValue(particle);

			// if no element with the hashValue exists, it inserts a new element and calls the constructor of the value m_type (std::vector<SPHParticle*>)
			m_hashedGrid[hashValue].push_back(particle);
		}
	}

	void SPHSpatialGrid::clear()
	{
		for (std::pair<int, std::vector<SPHParticle*>> gridElement : m_hashedGrid)
		{
			gridElement.second.clear();
		}
		m_hashedGrid.clear();
	}

	std::vector<SPHParticle*> SPHSpatialGrid::findNeighborParticles(SPHParticle* particle, float searchRadius)
	{
		std::vector<SPHParticle*> neighborParticles;
		std::vector<SPHParticle*> potentialNeighborParticles;
		
		int iMin = (int)((particle->getPosition().getX() - searchRadius) / m_gridSpacing);
		int iMax = (int)((particle->getPosition().getX() + searchRadius) / m_gridSpacing);
		int jMin = (int)((particle->getPosition().getY() - searchRadius) / m_gridSpacing);
		int jMax = (int)((particle->getPosition().getY() + searchRadius) / m_gridSpacing);
		int kMin = (int)((particle->getPosition().getZ() - searchRadius) / m_gridSpacing);
		int kMax = (int)((particle->getPosition().getZ() + searchRadius) / m_gridSpacing);

		for (int i = iMin; i <= iMax; i++)
		{
			for (int j = jMin; j <= jMax; j++)
			{
				for (int k = kMin; k <= kMax; k++)
				{
					int hashValue = calcHashValue(i, j, k);
					if (m_hashedGrid.find(hashValue) != m_hashedGrid.end())
					{
						std::vector<SPHParticle*> gridParticles = m_hashedGrid[hashValue];
						potentialNeighborParticles.insert(potentialNeighborParticles.end(), gridParticles.begin(), gridParticles.end());
					}
				}
			}
		}

		for (SPHParticle* potentialNeighborParticle : potentialNeighborParticles)
		{
			if ((particle->getPosition() - potentialNeighborParticle->getPosition()).squareMagnitude() <= searchRadius * searchRadius)
			{
				neighborParticles.push_back(potentialNeighborParticle);
			}
		}

		return neighborParticles;
	}

	int SPHSpatialGrid::calcHashValue(SPHParticle* particle)
	{
		int i = (int)(particle->getPosition().getX() / m_gridSpacing);
		int j = (int)(particle->getPosition().getY() / m_gridSpacing);
		int k = (int)(particle->getPosition().getZ() / m_gridSpacing);
		
		return calcHashValue(i, j, k);
	}

	int SPHSpatialGrid::calcHashValue(int i, int j, int k)
	{
		return 541 * i + 79 * j + 31 * k;
	}

	float SPHSpatialGrid::getGridSpacing() const
	{
		return m_gridSpacing;
	}

	void SPHSpatialGrid::setGridSpacing(float gridSpacing)
	{
		m_gridSpacing = gridSpacing;
	}
}
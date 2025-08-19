#pragma once
#include <cassert>

#include "data_structures/Array.hpp"
#include "Point.hpp"
#include "Boundaries.hpp"
#include "KDTree.hpp"

class PoissonRadiusField
{
private:
	enum class GridNodeType;
private:
	Array<Array<double>> m_radiusGrid;
	Array<Array<GridNodeType>> m_nodeTypeGrid; // to identify "intirior" grid nodes for laplacian smoothing
	AABB m_boundingBox;
	double m_cellSize;
	double m_boundaryFactor = 1.5; // factor to determine if point is near boundary
	double m_growthFactor = 0.08; // factor increasing radius away from the boundaries
	double m_deepInteriorFactor = 5.0; // factor to determine if point is far away from boundaries
	int m_smoothingIterations = 100;
public:
	PoissonRadiusField(const Boundaries& boundaries);
	~PoissonRadiusField() = default;
	PoissonRadiusField(const PoissonRadiusField&) = delete;
	PoissonRadiusField(PoissonRadiusField&&) = delete;
	PoissonRadiusField& operator=(const PoissonRadiusField&) = delete;
	PoissonRadiusField& operator=(PoissonRadiusField&&) = delete;
	double getRadius(const Point& p) const;
private:
	void laplaceSmoothing(int iterations);
};

enum class PoissonRadiusField::GridNodeType
{
	INTERIOR,
	DEEP_INTERIOR,
	BOUNDARY,
	EXTERIOR
};

PoissonRadiusField::PoissonRadiusField(const Boundaries& boundaries)
{
	m_cellSize = 2.0 * boundaries.minDist();
	m_boundingBox = boundaries.getBoundingBox();
	double xMin = m_boundingBox.xMin;
	double yMin = m_boundingBox.yMin;
	double xSize = m_boundingBox.xMax - xMin;
	double ySize = m_boundingBox.yMax - yMin;
	double scale = std::max(xSize, ySize);
	// N x M grid - N in x direction, M in y direction
	size_t N = std::ceil(xSize / m_cellSize) + 1;
	size_t M = std::ceil(ySize / m_cellSize) + 1;
	m_radiusGrid.resize(N);
	for (auto& col : m_radiusGrid)
		col.resize(M);
	m_nodeTypeGrid.resize(N);
	for (auto&& col : m_nodeTypeGrid)
		col.resize(M);
	// load boundary points into an arry to construct KDTree
	Array<BoundaryPoint> boundaryPoints;
	for (size_t i = 0; i < boundaries.getOuterBoundary().size(); i++)
	{
		const auto& outer = boundaries.getOuterBoundary();
		size_t iNext = (i + 1) % outer.size();
		size_t iPrev = (i + outer.size() - 1) % outer.size();
		double meanNeighbourDistance = 0.5 * (dist(outer[i], outer[iNext]) + dist(outer[i], outer[iPrev]));
		boundaryPoints.pushBack(BoundaryPoint(outer[i], meanNeighbourDistance));
	}
	for (const auto& inner : boundaries.getInnerBoundaries())
		for (size_t i = 0; i < inner.size(); i++)
		{
			size_t iNext = (i + 1) % inner.size();
			size_t iPrev = (i + inner.size() - 1) % inner.size();
			double meanNeighbourDist = 0.5 * (dist(inner[i], inner[iNext]) + dist(inner[i], inner[iPrev]));
			boundaryPoints.pushBack(BoundaryPoint(inner[i], meanNeighbourDist));
		}
	KDTree<BoundaryPoint, 2> kdTree(boundaryPoints);
	// fill the grid 
	//// treshold to determine wheter grid node is boundary
	//double boundaryThreshold = boundaries.minDist() * 3.0;
	//// deep interior threshold to determine wheter grid node is far from boundaries
	//double deepInteriorThreshold = boundaryThreshold * m_deepInteriorFactor;
	for(size_t i = 0; i < N; i++)
		for (size_t j = 0; j < M; j++)
		{
			double x = m_boundingBox.xMin + static_cast<double>(i) * m_cellSize;
			double y = m_boundingBox.yMin + static_cast<double>(j) * m_cellSize;
			Point gridPoint{ x, y };
			BoundaryPoint nearest = kdTree.findNearest<Point>(gridPoint);
			double boundaryRadius = nearest.meanDist();
			double distanceToBoundary = dist(gridPoint, nearest);
			m_radiusGrid[i][j] = boundaryRadius + scale * m_growthFactor * distanceToBoundary;

			// treshold to determine wheter grid node is boundary
			double boundaryThreshold = nearest.meanDist() * m_boundaryFactor;
			// deep interior threshold to determine wheter grid node is far from boundaries
			double deepInteriorThreshold = boundaryThreshold * m_deepInteriorFactor;

			if (boundaries.pointInBoundaries(gridPoint))
			{
				if (dist(gridPoint, nearest) < boundaryThreshold)
				{
					m_nodeTypeGrid[i][j] = GridNodeType::BOUNDARY;
				}
				else if (distanceToBoundary >= deepInteriorThreshold)
				{
					m_nodeTypeGrid[i][j] = GridNodeType::DEEP_INTERIOR;
				}
				else
				{
					m_nodeTypeGrid[i][j] = GridNodeType::INTERIOR;
				}
			}
			else
				m_nodeTypeGrid[i][j] = GridNodeType::EXTERIOR;
		}
	laplaceSmoothing(m_smoothingIterations);
}

inline double PoissonRadiusField::getRadius(const Point& p) const
{
	assert(p[0] >= m_boundingBox.xMin && p[0] <= m_boundingBox.xMax
		&& p[1] >= m_boundingBox.yMin && p[1] <= m_boundingBox.yMax);

	double gridCoordX = (p[0] - m_boundingBox.xMin) / m_cellSize;
	double gridCoordY = (p[1] - m_boundingBox.yMin) / m_cellSize;
	size_t leftBottomI = static_cast<size_t>(std::floor(gridCoordX));
	size_t leftBottomJ = static_cast<size_t>(std::floor(gridCoordY));
	assert(leftBottomI >= 0 && leftBottomI + 1 < m_radiusGrid.size());
	assert(leftBottomJ >= 0 && leftBottomJ + 1 < m_radiusGrid[leftBottomI].size());
	double fractionX = gridCoordX - leftBottomI;
	double fractionY = gridCoordY - leftBottomJ;
	// bilinear interpolation
	// y interpolations (left, right)
	double left = m_radiusGrid[leftBottomI][leftBottomJ] * (1.0 - fractionY) + m_radiusGrid[leftBottomI][leftBottomJ + 1] * fractionY;
	double right = m_radiusGrid[leftBottomI + 1][leftBottomJ] * (1.0 - fractionY) + m_radiusGrid[leftBottomI + 1][leftBottomJ + 1] * fractionY;
	// x  interpolation
	return left * (1.0 - fractionX) + right * fractionX;

}

inline void PoissonRadiusField::laplaceSmoothing(int iterations)
{
	for(int it = 0; it < iterations; it++)
		for(size_t i = 0; i < m_radiusGrid.size(); i++)
			for (size_t j = 0; j < m_radiusGrid[i].size(); j++)
			{
				if (m_nodeTypeGrid[i][j] != GridNodeType::INTERIOR)
					continue;
				int validNeighbourCount = 0;
				double neighbourValueSum = 0.0;
				// +1, -1 and so on for neighbours
				// delta indices for 24 neighbours
				int di[24] = { 1, -1, 0, 0, 1, 1, -1, -1, 2, -2, 0,  0, 4, -4, 0, 0, 8, -8, 0, 0, 16, -16, 0, 0};
				int dj[24] = { 0, 0, 1, -1, 1, -1, 1, -1, 0,  0, 2, -2, 0, 0, 4, -4, 0, 0, 8, -8, 0, 0, 16, -16};
				// initial iterations long range, then more local smoothing
				int neighbours = 24;
				if (it >= static_cast<int>(iterations * 0.4))
					neighbours = 12;
				if (it >= static_cast<int>(iterations * 0.8))
					neighbours = 8;
				for (int k = 0; k < neighbours; k++)
				{
					long long niSigned = static_cast<long long>(i) + di[k];
					long long njSigned = static_cast<long long>(j) + dj[k];
					if (niSigned >= 0 && niSigned < m_radiusGrid.size() &&
						njSigned >= 0 && njSigned < m_radiusGrid[i].size())
					{
						size_t ni = static_cast<size_t>(niSigned);
						size_t nj = static_cast<size_t>(njSigned);
						if (m_nodeTypeGrid[ni][nj] != GridNodeType::EXTERIOR)
						{
							validNeighbourCount++;
							neighbourValueSum += m_radiusGrid[ni][nj];
						}
					}
					else
					{
						validNeighbourCount++;
						neighbourValueSum += m_radiusGrid[i][j];
					}
				}
				if (validNeighbourCount > 0)
					m_radiusGrid[i][j] = neighbourValueSum / static_cast<double>(validNeighbourCount);
			}
}

#pragma once

#include <memory>
#include <limits>

#include "data_structures/List.hpp"
#include "data_structures/Array.hpp"

#include "Point.hpp"
#include "Boundaries.hpp"
#include "PoissonRadiusField.hpp"
#include "tools/Random.hpp"

struct Cell
{
	size_t row, col;
	Cell(size_t i, size_t j) : row(i), col(j) {}
	Cell() : row(std::numeric_limits<size_t>::max()), col(std::numeric_limits<size_t>::max()) {}
};

class BridsonGrid
{
public:
	BridsonGrid(const Boundaries& boundaries);
	~BridsonGrid() = default;
	BridsonGrid(const BridsonGrid&) = delete;
	BridsonGrid(BridsonGrid&&) = delete;
	BridsonGrid& operator=(const BridsonGrid&) = delete;
	BridsonGrid& operator=(BridsonGrid&&) = delete;
	void generateInnerPoints(Array<Point>& points);
private:
	void addPoint(Array<Point>& points);
	bool filled() const;
private:
	Array<Array<std::unique_ptr<Point>>> m_grid;
	List<Cell> m_activeCells;
	PoissonRadiusField m_radiusField;
	const Boundaries& m_boundaries;
	double m_cellSize;
	double m_xSize;
	double m_ySize;
	const int m_maxAttempts = 50;
	AABB m_box;
};

BridsonGrid::BridsonGrid(const Boundaries& boundaries) : m_boundaries(boundaries), m_radiusField(boundaries)
{
	// bounding box for indices calculation
	m_box = boundaries.getBoundingBox();
	double xSize = m_box.xMax - m_box.xMin;
	double ySize = m_box.yMax - m_box.yMin;
	// cell size based on minimal radius
	double minRadius = boundaries.minDist() * 0.999;
	m_cellSize = minRadius / std::sqrt(2.0);
	// maximal indices
	size_t iMax = std::ceil(xSize / m_cellSize);
	size_t jMax = std::ceil(ySize / m_cellSize);
	m_grid.resize(iMax);
	for (auto& row : m_grid)
		row.resize(jMax);
	// insert boundary points into grid and active list
	for (const auto& p : boundaries.getOuterBoundary())
	{
		size_t i = std::floor((p[0] - m_box.xMin) / m_cellSize);
		size_t j = std::floor((p[1] - m_box.yMin) / m_cellSize);
		m_grid[i][j] = nullptr;
		m_grid[i][j] = std::make_unique<Point>(p);
		m_activeCells.pushBack(Cell(i, j));
	}
	for(const auto& inner : boundaries.getInnerBoundaries())
		for (const auto& p : inner)
		{
			size_t i = std::floor((p[0] - m_box.xMin) / m_cellSize);
			size_t j = std::floor((p[1] - m_box.yMin) / m_cellSize);
			m_grid[i][j] = std::make_unique<Point>(p);
			m_activeCells.pushBack(Cell(i, j));
		}
}

inline void BridsonGrid::generateInnerPoints(Array<Point>& points)
{
	while (!filled())
		addPoint(points);
}

inline void BridsonGrid::addPoint(Array<Point>& points)
{
	// attemot to generate a point around point in random active cell
	size_t activeCellIdx = Random::get<size_t>(0, m_activeCells.size() - 1);
	List<Cell>::Iterator activeCellIterator = m_activeCells.begin();
	if (activeCellIdx < m_activeCells.size() / 2)
	{
		for (size_t idx = 0; idx < activeCellIdx; idx++)
		{
			++activeCellIterator;
		}
	}
	else
	{
		activeCellIterator = m_activeCells.end();
		for (size_t idx = m_activeCells.size(); idx > activeCellIdx; idx--)
		{
			--activeCellIterator;
		}
	}
	const auto& cell = *activeCellIterator;
	const auto& point = m_grid[cell.row][cell.col];
	std::unique_ptr<Point> newPoint = nullptr;
	for (int i = 0; i < m_maxAttempts; i++)
	{
		double poissonRadius = m_radiusField.getRadius(*point);
		double radiusFactor = 1.5;
		double r = Random::get(poissonRadius, radiusFactor * poissonRadius);
		double phi = Random::get(0.0, 2.0 * pi());
		int gridSearchRange = static_cast<int>(std::ceil(radiusFactor * poissonRadius / m_cellSize));
		Point candidatePoint{(*point)[0] + r * std::cos(phi), (*point)[1] + r * std::sin(phi)};
		int I = std::floor((candidatePoint[0]  - m_box.xMin) / m_cellSize);
		int J = std::floor((candidatePoint[1] - m_box.yMin) / m_cellSize);

		bool minDistAchived = true;
		for (int i = std::max(I - gridSearchRange, 0); i <= std::min(I + gridSearchRange, static_cast<int>(m_grid.size()) - 1); i++)
		{
			for (int j = std::max(J - gridSearchRange, 0); j <= std::min(J + gridSearchRange, static_cast<int>(m_grid[0].size()) - 1); j++)
			{
				if(m_grid[i][j])
					if (dist(candidatePoint, *(m_grid[i][j])) < poissonRadius)
					{
						minDistAchived = false;
						break;
					}
			}
			if (minDistAchived == false)
				break;
		}
		bool inDomain = m_boundaries.pointInBoundaries(candidatePoint);
		if (inDomain && minDistAchived)
		{
			newPoint = std::make_unique<Point>(candidatePoint);
			break;
		}
	}
	m_activeCells.erase(activeCellIterator);
	if (newPoint)
	{
		points.pushBack(*newPoint);
		size_t i = std::floor(((*newPoint)[0] - m_box.xMin) / m_cellSize);
		size_t j = std::floor(((*newPoint)[1] - m_box.yMin) / m_cellSize);
		m_grid[i][j] = std::move(newPoint);
		m_activeCells.pushBack(Cell(i, j));
	}
}


inline bool BridsonGrid::filled() const
{
	return m_activeCells.empty();
}
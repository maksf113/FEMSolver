#pragma once
#include <memory>

#include "Point.hpp"
#include "Boundaries.hpp"
#include "BridsonGrid.hpp"
#include "Triangulation.hpp"

class Domain
{
public:
	Domain();
	~Domain() = default;
	Domain(const Domain&) = delete;
	Domain(Domain&&) = delete;
	Domain& operator=(const Domain&) = delete;
	Domain& operator=(Domain&&) = delete;
	const Array<Point>& getInnerPoints() const;
	const Boundaries& getBoundaries() const;
	const Array<Point>& getTriangles() const;
	bool pointInDomain(const Point& p) const;
	const Triangulation& getTriangulation() const;
private:
	
private:
	Boundaries m_boundaries;
	Array<Point> m_innerPoints;
	std::unique_ptr<Triangulation> m_triangulation;
};

Domain::Domain() : m_boundaries()
{
	BridsonGrid grid(m_boundaries);
	grid.generateInnerPoints(m_innerPoints);
	m_triangulation =  std::make_unique<Triangulation>(m_boundaries, m_innerPoints);
}

inline const Array<Point>& Domain::getInnerPoints() const
{
	return m_innerPoints;
}

inline const Boundaries& Domain::getBoundaries() const
{
	return m_boundaries;
}

inline const Array<Point>& Domain::getTriangles() const
{
	return m_triangulation->getTrianglePoints();
}

inline bool Domain::pointInDomain(const Point& p) const
{
	if (!pointInPolygon(p, m_boundaries.getOuterBoundary()))
		return false;
	for (const auto& innerBoundary : m_boundaries.getInnerBoundaries())
		if (pointInPolygon(p, innerBoundary))
			return false;
	return true;
}

inline const Triangulation& Domain::getTriangulation() const
{
	return *m_triangulation;
}


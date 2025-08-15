#pragma once
#include <algorithm>

#include "data_structures/Array.hpp"
#include "math/Vector.hpp"


using Point = Vector<double, 2>;

class BoundaryPoint : public Point
{
private:
	double m_dist; // mean distance from its neighbours
public:
	BoundaryPoint(const Point& p = { 0 }, double dist = 0.0) : Point(p), m_dist(dist) {}
	double meanDist() { return m_dist; }
};

double distSquared(const Point& p, const Point& q)
{
	return normSquared(p - q);
}

double dist(const Point& p, const Point& q)
{
	return norm(p - q);
}

bool pointInPolygon(const Point& p, const Array<Point>& polygon)
{
	// raycasting point in polygon test (ray in positive x direction)
	size_t intersections = 0;
	size_t n = polygon.size();
	for (size_t i = 0; i < n; i++)
	{
		const Point& a = polygon[i];
		const Point& b = polygon[(i + 1) % n]; 
		Point low(a);
		Point high(b);
		if (low[1] > high[1])
			std::swap(low, high);
		// is point y between vertices y's
		if (p[1] <= low[1] || p[1] > high[1])
			continue;
		double xIntersect = low[0] + (p[1] - low[1]) * (high[0] - low[0]) / (high[1] - low[1]);
		if (xIntersect > p[0])
			++intersections;
	}
	return (intersections % 2) == 1;
}
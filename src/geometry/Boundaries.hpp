#pragma once
#include "Point.hpp"
#include "tools/Random.hpp"
#include "data_structures/Array.hpp"
#include "data_structures/List.hpp"

struct AABB
{
	double xMin, xMax, yMin, yMax;
};
double pi() { return 4.0 * atan(1.0); }
class Boundaries
{
public:
	Boundaries();
	~Boundaries() = default;
    Boundaries(const Boundaries&) = delete;
    Boundaries(Boundaries&&) = delete;
    Boundaries& operator=(const Boundaries&) = delete;
    Boundaries& operator=(Boundaries&&) = delete;
	const Array<Point>& getOuterBoundary() const;
	const Array<Array<Point>>& getInnerBoundaries() const;
    AABB getBoundingBox() const;
    double minDist() const;
    bool pointInBoundaries(const Point& p) const;
private:
	Array<Point> m_outer;
	Array<Array<Point>> m_inner;
    AABB m_boundingBox;
    double m_minDist;
};

Boundaries::Boundaries()
{
    //// --- CASE 0 : CIRCLE WITH CIRCULAR AND DISTORTED CIRCULAR HOLE ---
    //// -- Geometry Definition
    //size_t n_outer = 50;
    //size_t n_inner1 = 70;
    //size_t n_inner2 = 100;
    //m_outer.resize(n_outer);
    //m_inner.pushBack(Array<Point>(n_inner1));
    //m_inner.pushBack(Array<Point>(n_inner2));
    //double R = 1.0;
    //double r = 0.3;
    //for (int i = 0; i < m_outer.size(); i++)
    //{
    //    m_outer[i][0] = R * cos(2.0 * pi() / n_outer * i);
    //    m_outer[i][1] = R * sin(2.0 * pi() / n_outer * i);
    //}
    //for (int i = 0; i < m_inner[0].size(); i++)
    //{
    //    m_inner[0][i][0] = 0.8 * r * cos(2.0 * pi() / n_inner1 * i) - 0.5;
    //    m_inner[0][i][1] = r * sin(2.0 * pi() / n_inner1 * i);
    //}
    //for (int i = 0; i < m_inner[1].size(); i++)
    //{
    //    m_inner[1][i][0] = 0.7 * r * cos(2.0 * pi() / n_inner2 * i) + 0.010 * sin(2.0 * pi() / n_inner2 * i * 5.0) + 0.005 * cos(2.0 * pi() / n_inner2 * i * 8.0) + 0.5;
    //    m_inner[1][i][1] = 0.7 * r * sin(2.0 * pi() / n_inner2 * i) + 0.004 * sin(2.0 * pi() / n_inner2 * i * 10.0) + 0.0012 * cos(2.0 * pi() / n_inner2 * i * 15.0) + 0.2;
    //}

    //// --- CASE 1: SQUARE WITH CIRCULAR HOLE ---
    //// -- Geometry Definition
    //size_t n_outer_side = 50; // Points per side of the square
    //size_t n_outer = n_outer_side * 4;
    //size_t n_inner1 = 80;

    //m_outer.resize(n_outer);
    //m_inner.pushBack(Array<Point>(n_inner1));

    //// Define the outer square boundary from (-1,-1) to (1,1)
    //double L = 1.0; // Half-length of the square's side
    //for (size_t i = 0; i < n_outer_side; ++i) {
    //    m_outer[i + n_outer_side * 0][0] = -L + 2.0 * L * i / n_outer_side; m_outer[i + n_outer_side * 0][1] = -L;
    //    m_outer[i + n_outer_side * 1][0] = L; m_outer[i + n_outer_side * 1][1] = -L + 2.0 * L * i / n_outer_side;
    //    m_outer[i + n_outer_side * 2][0] = L - 2.0 * L * i / n_outer_side; m_outer[i + n_outer_side * 2][1] = L;
    //    m_outer[i + n_outer_side * 3][0] = -L; m_outer[i + n_outer_side * 3][1] = L - 2.0 * L * i / n_outer_side;
    //}

    //// Define the inner circular hole
    //double r = 0.4; // Radius of the inner circle
    //for (size_t i = 0; i < n_inner1; ++i) {
    //    m_inner[0][i][0] = r * cos(2.0 * pi() / n_inner1 * i);
    //    m_inner[0][i][1] = r * sin(2.0 * pi() / n_inner1 * i);
    //}

    // --- CASE 2: RECTANGLE WITH FOUR SQUARE HOLES ---
    // -- Geometry Definition
    size_t n_outer_horiz = 100; // Points on horizontal sides
    size_t n_outer_vert = 50;  // Points on vertical sides
    size_t n_outer = 2 * n_outer_horiz + 2 * n_outer_vert;
    size_t n_inner_side = 30; // Points per side for inner squares
    size_t n_inner_total = n_inner_side * 4;

    m_outer.resize(n_outer);
    m_inner.pushBack(Array<Point>(n_inner_total)); // Hole 1
    m_inner.pushBack(Array<Point>(n_inner_total)); // Hole 2
    m_inner.pushBack(Array<Point>(n_inner_total)); // Hole 3
    m_inner.pushBack(Array<Point>(n_inner_total)); // Hole 4

    // Define the outer rectangle from (-2,-1) to (2,1)
    double Rx = 2.0, Ry = 1.0;
    size_t current_pos = 0;
    for (size_t i = 0; i < n_outer_horiz; ++i) { m_outer[current_pos + i][0] = -Rx + 2 * Rx * i / n_outer_horiz; m_outer[current_pos + i][1] = -Ry; } current_pos += n_outer_horiz;
    for (size_t i = 0; i < n_outer_vert; ++i) { m_outer[current_pos + i][0] = Rx; m_outer[current_pos + i][1] = -Ry + 2 * Ry * i / n_outer_vert; } current_pos += n_outer_vert;
    for (size_t i = 0; i < n_outer_horiz; ++i) { m_outer[current_pos + i][0] = Rx - 2 * Rx * i / n_outer_horiz; m_outer[current_pos + i][1] = Ry; } current_pos += n_outer_horiz;
    for (size_t i = 0; i < n_outer_vert; ++i) { m_outer[current_pos + i][0] = -Rx; m_outer[current_pos + i][1] = Ry - 2 * Ry * i / n_outer_vert; }

    // Helper function to create a square boundary
    auto createSquare = [&](Array<Point>& boundary, double center_x, double center_y, double side) {
        double L = side / 2.0;
        for (size_t i = 0; i < n_inner_side; ++i) {
            boundary[i + n_inner_side * 0][0] = center_x - L + side * i / n_inner_side; boundary[i + n_inner_side * 0][1] = center_y - L;
            boundary[i + n_inner_side * 1][0] = center_x + L; boundary[i + n_inner_side * 1][1] = center_y - L + side * i / n_inner_side;
            boundary[i + n_inner_side * 2][0] = center_x + L - side * i / n_inner_side; boundary[i + n_inner_side * 2][1] = center_y + L;
            boundary[i + n_inner_side * 3][0] = center_x - L; boundary[i + n_inner_side * 3][1] = center_y + L - side * i / n_inner_side;
        }
        };

    // Create the four inner square holes
    createSquare(m_inner[0], -1.0, 0.5, 0.5);
    createSquare(m_inner[1], 1.0, 0.5, 0.5);
    createSquare(m_inner[2], -1.0, -0.5, 0.5);
    createSquare(m_inner[3], 1.0, -0.5, 0.5);

    //// --- CASE 3: FLOWER SHAPE WITH OFFSET HOLES ---
    //// -- Geometry Definition
    //size_t n_outer = 250;
    //size_t n_inner1 = 80;
    //size_t n_inner2 = 70;

    //m_outer.resize(n_outer);
    //m_inner.pushBack(Array<Point>(n_inner1));
    //m_inner.pushBack(Array<Point>(n_inner2));

    //// Define the outer "flower" or "gear" shape boundary
    //double R_base = 1.0;
    //double R_amp = 0.15;
    //int n_petals = 7;
    //for (int i = 0; i < m_outer.size(); i++)
    //{
    //    double angle = 2.0 * pi() / n_outer * i;
    //    double R = R_base + R_amp * cos(n_petals * angle);
    //    m_outer[i][0] = R * cos(angle);
    //    m_outer[i][1] = R * sin(angle);
    //}

    //// Define first inner hole (offset)
    //double r1 = 0.2;
    //double c1_x = -0.4, c1_y = 0.5;
    //for (int i = 0; i < m_inner[0].size(); i++)
    //{
    //    m_inner[0][i][0] = c1_x + r1 * cos(2.0 * pi() / n_inner1 * i);
    //    m_inner[0][i][1] = c1_y + r1 * sin(2.0 * pi() / n_inner1 * i);
    //}

    //// Define second inner hole (offset)
    //double r2 = 0.25;
    //double c2_x = 0.5, c2_y = -0.3;
    //for (int i = 0; i < m_inner[1].size(); i++)
    //{
    //    m_inner[1][i][0] = c2_x + r2 * cos(2.0 * pi() / n_inner2 * i);
    //    m_inner[1][i][1] = c2_y + r2 * sin(2.0 * pi() / n_inner2 * i);
    //}

    
    //  set axis aligned bounding box
    m_boundingBox.xMin = m_outer[0][0];
    m_boundingBox.xMax = m_outer[0][0];
    m_boundingBox.yMin = m_outer[0][1];
    m_boundingBox.yMax = m_outer[0][1];
    for (const auto& p : m_outer)
    {
        if (p[0] < m_boundingBox.xMin)
            m_boundingBox.xMin = p[0];
        if (p[0] > m_boundingBox.xMax)
            m_boundingBox.xMax = p[0];
        if (p[1] < m_boundingBox.yMin)
            m_boundingBox.yMin = p[1];
        if (p[1] > m_boundingBox.yMax)
            m_boundingBox.yMax = p[1];
    }
    // minimal distance between boudary points
    m_minDist = dist(m_outer[0], m_outer[1]);
    for (size_t i = 0; i < m_outer.size(); i++)
    {
        size_t iPlus1 = (i + 1) % m_outer.size();
        double distance = dist(m_outer[i], m_outer[iPlus1]);
        if (distance < m_minDist)
            m_minDist = distance;
    }
    for(const auto& inner : m_inner)
        for (size_t i = 0; i < inner.size(); i++)
        {
            size_t iPlus1 = (i + 1) % inner.size();
            double distance = dist(inner[i], inner[iPlus1]);
            if (distance < m_minDist)
                m_minDist = distance;
        }
}

inline const Array<Point>& Boundaries::getOuterBoundary() const
{
    return m_outer;
}

inline const Array<Array<Point>>& Boundaries::getInnerBoundaries() const
{
    return m_inner;
}

inline AABB Boundaries::getBoundingBox() const
{
    return m_boundingBox;
}

inline double Boundaries::minDist() const
{
    return m_minDist;
}

inline bool Boundaries::pointInBoundaries(const Point& p) const
{
    if (!pointInPolygon(p, m_outer))
        return false;
    for (const auto& inner : m_inner)
        if (pointInPolygon(p, inner))
            return false;
    return true;
}

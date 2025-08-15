#pragma once
#include "data_structures/Array.hpp"
#include "geometry/Point.hpp"

class Node
{
private:
	Point m_position;
	int m_boundaryId;
public:
	Node() = default;
	Node(const Point& point, int boundaryId);
	Node(const Node& other);
	Node(Node&& other);
	~Node() = default;
	Node& operator=(const Node& other);
	Node& operator=(Node&& other);
	const Point& position() const;
	int boundaryId() const;
};

inline Node::Node(const Point& point, int boundaryId) :
	m_position(point), m_boundaryId(boundaryId) {}

inline Node::Node(const Node& other) :
	m_position(other.m_position), m_boundaryId(other.m_boundaryId) {}

inline Node::Node(Node&& other) :
	m_position(std::move(other.m_position)), m_boundaryId(std::move(other.m_boundaryId)) {}

inline Node& Node::operator=(const Node& other)
{
	m_position = other.m_position;
	m_boundaryId = other.m_boundaryId;
	return *this;
}

inline Node& Node::operator=(Node&& other)
{
	std::swap(m_position, other.m_position);
	std::swap(m_boundaryId, other.m_boundaryId);
	return *this;
}

inline const Point& Node::position() const
{
	return m_position;
}

inline int Node::boundaryId() const
{
	return m_boundaryId;
}

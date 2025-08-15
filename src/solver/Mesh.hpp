#pragma once
#include "data_structures/Array.hpp"
#include "geometry/Point.hpp"
#include "geometry/Triangulation.hpp"
#include "Node.hpp"
#include "FiniteElement.hpp"
#include "ReferenceElement.hpp"


template<typename T, int N_NODES>
class Mesh
{

private:
	Array<Node> m_nodes;
	Array<FiniteElement<N_NODES>> m_elements;
	ReferenceElement<T, N_NODES> m_referenceElement;
public:
	Mesh(const Triangulation& triangulation);
	~Mesh() = default;
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
	using ConstIterator = typename Array<FiniteElement<N_NODES>>::ConstIterator;
	ConstIterator begin() const;
	ConstIterator end() const;
	size_t elementCount() const;
	size_t nodeCount() const;
	const Node& node(size_t i) const;
	const FiniteElement<N_NODES>& element(size_t i) const;
	const ReferenceElement<T, N_NODES>& referenceElement() const;
};

template<typename T, int N_NODES>
inline Mesh<T, N_NODES>::Mesh(const Triangulation& triangulation)
{
	for (size_t i = 0; i < triangulation.vertexCount(); i++)
	{
		Point p = triangulation.getVertexPoint(i);
		int boundaryId = triangulation.getVertexBoundaryId(i);
		m_nodes.pushBack({ p, boundaryId });
	}
	for (size_t i = 0; i < triangulation.getTriangleCount(); i++)
	{
		m_elements.pushBack(triangulation.getTriangleVertexIndices(i));
		m_elements.back().setMaterial(0);
	}
}

template<typename T, int N_NODES>
inline typename Mesh<T, N_NODES>::ConstIterator Mesh<T, N_NODES>::begin() const
{
	return m_elements.cbegin();
}

template<typename T, int N_NODES>
inline typename Mesh<T, N_NODES>::ConstIterator Mesh<T, N_NODES>::end() const
{
	return m_elements.cend();
}

template<typename T, int N_NODES>
inline size_t Mesh<T, N_NODES>::elementCount() const
{
	return m_elements.size();
}

template<typename T, int N_NODES>
inline size_t Mesh<T, N_NODES>::nodeCount() const
{
	return m_nodes.size();
}

template<typename T, int N_NODES>
inline const Node& Mesh<T, N_NODES>::node(size_t i) const
{
	return m_nodes[i];
}

template<typename T, int N_NODES>
inline const FiniteElement<N_NODES>& Mesh<T, N_NODES>::element(size_t i) const
{
	return m_elements[i];
}

template<typename T, int N_NODES>
inline const ReferenceElement<T, N_NODES>& Mesh<T, N_NODES>::referenceElement() const
{
	return m_referenceElement;
}

 
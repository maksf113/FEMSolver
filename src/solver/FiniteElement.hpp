#pragma once
#include <iostream>
#include <utility>
#include "data_structures/StaticArray.hpp"

template<int N_NODES>
class FiniteElement
{
private:
	StaticArray<size_t, static_cast<size_t>(N_NODES)> m_nodes;
	int m_material;
public:
	FiniteElement() = default;
	FiniteElement(const StaticArray<size_t, N_NODES>& nodes);
	FiniteElement(const FiniteElement<N_NODES>& other);
	FiniteElement(FiniteElement<N_NODES>&& other);
	~FiniteElement() = default;
	FiniteElement& operator=(const FiniteElement<N_NODES>& other);
	FiniteElement& operator=(FiniteElement<N_NODES>&& other);
	void setMaterial(int i);
	int materialIdx() const;
	size_t nodeIdx(size_t i) const;
};

template<int N_NODES>
inline FiniteElement<N_NODES>::FiniteElement(const StaticArray<size_t, N_NODES>& nodes)
{
	for (int i = 0; i < N_NODES; i++)
		m_nodes[i] = nodes[i];
}

template<int N_NODES>
inline FiniteElement<N_NODES>::FiniteElement(const FiniteElement<N_NODES>& other) : 
	m_nodes(other.m_nodes), m_material(other.m_material) {}

template<int N_NODES>
inline FiniteElement<N_NODES>::FiniteElement(FiniteElement<N_NODES>&& other) :
	m_nodes(std::move(other.m_nodes)), m_material(std::move(other.m_material)) {}

template<int N_NODES>
inline FiniteElement<N_NODES>& FiniteElement<N_NODES>::operator=(const FiniteElement& other)
{
	if(this != &other)
	{
		m_nodes = other.m_nodes;
		m_material = other.m_material;
	}
	return *this;
}

template<int N_NODES>
inline FiniteElement<N_NODES>& FiniteElement<N_NODES>::operator=(FiniteElement<N_NODES>&& other)
{
	if(this != &other)
	{
		m_nodes = std::move(other.m_nodes);
		m_material = std::move(other.m_material);
	}
	return *this;
}

template<int N_NODES>
inline void FiniteElement<N_NODES>::setMaterial(int i)
{
	m_material = i;
}

template<int N_NODES>
inline int FiniteElement<N_NODES>::materialIdx() const
{
	return m_material;
}

template<int N_NODES>
inline size_t FiniteElement<N_NODES>::nodeIdx(size_t i) const
{
	assert(i >= 0 && i < N_NODES);
	return m_nodes[i];
}

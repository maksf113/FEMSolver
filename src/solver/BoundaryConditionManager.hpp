#pragma once
#include <functional>
#include <utility>
#include "data_structures/Array.hpp"
#include "geometry/Point.hpp"

template<typename T>
struct BoundaryCondition
{
	std::function<T(const Point&)> getValue;
};

template<typename T>
class BoundaryConditionManager
{
private:
	Array<BoundaryCondition<T>> m_BCs;
public:
	void addBC(const BoundaryCondition<T>& bc);
	void addBC(BoundaryCondition<T>&& bc);
	const BoundaryCondition<T>& getBC(size_t i) const;
};

template<typename T>
void BoundaryConditionManager<T>::addBC(const BoundaryCondition<T>& bc)
{
	m_BCs.pushBack(bc);
}

template<typename T>
inline void BoundaryConditionManager<T>::addBC(BoundaryCondition<T>&& bc)
{
	m_BCs.pushBack(std::move(bc));
}

template<typename T>
inline const BoundaryCondition<T>& BoundaryConditionManager<T>::getBC(size_t i) const
{
	return m_BCs[i];
}

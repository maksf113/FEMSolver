#pragma once
#include <utility>
#include <cmath>
#include "data_structures/List.hpp"
#include "RowElement.hpp"
#include "Vector.hpp"

namespace sparse
{
template<typename T>
class Row
{
private:
	List<RowElement<T>> m_elements;
public:
	Row() = default;
	Row(const Row<T>& other);
	Row(Row<T>&& r) noexcept;
	Row<T>& operator=(const Row<T>& other);
	Row<T>& operator=(Row<T>&& other) noexcept;
	~Row() = default;
	using ConstIterator = typename List<RowElement<T>>::ConstIterator;
	ConstIterator begin() const;
	ConstIterator end() const;
	size_t dim() const;
	const T& operator[](size_t i) const; // read only version
	const T& operator()(size_t i) const; // read only version
	void set(const RowElement<T>& e);
	void set(RowElement<T>&& e);
	void insert(const RowElement<T>& e);
	void insert(RowElement<T>&& e);
	void clear();
	Row<T>& operator+=(const Row<T>& other);
	Row<T>& operator-=(const Row<T>& other);
	Row<T>& operator*=(const T& t);
	Row<T>& operator/=(const T& t);
};

template<typename T>
inline Row<T>::Row(const Row<T>& other) : m_elements(other.m_elements) {}

template<typename T>
inline Row<T>::Row(Row<T>&& other) noexcept : m_elements(std::move(other.m_elements)) {}

template<typename T>
inline Row<T>& Row<T>::operator=(const Row<T>& other)
{
	if (this != &other)
	{
		m_elements = other.m_elements;
	}
	return *this;
}

template<typename T>
inline Row<T>& Row<T>::operator=(Row<T>&& other) noexcept
{
	if (this != &other)
	{
		m_elements = std::move(other.m_elements);
	}
	return *this;
}

template<typename T>
inline typename Row<T>::ConstIterator Row<T>::begin() const
{
	return m_elements.cbegin();
}

template<typename T>
inline typename Row<T>::ConstIterator Row<T>::end() const
{
	return m_elements.cend();
}

template<typename T>
inline size_t Row<T>::dim() const
{
	return m_elements.size();
}

template<typename T>
inline const T& Row<T>::operator[](size_t i) const
{
	for (const auto& elem : m_elements)
	{
		if (elem.col() == i)
			return elem();
	}
	static constexpr T zero{};
	return zero;
}

template<typename T>
inline const T& Row<T>::operator()(size_t i) const
{
	for (const auto& elem : m_elements)
	{
		if (elem.col() == i)
			return elem();
	}
	static constexpr T zero{};
	return zero;
}

template<typename T>
inline void Row<T>::set(const RowElement<T>& e)
{
	for (List<RowElement<T>>::Iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (it->col() == e.col())
		{
			if (e.val() != T{})
			{
				*it = e;
			}
			else
			{
				m_elements.erase(it);
			}
			return;
		}
		if (it->col() > e.col())
		{
			if (e.val() != T{})
			{
				m_elements.insert(it, e);
			}
			return;
		}
	}
	if (e.val() != T{})
	{
		m_elements.pushBack(e);
	}
}

template<typename T>
inline void Row<T>::set(RowElement<T>&& e)
{
	for (List<RowElement<T>>::Iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (it->col() == e.col())
		{
			if (e.val() != T{})
			{
				*it = std::move(e);
			}
			else
			{
				m_elements.erase(it);
			}
			return;
		}
		if (it->col() > e.col())
		{
			if (e.val() != T{})
			{
				m_elements.insert(it, std::move(e));
			}
			return;
		}
	}
	if (e.val() != T{})
	{
		m_elements.pushBack(std::move(e));
	}
}

template<typename T>
inline void Row<T>::insert(const RowElement<T>& e)
{
	if (e.val() == T{})
		return;
	for (List<RowElement<T>>::Iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (it->col() == e.col())
		{
			*it += e;
			if (it->val() == T{})
				m_elements.erase(it);
			return;
		}
		if (it->col() > e.col())
		{
			m_elements.insert(it, e);
			return;
		}
	}
	m_elements.pushBack(e);
}

template<typename T>
inline void Row<T>::insert(RowElement<T>&& e)
{
	if (e.val() == T{})
		return;
	for (auto it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (it->col() == e.col())
		{
			*it += e;
			if (it->val() == T{})
				m_elements.erase(it);
			return;
		}
		if (it->col() > e.col())
		{
			m_elements.insert(it, std::move(e));
			return;
		}
	}
	m_elements.pushBack(std::move(e));
}

template<typename T>
inline Row<T>& Row<T>::operator+=(const Row<T>& other)
{
	auto thisIt = this->m_elements.begin();
	auto otherIt = other.m_elements.cbegin();
	while (thisIt != m_elements.end() && otherIt != other.m_elements.cend())
	{
		if (thisIt->col() < otherIt->col())
		{
			++thisIt;
		}
		else if (thisIt->col() > otherIt->col())
		{
			m_elements.insert(thisIt, *otherIt);
			++otherIt;
		}
		else // equal columns
		{
			*thisIt += *otherIt;
			if (thisIt->val() == T{})
			{
				thisIt = m_elements.erase(thisIt);
			}
			else
			{
				++thisIt;
			}
			++otherIt;
		}
	}
	// remaining elements of other (of col greater than max col of this)
	while (otherIt != other.m_elements.cend())
	{
		m_elements.pushBack(*otherIt);
		++otherIt;
	}
	return *this;
}

template<typename T>
inline Row<T>& Row<T>::operator-=(const Row<T>& other)
{
	auto thisIt = this->m_elements.begin();
	auto otherIt = other.m_elements.cbegin();
	while (thisIt != m_elements.end() && otherIt != other.m_elements.cend())
	{
		if (thisIt->col() < otherIt->col())
		{
			++thisIt;
		}
		else if (thisIt->col() > otherIt->col())
		{
			m_elements.insert(thisIt, -(*otherIt));
			++otherIt;
		}
		else // equal columns
		{
			*thisIt -= *otherIt;
			if (thisIt->val() == T{})
			{
				thisIt = m_elements.erase(thisIt);
			}
			else
			{
				++thisIt;
			}
			++otherIt;
		}
	}
	// remaining elements of other (of col greater than max col of this)
	while (otherIt != other.m_elements.cend())
	{
		m_elements.pushBack(-(*otherIt));
		++otherIt;
	}
	return *this;
}

template<typename T>
inline Row<T>& Row<T>::operator*=(const T& t)
{
	for (auto& elem : m_elements)
		elem *= t;
	return *this;
}

template<typename T>
inline Row<T>& Row<T>::operator/=(const T& t)
{
	for (auto& elem : m_elements)
		elem /= t;
	return *this;
}
// non-member operators/functions
template<typename T>
inline T operator*(const Row<T>& r, const Vector<T>& v)
{
	T result{};
	for (const auto& elem : r)
	{
		result += elem.val() * v[elem.col()];
	}
	return result;
}

template<typename T>
inline T normSq(const Row<T>& r)
{
	T result{};
	for (const auto& elem : r)
	{
		result += elem.val() * elem.val();
	}
	return result;
}
template<typename T>
inline T norm(const Row<T>& r)
{
	return std::sqrt(normSq(r));
}
template<typename T>
inline void Row<T>::clear()
{
	m_elements.clear();
}
}
#pragma once
#include <limits>
#include <cassert>
#include <utility>
#include <cstddef>

namespace sparse
{
template<typename T>
class RowElement
{
private:
	T m_val;
	size_t m_col;
	static constexpr size_t INVALID_COL = std::numeric_limits<size_t>::max();
public:
	RowElement();
	RowElement(const T& val, size_t col);
	RowElement(const RowElement& other);
	RowElement(RowElement&& other) noexcept;
	~RowElement() = default;
	RowElement<T>& operator=(const RowElement<T>& other);
	RowElement<T>& operator=(RowElement<T>&& other) noexcept;
	RowElement<T>& operator+=(const T& t);
	RowElement<T>& operator+=(const RowElement<T>& other);
	RowElement<T>& operator-=(const T& t);
	RowElement<T>& operator-=(const RowElement<T>& other);
	RowElement<T>& operator*=(const T& t);
	RowElement<T>& operator/=(const T& t);
	const T& val() const;
	size_t col() const;
	T& operator()();
	const T& operator()() const;
};

template<typename T>
inline RowElement<T>::RowElement() : 
	m_val(T{}), m_col(RowElement::INVALID_COL) {}

template<typename T>
inline RowElement<T>::RowElement(const T& val, size_t col) : 
	m_val(val), m_col(col) {}

template<typename T>
inline RowElement<T>::RowElement(const RowElement& other) : 
	m_val(other.m_val), m_col(other.m_col) {}

template<typename T>
inline RowElement<T>::RowElement(RowElement&& other) noexcept : RowElement<T>()
{
	std::swap(m_val, other.m_val);
	std::swap(m_col, other.m_col);
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator=(const RowElement<T>& other)
{
	if (this != &other)
	{
		m_val = other.m_val;
		m_col = other.m_col;
	}
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator=(RowElement<T>&& other) noexcept
{
	if (this != &other)
	{
		std::swap(m_val, other.m_val);
		std::swap(m_col, other.m_col);
	}
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator+=(const T& t)
{
	m_val += t;
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator+=(const RowElement<T>& other)
{
	assert(m_col == other.m_col);
	m_val += other.m_val;
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator-=(const T& t)
{
	m_val -= t;
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator-=(const RowElement<T>& other)
{
	assert(m_col == other.m_col);
	m_val -= other.m_val;
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator*=(const T& t)
{
	m_val *= t;
	return *this;
}

template<typename T>
inline RowElement<T>& RowElement<T>::operator/=(const T& t)
{
	m_val /= t;
	return *this;
}

template<typename T>
inline const T& RowElement<T>::val() const
{
	return m_val;
}

template<typename T>
inline size_t RowElement<T>::col() const
{
	return m_col;
}

template<typename T>
inline T& RowElement<T>::operator()()
{
	return m_val;
}

template<typename T>
inline const T& RowElement<T>::operator()() const
{
	return m_val;
}

// binary non-member operators

template<typename T>
inline RowElement<T> operator+(const RowElement<T>& e1, const RowElement<T>& e2)
{
	RowElement<T> result(e1);
	return result += e2;
}

template<typename T>
inline RowElement<T> operator-(const RowElement<T>& e1, const RowElement<T>& e2)
{
	RowElement<T> result(e1);
	return result -= e2;
}

template<typename T>
inline RowElement<T> operator*(const RowElement<T>& e, const T& t)
{
	RowElement<T> result(e);
	return result *= t;
}            

template<typename T>
inline RowElement<T> operator*(const T& t, const RowElement<T>& e)
{
	RowElement<T> result(e);
	return result *= t;
}

template<typename T>
inline RowElement<T> operator/(const RowElement<T>& e, const T& t)
{
	RowElement<T> result(e);
	return result /= t;
}

// non-member unary operators

template<typename T>
inline RowElement<T> operator-(const RowElement<T>& r)
{
	return RowElement<T>(-r.val(), r.col());
}
}
#pragma once
#include <cmath>
#include "data_structures/Array.hpp"

namespace sparse
{
template<typename T>
class Vector
{
private:
	Array<T> m_components;
public:
	Vector() = default;
	Vector(size_t n);
	Vector(size_t n, const T& t);
	Vector(const Vector<T>& other);
	Vector(Vector<T>&& other);
	~Vector() = default;

	Vector<T>& operator=(const Vector<T>& other);
	Vector<T>& operator=(Vector<T>&& other);

	const T& operator[](size_t i) const;
	T& operator[](size_t i);

	size_t dim() const;
	void resize(size_t n);

	Vector<T>& operator+=(const Vector<T>& other);
	Vector<T>& operator-=(const Vector<T>& other);
	Vector<T>& operator*=(const T& t);
	Vector<T>& operator/=(const T& t);
};
template<typename T>
inline Vector<T>::Vector(size_t n) : m_components(n) {}

template<typename T>
inline Vector<T>::Vector(size_t n, const T& t) : m_components(n, t) {}

template<typename T>
inline Vector<T>::Vector(const Vector<T>& other) : m_components(other.m_components) {}

template<typename T>
inline Vector<T>::Vector(Vector<T>&& other) : m_components(std::move(other.m_components)) {}


template<typename T>
inline Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
	if(this != &other)
	{
		m_components = other.m_components;
	}
	return *this;
}

template<typename T>
inline Vector<T>& Vector<T>::operator=(Vector<T>&& other)
{
	if(this != &other)
	{
		m_components = std::move(other.m_components);
	}
	return *this;
}

template<typename T>
inline const T& Vector<T>::operator[](size_t i) const
{
	return m_components[i];
}

template<typename T>
inline T& Vector<T>::operator[](size_t i)
{
	return m_components[i];
}

template<typename T>
inline size_t Vector<T>::dim() const
{
	return m_components.size();
}

template<typename T>
inline void Vector<T>::resize(size_t n)
{
	m_components.resize(n);
}

template<typename T>
Vector<T>& Vector<T>::operator+=(const Vector<T>& other)
{
	for (size_t i = 0; i < m_components.size(); i++)
	{
		m_components[i] += other.m_components[i];
	}
	return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator-=(const Vector<T>& other)
{
	for (size_t i = 0; i < m_components.size(); i++)
	{
		m_components[i] -= other.m_components[i];
	}
	return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator*=(const T& t)
{
	for (size_t i = 0; i < m_components.size(); i++)
	{
		m_components[i] *= t;
	}
	return *this;
}
template<typename T>
Vector<T>& Vector<T>::operator/=(const T& t)
{
	for (size_t i = 0; i < m_components.size(); i++)
	{
		m_components[i] /= t;
	}
	return *this;
}

// nonmember operators/functions

template<typename T>
Vector<T> operator+(const Vector<T>& u, const Vector<T>& v)
{
	Vector<T> result(u);
	return result += v;
}

template<typename T>
Vector<T> operator-(const Vector<T>& u, const Vector<T>& v)
{
	Vector<T> result(u);
	return result -= v;
}

template<typename T>
Vector<T> operator-(const Vector<T>& v)
{
	Vector<T> result(v.dim());
	for (size_t i = 0; i < v.dim(); i++)
		result[i] = -v[i];
	return result;
}

template<typename T>
Vector<T> operator*(const Vector<T>& v, const T& t)
{
	Vector<T> result(v);
	return result *= t;
}

template<typename T>
Vector<T> operator*(const T& t, const Vector<T>& v)
{
	Vector<T> result(v);
	return result *= t;
}

template<typename T>
Vector<T> operator/(const Vector<T>& v, const T& t)
{
	Vector<T> result(v);
	return result /= t;
}

template<typename T>
T operator*(const Vector<T>& u, const Vector<T>& v)
{
	T result{};
	for (size_t i = 0; i < u.dim(); i++)
	{
		result += u[i] * v[i];
	}
	return result;
}

template<typename T>
T dot(const Vector<T>& u, const Vector<T>& v)
{
	return u * v;
}

template<typename T>
T normSq(const Vector<T>& v)
{
	return v * v;
}

template<typename T>
T norm(const Vector<T>& v)
{
	return std::sqrt(normSq(v));
}
}
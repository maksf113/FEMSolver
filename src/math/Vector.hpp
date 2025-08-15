#pragma once
#include <iostream>
#include <ostream>
#include <type_traits>

#include "data_structures/StaticArray.hpp"

template <typename T, size_t N>
class Vector : private StaticArray<T, N>
{
	//static_assert(std::is_arithmetic<T>::value && std::is_default_constructible<T>::value, "Vector<T, size_t> required T to be arithmetic and default constructable");
public:
	using StaticArray<T, N>::StaticArray;
	using StaticArray<T, N>::operator=;
	using StaticArray<T, N>::operator[];
	using StaticArray<T, N>::size;
	using StaticArray<T, N>::empty;
	using StaticArray<T, N>::begin;
	using StaticArray<T, N>::cbegin;
	using StaticArray<T, N>::end;
	using StaticArray<T, N>::cend;
	using StaticArray<T, N>::data;

	Vector& operator+=(const Vector& v) noexcept;
	Vector& operator-=(const Vector& v) noexcept;
	Vector& operator*=(const T& t) noexcept;
	Vector& operator/=(const T& t);

	Vector& normalize();
};

template<typename T, size_t N>
inline Vector<T, N>& Vector<T, N>::operator+=(const Vector& v) noexcept
{
	for (size_t i = 0; i < N; i++)
		(*this)[i] += v[i];
	return *this;
}
template<typename T, size_t N>
inline Vector<T, N>& Vector<T, N>::operator-=(const Vector& v) noexcept
{
	for (size_t i = 0; i < N; i++)
		(*this)[i] -= v[i];
	return *this;
}

template<typename T, size_t N>
inline Vector<T, N>& Vector<T, N>::operator*=(const T& t) noexcept
{
	for (size_t i = 0; i < N; i++)
		(*this)[i] *= t;
	return *this;
}

template<typename T, size_t N>
inline Vector<T, N>& Vector<T, N>::operator/=(const T& t)
{
	for (size_t i = 0; i < N; i++)
		(*this)[i] /= t;
	return *this;
}

template<typename T, size_t N>
inline Vector<T, N>& Vector<T, N>::normalize()
{
	*this /= norm(*this);
	return *this;
}

template<typename T, size_t N>
inline Vector<T, N> operator-(const Vector<T, N>& v)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = -v[i];
	return result;
}

template<typename T, size_t N>
inline Vector<T, N> operator+(const Vector<T, N>& u, const Vector<T, N>& v)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = u[i] + v[i];
	return result;
}

template<typename T, size_t N>
inline Vector<T, N> operator-(const Vector<T, N>& u, const Vector<T, N>& v)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = u[i] - v[i];
	return result;
}

template<typename T, size_t N>
inline Vector<T, N> operator*(const Vector<T, N>& v, const T& t)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = t * v[i];
	return result;
}

template<typename T, size_t N>
inline Vector<T, N> operator*(const T& t, const Vector<T, N>& v)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = t * v[i];
	return result;
}

template<typename T, size_t N>
inline Vector<T, N> operator/(const Vector<T, N>& v, const T& t)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = v[i] / t;
	return result;
}

template<typename T, size_t N>
inline T operator*(const Vector<T, N>& u, const Vector<T, N>& v)
{
	T result{};
	for (size_t i = 0; i < N; i++)
		result += u[i] * v[i];
	return result;
}

template<typename T, size_t N>
inline T dot(const Vector<T, N>& u, const Vector<T, N>& v)
{
	T result{};
	for (size_t i = 0; i < N; i++)
		result += u[i] * v[i];
	return result;
}

template<typename T>
inline T cross(const Vector<T, 2>& u, const Vector<T, 2>& v)
{
	return u[0] * v[1] - u[1] * v[0];
}

template<typename T>
inline Vector<T, 3> cross(const Vector<T, 3>& u, const Vector<T, 3>& v)
{
	Vector<T, 3> result;
	result[0] = u[1] * v[2] - u[2] * v[1];
	result[1] = u[2] * v[0] - u[0] * v[2];
	result[2] = u[0] * v[1] - u[1] * v[0];
	return result;
}

template<typename T, size_t N>
T normSquared(const Vector<T, N>& v)
{
	T result{};
	for (size_t i = 0; i < N; i++)
		result += v[i] * v[i];
	return result;

}

template<typename T, size_t N>
T norm(const Vector<T, N>& v)
{
	T result{};
	for (size_t i = 0; i < N; i++)
		result += v[i] * v[i];
	return std::sqrt(result);

}

template<size_t N>
void print(const Vector<double, N>& v)
{
	for (size_t i = 0; i < N; i++)
		printf("v[%d] = %lf\n", i, v[i]);
}

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& v)
{
	os << "v[";
	for (size_t i = 0; i < N; i++)
		os << v[i] << (i == N - 1 ? "" : ", ");
	os << "]";
	return os;
}
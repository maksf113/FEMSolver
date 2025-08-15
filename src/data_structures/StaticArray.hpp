#pragma once
#include <utility>
#include <algorithm>
#include <initializer_list>

template <typename T, size_t N>
class StaticArray
{
protected:
	T m_items[N > 0 ? N : 1];
public:
	StaticArray();
	StaticArray(const T& t);
	StaticArray(const StaticArray& other);
	StaticArray(StaticArray&& other) noexcept;
	StaticArray(std::initializer_list<T> initList);

	StaticArray& operator=(const StaticArray& other);
	StaticArray& operator=(StaticArray&& other) noexcept;

	T& operator[](size_t i);
	const T& operator[](size_t i) const;

	constexpr bool empty() const;
	constexpr size_t size() const;
	constexpr size_t capacity() const;

	const T& front() const;
	const T& back() const;

	typedef T* Iterator;
	typedef const T* ConstIterator;

	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;

	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;

	Iterator data();
	ConstIterator data() const;
};

template<typename T, size_t N>
inline StaticArray<T, N>::StaticArray()
{
	for (size_t i = 0; i < N; i++)
		m_items[i] = T{};
}

template<typename T, size_t N>
inline StaticArray<T, N>::StaticArray(const T& t)
{
	for (size_t i = 0; i < N; i++)
		m_items[i] = T(t);
}

template<typename T, size_t N>
inline StaticArray<T, N>::StaticArray(const StaticArray& other)
{
	for (size_t i = 0; i < N; i++)
		m_items[i] = other.m_items[i];
}

template<typename T, size_t N>
inline StaticArray<T, N>::StaticArray(StaticArray&& other) noexcept
{
	for (size_t i = 0; i < N; i++)
		m_items[i] = std::move(other.m_items[i]);
}

template<typename T, size_t N>
inline StaticArray<T, N>::StaticArray(std::initializer_list<T> initList)
{
	size_t i = 0;
	for (const T& item : initList)
		m_items[i++] = item;
}

template<typename T, size_t N>
inline StaticArray<T, N>& StaticArray<T, N>::operator=(const StaticArray& other)
{
	if (this == &other)
		return *this;
	for (size_t i = 0; i < N; i++)
		m_items[i] = other.m_items[i];
	return *this;
}

template<typename T, size_t N>
inline StaticArray<T, N>& StaticArray<T, N>::operator=(StaticArray&& other) noexcept
{
	if (this == &other)
		return *this;
	for (size_t i = 0; i < N; i++)
		m_items[i] = std::move(other.m_items[i]);
	return *this;
}

template<typename T, size_t N>
inline T& StaticArray<T, N>::operator[](size_t i)
{
	return m_items[i];
}
template<typename T, size_t N>
inline const T& StaticArray<T, N>::operator[](size_t i) const
{
	return m_items[i];
}

template<typename T, size_t N>
inline constexpr bool StaticArray<T, N>::empty() const
{
	return N == 0;
}

template<typename T, size_t N>
inline constexpr size_t StaticArray<T, N>::size() const
{
	return N;
}

template<typename T, size_t N>
inline constexpr size_t StaticArray<T, N>::capacity() const
{
	return N;
}

template<typename T, size_t N>
inline const T& StaticArray<T, N>::front() const
{
	return m_items[0];
}

template<typename T, size_t N>
inline const T& StaticArray<T, N>::back() const
{
	return m_items[N - 1];
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::Iterator StaticArray<T, N>::begin()
{
	return m_items;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::ConstIterator StaticArray<T, N>::begin() const
{
	return m_items;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::ConstIterator StaticArray<T, N>::cbegin() const
{
	return m_items;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::Iterator StaticArray<T, N>::end()
{
	return m_items + N;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::ConstIterator StaticArray<T, N>::end() const
{
	return m_items + N;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::ConstIterator StaticArray<T, N>::cend() const
{
	return m_items + N;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::Iterator StaticArray<T, N>::data()
{
	return m_items;
}

template<typename T, size_t N>
inline typename StaticArray<T, N>::ConstIterator StaticArray<T, N>::data() const
{
	return m_items;
}

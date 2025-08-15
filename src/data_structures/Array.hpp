#pragma once
#include <algorithm>
#include <utility>
#include <initializer_list>

template <typename T>
class Array
{
protected:
	size_t m_size;
	size_t m_capacity;
	T* m_items;
public:
	explicit Array(size_t size = 0);
	Array(size_t size, const T& t);
	Array(const Array& a);
	Array(Array&& a) noexcept;
	Array(std::initializer_list<T> initList);
	~Array();

	Array& operator=(const Array& a);
	Array& operator=(Array&& a) noexcept;

	void resize(size_t newSize);
	void reserve(size_t newCapacity);

	T& operator[](size_t i);
	const T& operator[](size_t i) const;

	bool empty() const;
	size_t size() const;
	size_t capacity() const;

	void pushBack(const T& t);
	void pushBack(T&& t);
	void popBack();
	const T& back() const;
	T& back();
	const T& front() const;
	T& front();

	void swap(Array&& other) noexcept;
	void shrinkToFit();
	void clear();

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

	static constexpr size_t SPARE_CAPACITY = 8;
};


template<typename T>
inline Array<T>::Array(size_t size)
	: m_size(size), m_capacity(size + SPARE_CAPACITY)
{
	m_items = new T[m_capacity];
	for (size_t i = 0; i < size; i++)
		m_items[i] = T();
}

template<typename T>
inline Array<T>::Array(size_t size, const T& t) : m_size(size), m_capacity(size + SPARE_CAPACITY)
{
	m_items = new T[m_capacity];
	for (size_t i = 0; i < size; i++)
		m_items[i] = T(t);
}

template<typename T>
inline Array<T>::Array(const Array& a)
	: m_size(a.m_size), m_capacity(a.m_capacity), m_items(nullptr)
{
	m_items = new T[m_capacity];
	for (size_t i = 0; i < m_size; ++i)
		m_items[i] = a.m_items[i];
}

template<typename T>
inline Array<T>::Array(Array<T>&& a) noexcept
	: m_size(a.m_size), m_capacity(a.m_capacity), m_items(a.m_items)
{
	a.m_items = nullptr;
	a.m_size = 0;
	a.m_capacity = 0;
}

template<typename T>
inline Array<T>::Array(std::initializer_list<T> initList) :
	m_size(initList.size()), m_capacity(initList.size() + SPARE_CAPACITY)
{
	m_items = new T[m_capacity];
	size_t i = 0;
	for (const T& item : initList)
	{
		m_items[i++] = item;
	}
}

template<typename T>
inline Array<T>::~Array()
{
	delete[] m_items;
}

template<typename T>
inline Array<T>& Array<T>::operator=(const Array<T>& a)
{
	Array<T> copy(a);
	std::swap(*this, copy);
	return *this;
}

template<typename T>
inline Array<T>& Array<T>::operator=(Array<T>&& a) noexcept
{
	if (this != &a)
	{
		std::swap(m_size, a.m_size);
		std::swap(m_capacity, a.m_capacity);
		std::swap(m_items, a.m_items);
	}
	return *this;
}

template<typename T>
inline void Array<T>::resize(size_t newSize)
{
	// if growing
	if (newSize > m_size)
	{
		if (newSize > m_capacity)
			reserve(newSize * 2);
		for (size_t i = m_size; i < newSize; ++i)
			m_items[i] = T();
	}
	m_size = newSize;
}

template<typename T>
inline void Array<T>::reserve(size_t newCapacity)
{
	if (newCapacity < m_size)
		return;
	T* newData = new T[newCapacity];
	for (size_t i = 0; i < m_size; ++i)
		newData[i] = std::move(m_items[i]);
	// free old memory
	delete[] m_items;
	m_items = newData;
	m_capacity = newCapacity;
}

template<typename T>
inline T& Array<T>::operator[](size_t i)
{
	return m_items[i];
}

template<typename T>
inline const T& Array<T>::operator[](size_t i) const
{
	return m_items[i];
}

template<typename T>
inline bool Array<T>::empty() const
{
	return m_size == static_cast<size_t>(0);
}

template<typename T>
inline size_t Array<T>::size() const
{
	return m_size;
}

template<typename T>
inline size_t Array<T>::capacity() const
{
	return m_capacity;
}

template<typename T>
inline void Array<T>::pushBack(const T& t)
{
	if (m_size == m_capacity)
		reserve(std::max(2 * m_capacity, static_cast<size_t>(1)));
	m_items[m_size++] = t;
}

template<typename T>
inline void Array<T>::pushBack(T&& t)
{
	if (m_size == m_capacity)
		reserve(std::max(2 * m_capacity, size_t(1)));
	m_items[m_size++] = std::move(t);
}

template<typename T>
inline void Array<T>::popBack()
{
	if (m_size > 0)
	{
		--m_size;
	}
}

template<typename T>
inline const T& Array<T>::back() const
{
	return m_items[m_size - 1];
}

template<typename T>
inline T& Array<T>::back()
{
	return m_items[m_size - 1];
}

template<typename T>
inline const T& Array<T>::front() const
{
	return m_items[0];
}

template<typename T>
inline T& Array<T>::front()
{
	return m_items[0];
}

template<typename T>
inline void Array<T>::swap(Array&& other) noexcept
{
	using std::swap;
	swap(m_size, other.m_size);
	swap(m_capacity, other.m_capacity);
	swap(m_items, other.m_items);
}

template<typename T>
inline void Array<T>::shrinkToFit()
{
	if (m_capacity == m_size)
		return;
	if (m_size == 0)
	{
		delete[] m_items;
		m_items = nullptr;
		m_capacity = 0;
		return;
	}
	T* newItems = new T[m_size];
	for (size_t i = 0; i < m_size; i++)
	{
		newItems[i] = std::move(m_items[i]);
	}
	delete[] m_items;
	m_items = newItems;
	m_capacity = m_size;
}

template<typename T>
inline void Array<T>::clear()
{
	m_size = 0;
}

template<typename T>
inline typename Array<T>::Iterator Array<T>::begin()
{
	return m_items;
}

template<typename T>
inline typename Array<T>::ConstIterator Array<T>::begin() const
{
	return m_items;
}

template<typename T>
inline typename Array<T>::ConstIterator Array<T>::cbegin() const
{
	return m_items;
}

template<typename T>
inline typename Array<T>::Iterator Array<T>::end()
{
	return m_items + m_size;
}

template<typename T>
inline typename Array<T>::ConstIterator Array<T>::end() const
{
	return m_items + m_size;
}

template<typename T>
inline typename Array<T>::ConstIterator Array<T>::cend() const
{
	return m_items + m_size;
}

template<typename T>
inline typename Array<T>::Iterator Array<T>::data()
{
	return m_items;
}

template<typename T>
inline typename Array<T>::ConstIterator Array<T>::data() const
{
	return m_items;
}

// non-member swap function
template <typename T>
inline void swap(Array<T>& a, Array<T>& b) noexcept
{
	a.swap(b);
}
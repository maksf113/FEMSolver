#pragma once
#include <utility>
#include <cstddef>
#include <initializer_list>

template <typename T>
class List
{
private:
	struct Node;
public:
	class ConstIterator;
	class Iterator;
private:
	size_t m_size;
	Node* m_head;
	Node* m_tail;
public:
	List();
	List(std::initializer_list<T> iniList);
	List(const List& other);
	List(List&& other) noexcept;
	~List();
	List& operator=(const List& other);
	List& operator=(List&& other) noexcept;
	void swap(List& other);

	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;
	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;

	size_t size() const;
	bool empty() const;
	void clear();
	bool operator==(const List& other) const;
	bool operator!=(const List& other) const;

	T& front();
	const T& front() const;
	T& back();
	const T& back() const;

	void pushFront(const T& t);
	void pushFront(T&& t);
	void pushBack(const T& t);
	void pushBack(T&& t);
	void popFront();
	void popBack();

	Iterator insert(Iterator it, const T& t);
	Iterator insert(Iterator it, T&& t);

	Iterator erase(Iterator it);
	Iterator erase(Iterator from, Iterator to);
private:
	void init();
};

template <typename T>
struct List<T>::Node
{
	T item;
	Node* prev;
	Node* next;
	Node(const T& t = T(), Node* p = nullptr, Node* n = nullptr)
		: item(t), prev(p), next(n) {
	}
	Node(T&& t, Node* p = nullptr, Node* n = nullptr)
		: item(std::move(t)), prev(p), next(n) {
	}
};

template <typename T>
class List<T>::ConstIterator
{
	friend class List<T>;
protected:
	Node* m_current;
	const List<T>* m_list;
public:
	ConstIterator() : m_list(nullptr), m_current(nullptr) {}
	const T& operator*() const { return m_current->item; }
	const T* operator->() const { return &(m_current->item); }
	ConstIterator& operator++()
	{
		m_current = m_current->next;
		return *this;
	}
	ConstIterator operator++(int)
	{
		ConstIterator old = *this;
		++(*this);
		return old;
	}
	ConstIterator& operator--()
	{
		m_current = m_current->prev;
		return *this;
	}
	ConstIterator operator--(int)
	{
		ConstIterator old = *this;
		--(*this);
		return old;
	}
	bool operator==(const ConstIterator& it) const
	{
		return m_current == it.m_current;
	}
	bool operator!=(const ConstIterator& it) const
	{
		return !(*this == it);
	}
protected:
	ConstIterator(const List<T>& other, Node* p) : m_list(&other), m_current(p) {}
};

template<typename T>
class List<T>::Iterator : public ConstIterator
{
	friend class List<T>;
protected:
	Iterator(const List<T>& other, Node* p) : ConstIterator(other, p) {}
public:
	Iterator() : ConstIterator() {}
	T& operator*() { return ConstIterator::m_current->item; }
	const T& operator*() const { return ConstIterator::m_current->item; }
	T* operator->() { return &(ConstIterator::m_current->item); }
	const T* operator->() const { return &(ConstIterator::m_current->item); }
	Iterator& operator++()
	{
		this->m_current = this->m_current->next;
		return *this;
	}
	Iterator operator++(int)
	{
		Iterator old = *this;
		++(*this);
		return old;
	}
	Iterator& operator--()
	{
		this->m_current = this->m_current->prev;
		return *this;
	}
	Iterator operator--(int)
	{
		Iterator old = *this;
		--(*this);
		return old;
	}
};

template<typename T>
inline List<T>::List()
{
	init();
}

template<typename T>
inline List<T>::List(std::initializer_list<T> iniList)
{
	init();
	for (const auto& item : iniList)
		pushBack(item);
}

template<typename T>
inline List<T>::List(const List& other)
{
	init();
	for (auto& x : other)
		pushBack(x);
}

template<typename T>
inline List<T>::List(List&& other) noexcept
	: m_size(other.m_size), m_head(other.m_head), m_tail(other.m_tail)
{
	other.init();
}

template<typename T>
inline List<T>::~List()
{
	clear();
	delete m_head;
	delete m_tail;
}

template<typename T>
inline List<T>& List<T>::operator=(const List<T>& other)
{
	List<T> copy(other);
	std::swap(*this, copy);
	return *this;
}

template<typename T>
inline List<T>& List<T>::operator=(List<T>&& other) noexcept
{
	std::swap(m_size, other.m_size);
	std::swap(m_head, other.m_head);
	std::swap(m_tail, other.m_tail);
	return *this;
}

template<typename T>
inline void List<T>::swap(List& other)
{
	std::swap(m_size, other.m_size);
	std::swap(m_head, other.m_head);
	std::swap(m_tail, other.m_tail);
}

template<typename T>
typename List<T>::Iterator List<T>::begin() 
{ 
	return Iterator(*this, m_head->next); 
}

template<typename T>
typename List<T>::ConstIterator List<T>::begin() const
{ 
	return ConstIterator(*this, m_head->next); 
}

template<typename T>
typename List<T>::ConstIterator List<T>::cbegin() const
{ 
	return ConstIterator(*this, m_head->next); 
}

template<typename T>
typename List<T>::Iterator List<T>::end()
{ 
	return Iterator(*this, m_tail); 
}

template<typename T>
typename List<T>::ConstIterator List<T>::end() const
{ 
	return ConstIterator(*this, m_tail); 
}

template<typename T>
typename List<T>::ConstIterator List<T>::cend() const
{ 
	return ConstIterator(*this, m_tail);
}

template<typename T>
size_t List<T>::size() const { return m_size; }

template<typename T>
bool List<T>::empty() const { return m_size == 0; }

template<typename T>
void List<T>::clear() { while (!empty()) popFront(); }

template<typename T>
inline bool List<T>::operator==(const List& other) const
{
	if(m_size != other.m_size)
		return false;
	auto it1 = cbegin();
	auto it2 = other.cbegin();

	while (it1 != cend())
	{
		if (*it1 != *it2)
			return false;
		++it1;
		++it2;
	}

	return true;
}

template<typename T>
inline bool List<T>::operator!=(const List& other) const
{
	return !(*this == other);
}

template<typename T>
T& List<T>::front() 
{ 
	return *(begin()); 
}

template<typename T>
const T& List<T>::front() const 
{ 
	return *(cbegin()); 
}

template<typename T>
T& List<T>::back() 
{ 
	return *(--end()); 
}

template<typename T>
const T& List<T>::back() const 
{ 
	return *(--cend()); 
}

template<typename T>
void List<T>::pushFront(const T& t)
{ 
	insert(begin(), t); 
}

template<typename T>
void List<T>::pushFront(T&& t)
{
	insert(begin(), std::move(t)); 
}

template<typename T>
void List<T>::pushBack(const T& t)
{ 
	insert(end(), t);
}

template<typename T>
void List<T>::pushBack(T&& t) 
{
	insert(end(), std::move(t));
}

template<typename T>
void List<T>::popFront()
{ 
	erase(begin()); 
}

template<typename T>
void List<T>::popBack() 
{ 
	erase(--end());
}

template<typename T>
inline typename List<T>::Iterator List<T>::insert(Iterator it, const T& t)
{
	Node* p = it.m_current;
	m_size++;
	return Iterator(*this, p->prev = p->prev->next = new Node(t, p->prev, p));
}

template<typename T>
inline typename List<T>::Iterator List<T>::insert(Iterator it, T&& t)
{
	Node* p = it.m_current;
	m_size++;
	return Iterator(*this, p->prev = p->prev->next = new Node(std::move(t), p->prev, p));
}

template<typename T>
inline typename List<T>::Iterator List<T>::erase(Iterator it)
{
	Node* p = it.m_current;
	Iterator retVal(*this, p->next);
	p->prev->next = p->next;
	p->next->prev = p->prev;
	delete p;
	m_size--;
	return retVal;
}

template<typename T>
inline typename List<T>::Iterator List<T>::erase(Iterator from, Iterator to)
{
	for (Iterator it = from; it != to;)
		it = erase(it);
	return to;
}

template<typename T>
inline void List<T>::init()
{
	m_size = 0;
	m_head = new Node();
	m_tail = new Node();
	m_head->next = m_tail;
	m_tail->prev = m_head;
}

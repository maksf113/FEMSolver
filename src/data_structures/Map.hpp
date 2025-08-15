#pragma once
#include <utility>
#include <functional>

#include "Array.hpp"

template <typename Key, typename Value,
		  typename Hasher = std::hash<Key>,
		  typename KeyEqual = std::equal_to<Key>>
class Map
{
private:
	struct Node;
public:
	struct ConstIterator;
	struct Iterator;
public:
	explicit Map(size_t bucketCount = 8);
	Map(const Map& other);
	Map(Map&& other) noexcept;
	~Map();
	Map& operator=(const Map& other);
	Map& operator=(Map&& other) noexcept;
	void swap(Map& other);

	Iterator begin();
	ConstIterator begin() const;
	ConstIterator cbegin() const;
	Iterator end();
	ConstIterator end() const;
	ConstIterator cend() const;

	Value& operator[](const Key& key);
	Value& operator[](Key&& key);
	Value& at(const Key& key);
	Value& at(Key&& key);
	const Value& at(const Key& key) const;


	bool empty() const;
	size_t size() const;
	size_t capacity() const;

	void clear();

	Iterator find(const Key& key);
	ConstIterator find(const Key& key) const;

	std::pair<Iterator, bool> insert(const std::pair<Key, Value>& pair);
	std::pair<Iterator, bool> insert(std::pair<Key, Value>&& pair);

	Iterator erase(ConstIterator it);

private:
	Array<Node*> m_buckets;
	size_t m_size;
	float m_maxLoadFactor;
	Hasher m_hasher;
	KeyEqual m_keyEqual;

private:
	void rehash(size_t newCapacity);
	size_t getBucketIndex(const Key& key) const;
	Node* findFirstNode() const; // nullptr if not found
};

// Node definition
template <typename Key, typename Value, typename Hasher, typename KeyEqual>
struct Map<Key, Value, Hasher, KeyEqual>::Node
{
	std::pair<const Key, Value> pair;
	Node* next;

	Node(const Key& k, const Value& v, Node* n = nullptr)
		: pair(k, v), next(n) {}
	Node(const Key& k, Value&& v, Node* n = nullptr)
		: pair(k, std::move(v)), next(n) {}
	Node(Key&& k, Value&& v, Node* n = nullptr)
		: pair(std::move(k), std::move(v)), next(n) {}
};
//ConstIterator definition
template <typename Key, typename Value, typename Hasher, typename KeyEqual>
class Map<Key, Value, Hasher, KeyEqual>::ConstIterator
{
	friend class Map<Key, Value, Hasher, KeyEqual>;
protected:
	const Map* m_map;
	Node* m_current;
public:
	ConstIterator() : m_map(nullptr), m_current(nullptr) {}

	const std::pair<const Key, Value>& operator*() const { return m_current->pair; }
	const std::pair<const Key, Value>* operator->() const { return &(m_current->pair); }

	bool operator==(const ConstIterator& other) const { return m_current == other.m_current; }
	bool operator!=(const ConstIterator& other) const { return m_current != other.m_current; }

	ConstIterator& operator++()
	{
		// next in the bucket if not nullptr
		if (m_current->next)
		{
			m_current = m_current->next;
			return *this;
		}
		// next bucket otherwise
		size_t bucketIdx = m_map->getBucketIndex(m_current->pair.first);
		for (size_t i = bucketIdx + 1; i < m_map->capacity(); ++i)
		{
			if (m_map->m_buckets[i])
			{
				m_current = m_map->m_buckets[i];
				return *this;
			}
		}
		m_current = nullptr;
		return *this;
	}
	ConstIterator operator++(int)
	{
		ConstIterator old = *this;
		++(*this);
		return old;
	}
protected:
	ConstIterator(const Map* map, Node* n) : m_map(map), m_current(n) {}

};
// Iterator definition
template <typename Key, typename Value, typename Hasher, typename KeyEqual>
class Map<Key, Value, Hasher, KeyEqual>::Iterator : public ConstIterator
{
	friend class Map<Key, Value, Hasher, KeyEqual>;
public:
	Iterator() : ConstIterator() {}

	const std::pair<const Key, Value>& operator*() const { return m_current->pair; }
	std::pair<const Key, Value>& operator*() { return m_current->pair; }
	const std::pair<const Key, Value>* operator->() const { return &(m_current->pair); }
	std::pair<const Key, Value>* operator->() { return &(m_current->pair); }

	Iterator& operator++()
	{
		ConstIterator::operator++();
		return *this;
	}
	Iterator operator++(int)
	{
		Iterator old = *this;
		++(*this);
		return old;
	}
private:
	Iterator(const Map* map, Node* n) : ConstIterator(map, n) {}
};
// constructors & destructor
template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Map<Key, Value, Hasher, KeyEqual>::Map(size_t bucketCount)
	: m_buckets(bucketCount > 0 ? bucketCount : 1),
	  m_size(0), m_maxLoadFactor(1.0f) {}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Map<Key, Value, Hasher, KeyEqual>::Map(const Map& other)
	: m_buckets(other.capacity()),
	  m_size(other.m_size),
	  m_maxLoadFactor(other.m_maxLoadFactor),
	  m_hasher(other.m_hasher),
	  m_keyEqual(other.m_keyEqual)
{
	// deep copy
	for (size_t i = 0; i < other.capacity(); ++i)
	{
		Node* currentOther = other.m_buckets[i];
		Node** currentThis = &m_buckets[i];
		while (currentOther)
		{
			*currentThis = new Node(currentOther->pair.first, currentOther->pair.second);
			currentOther = currentOther->next;
			currentThis = &((*currentThis)->next);
		}
	}
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Map<Key, Value, Hasher, KeyEqual>::Map(Map&& other) noexcept :
	m_buckets(std::move(other.m_buckets)),
	m_size(other.m_size),
	m_maxLoadFactor(other.m_maxLoadFactor),
	m_hasher(std::move(other.m_hasher)),
	m_keyEqual(std::move(other.m_keyEqual))
{
	other.m_size = 0;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Map<Key, Value, Hasher, KeyEqual>::~Map()
{
	clear();
}

// assignment & swap
template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Map<Key, Value, Hasher, KeyEqual>&
Map<Key, Value, Hasher, KeyEqual>::operator=(const Map& other)
{
	Map copy(other);
	swap(copy);
	return *this;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Map<Key, Value, Hasher, KeyEqual>& 
Map<Key, Value, Hasher, KeyEqual>::operator=(Map&& other) noexcept
{
	swap(other);
	return *this;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline void Map<Key, Value, Hasher, KeyEqual>::swap(Map& other)
{
	using std::swap;
	swap(m_buckets, other.m_buckets);
	swap(m_size, other.m_size);
	swap(m_maxLoadFactor, other.m_maxLoadFactor);
	swap(m_hasher, other.m_hasher);
	swap(m_keyEqual, other.m_keyEqual);
}

// capacity

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline bool Map<Key, Value, Hasher, KeyEqual>::empty() const
{
	return m_size == 0;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline size_t Map<Key, Value, Hasher, KeyEqual>::size() const
{
	return m_size;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline size_t Map<Key, Value, Hasher, KeyEqual>::capacity() const
{
	return m_buckets.size();
}

// iterators
template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::Iterator 
Map<Key, Value, Hasher, KeyEqual>::begin()
{
	return Iterator(this, findFirstNode());
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::ConstIterator
Map<Key, Value, Hasher, KeyEqual>::begin() const
{
	return ConstIterator(this, findFirstNode());
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::ConstIterator
Map<Key, Value, Hasher, KeyEqual>::cbegin() const
{
	return ConstIterator(this, findFirstNode());
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::Iterator
Map<Key, Value, Hasher, KeyEqual>::end()
{
	return Iterator(this, nullptr);
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::ConstIterator
Map<Key, Value, Hasher, KeyEqual>::end() const
{
	return ConstIterator(this, nullptr);
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::ConstIterator
Map<Key, Value, Hasher, KeyEqual>::cend() const
{
	return ConstIterator(this, nullptr);
}


// element access, modifiers & lookup
template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Value& Map<Key, Value, Hasher, KeyEqual>::operator[](const Key& key)
{
	Iterator it = find(key);
	if (it != end())
		return it->second;
	// key not found - insert with default constructed value
	return insert({ key, Value{} }).first->second;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline Value& Map<Key, Value, Hasher, KeyEqual>::operator[](Key&& key)
{
	Iterator it = find(key);
	if (it != end())
		return it->second;
	// key not found - insert with default constructed value
	return insert({ std::move(key), Value{} }).first->second;
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::Iterator 
Map<Key, Value, Hasher, KeyEqual>::find(const Key& key)
{
	size_t bucketIdx = getBucketIndex(key);
	Node* current = m_buckets[bucketIdx];
	while (current)
	{
		if (m_keyEqual(current->pair.first, key))
			return Iterator(this, current);
		current = current->next;
	}
	return end();
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::ConstIterator
Map<Key, Value, Hasher, KeyEqual>::find(const Key& key) const
{
	size_t bucketIdx = getBucketIndex(key);
	Node* current = m_buckets[bucketIdx];
	while (current)
	{
		if (m_keyEqual(current->pair.first, key))
			return ConstIterator(this, current);
		current = current->next;
	}
	return cend();
}



template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline std::pair<typename Map<Key, Value, Hasher, KeyEqual>::Iterator, bool>
Map<Key, Value, Hasher, KeyEqual>::insert(const std::pair<Key, Value>& pair)
{
	Iterator existing = find(pair.first);
	// key already exists
	if (existing != end())
		return { existing, false };

	if (static_cast<float>(m_size + 1) / capacity() > m_maxLoadFactor)
		rehash(capacity() * 2);

	size_t bucketIdx = getBucketIndex(pair.first);
	Node* newNode = new Node(pair.first, pair.second, m_buckets[bucketIdx]);
	m_buckets[bucketIdx] = newNode;
	m_size++;

	return { Iterator(this, newNode), true };
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline std::pair<typename Map<Key, Value, Hasher, KeyEqual>::Iterator, bool>
Map<Key, Value, Hasher, KeyEqual>::insert(std::pair<Key, Value>&& pair)
{
	Iterator existing = find(pair.first);
	// key already exists
	if (existing != end())
		return { existing, false };

	if (static_cast<float>(m_size + 1) / capacity() > m_maxLoadFactor)
		rehash(capacity() * 2);

	size_t bucketIdx = getBucketIndex(pair.first);
	Node* newNode = new Node(std::move(pair.first), std::move(pair.second), m_buckets[bucketIdx]);
	m_buckets[bucketIdx] = newNode;
	m_size++;

	return { Iterator(this, newNode), true };
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::Iterator 
Map<Key, Value, Hasher, KeyEqual>::erase(ConstIterator it)
{
	// end or other map
	if (it.m_map != this || it.m_current == nullptr)
		return end();
	// find the node to delete and previous node to set next
	const Key& key = it->first;
	size_t bucketIdx = getBucketIndex(key);
	Node* current = m_buckets[bucketIdx];
	Node* prev = nullptr;

	// find the node in the bucket to delete
	while (current)
	{
		if (current == it.m_current)
		{
			Iterator nextIt(this, it.m_current);
			++nextIt;

			if (prev) // not first in the chain
				prev->next = current->next;
			else // first in the chain to erase
				m_buckets[bucketIdx] = current->next;

			delete current;
			m_size--;
			return nextIt;
		}
		prev = current;
		current = current->next;
	}
	return end(); // should not be reached for valid iterator
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline void Map<Key, Value, Hasher, KeyEqual>::clear()
{
	for (size_t i = 0; i < capacity(); ++i)
	{
		Node* current = m_buckets[i];
		while (current)
		{
			Node* toDelete = current;
			current = current->next;
			delete toDelete;
		}
		m_buckets[i] = nullptr;
	}
	m_size = 0;
}

// priovate helpers
template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline void Map<Key, Value, Hasher, KeyEqual>::rehash(size_t newCapacity)
{
	if (newCapacity <= capacity())
		return;

	Array<Node*> oldBuckets(0);
	std::swap(m_buckets, oldBuckets);

	m_buckets.resize(newCapacity);
	// relink nodes
	for (size_t i = 0; i < oldBuckets.size(); ++i)
	{
		Node* current = oldBuckets[i];
		while (current)
		{
			Node* next = current->next;
			size_t newIndex = getBucketIndex(current->pair.first);
			current->next = m_buckets[newIndex];
			m_buckets[newIndex] = current;

			current = next;
		}
	}
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline size_t Map<Key, Value, Hasher, KeyEqual>::getBucketIndex(const Key& key) const
{
	return m_hasher(key) % capacity();
}

template<typename Key, typename Value, typename Hasher, typename KeyEqual>
inline typename Map<Key, Value, Hasher, KeyEqual>::Node* 
Map<Key, Value, Hasher, KeyEqual>::findFirstNode() const
{
	for (auto& bucket : m_buckets)
		if (bucket)
			return bucket;
	return nullptr; // empty map
}

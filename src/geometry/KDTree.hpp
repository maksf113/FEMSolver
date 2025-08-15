#pragma once
#include <algorithm>
#include <limits>

#include "data_structures/Array.hpp"
#include "Point.hpp"

template <typename PointType, size_t K>
class KDTree
{
	static_assert(K > 0, "KDTree dimension K must be grater than 0");

private:
	struct Node;
private:

	Node* m_root;
	size_t m_size; // number of points in the tree

public:
	KDTree();
	explicit KDTree(Array<PointType> points); // by value - will modify
	KDTree(const KDTree& other);
	KDTree(KDTree&& other) noexcept;
	~KDTree();

	KDTree& operator=(const KDTree& other);
	KDTree& operator=(KDTree&& other) noexcept;

	void swap(KDTree& other) noexcept;

	template <typename QueryPointType>
	PointType findNearest(const QueryPointType& target) const;

	bool empty() const noexcept;
	size_t size() const noexcept;

private:
	Node* buildRecursive(Array<PointType>& points, size_t left, size_t right, size_t depth);
	template <typename QueryPointType>
	void findNearestRecursive(const Node* node, const QueryPointType& target, 
		PointType& bestPoint, double& bestDistSq, size_t depth) const;
	void destroyRecursive(Node* node);
	Node* copyRecursive(const Node* other);
};

template <typename PointType, size_t K>
struct KDTree<PointType, K>::Node
{
	PointType point; // point stored in the node
	Node* left; // coordinate smaller
	Node* right; // coordinate larger
	
	Node(const PointType& p, Node* l = nullptr, Node* r = nullptr)
		: point(p), left(l), right(r) {}
	Node(PointType&& p, Node* l = nullptr, Node* r = nullptr)
		: point(std::move(p)), left(l), right(r) {}
};

// constructors, destructor, assignements
template <typename PointType, size_t K>
inline KDTree<PointType, K>::KDTree() : m_root(nullptr), m_size(0) {}

template<typename PointType, size_t K>
inline KDTree<PointType, K>::KDTree(Array<PointType> points)
	:m_root(nullptr), m_size(0)
{
	if (points.empty())
		return;
	m_size = points.size();
	m_root = buildRecursive(points, 0, points.size(), 0);
}

template<typename PointType, size_t K>
inline KDTree<PointType, K>::KDTree(const KDTree& other)
	: m_root(copyRecursive(other.m_root)), m_size(other.m_size) {}

template<typename PointType, size_t K>
inline KDTree<PointType, K>::KDTree(KDTree&& other) noexcept
	: m_root(other.m_root), m_size(other.m_size)
{
	other.m_root = nullptr;
	other.m_size = 0;
}

template<typename PointType, size_t K>
inline KDTree<PointType, K>::~KDTree()
{
	destroyRecursive(m_root);
}

template<typename PointType, size_t K>
inline KDTree<PointType, K>& KDTree<PointType, K>::operator=(const KDTree<PointType, K>& other)
{
	if (this != &other)
	{
		KDTree temp(other);
		swap(temp);
	}
	return *this;
}

template<typename PointType, size_t K>
inline KDTree<PointType, K>& KDTree<PointType, K>::operator=(KDTree<PointType, K>&& other) noexcept
{
	if (this != &other)
	{
		destroyRecursive(m_root); // clean current tree
		m_root = other.m_root;
		m_size = other.m_size;
		other.m_root = nullptr;
		other.m_size = 0;
	}
	return *this;
}

template<typename PointType, size_t K>
inline void KDTree<PointType, K>::swap(KDTree& other) noexcept
{
	std::swap(m_root, other.m_root);
	std::swap(m_size, other.m_size);
}

// public methods

template <typename PointType, size_t K>
template <typename QueryPointType>
inline PointType KDTree<PointType, K>::findNearest(const QueryPointType& target) const
{
	//static_assert(m_root, "Cannot find nearest in an empty KDTRee");
	if (m_root == nullptr)
		throw std::runtime_error("KDTree is empty, cannot find nearest point");
	PointType bestPoint = m_root->point;
	double bestDistSq = distSquared(target, bestPoint);
	findNearestRecursive(m_root, target, bestPoint, bestDistSq, 0);
	return bestPoint;
}

template<typename PointType, size_t K>
inline bool KDTree<PointType, K>::empty() const noexcept
{
	return m_size == 0;
}

template<typename PointType, size_t K>
inline size_t KDTree<PointType, K>::size() const noexcept
{
	return m_size;
}

// private helper methods

template<typename PointType, size_t K>
inline typename KDTree<PointType, K>::Node* KDTree<PointType, K>::buildRecursive(Array<PointType>& points, size_t left, size_t right, size_t depth)
{
	// no points in this range
	if(left >= right) 
		return nullptr; 
	size_t axis = depth % K; // splitting axis
	size_t medianIdx = left + (right - left) / 2;
	// partition subarray [left, right) around median element
	std::nth_element(points.begin() + left, points.begin() + medianIdx,
		points.begin() + right,
		[axis](const PointType& a, const PointType& b)
		{return a[axis] < b[axis]; }
		);
	// create a new node with the median point
	Node* node = new Node(points[medianIdx]);
	//recursively
	node->left = buildRecursive(points, left, medianIdx, depth + 1);
	node->right = buildRecursive(points, medianIdx + 1, right, depth + 1);
	return node;
}

template<typename PointType, size_t K>
template <typename QueryPointType>
inline void KDTree<PointType, K>::findNearestRecursive(const Node* node, const QueryPointType& target, PointType& bestPoint, double& bestDistSq, size_t depth) const
{
	if (node == nullptr)
		return;
	// squared distance from target to current nodes point
	double currentDistSq = distSquared(target, node->point);
	if (currentDistSq < bestDistSq)
	{
		bestDistSq = currentDistSq;
		bestPoint = node->point;
	}
	size_t axis = depth % K;
	// signed distance from target to splitting hyperplane 
	// defined by node->point on current axis
	double distToPlane = target[axis] - node->point[axis];
	// near subtree containing target to visit first
	Node* nearSubtree = (distToPlane < 0) ? node->left  : node->right;
	Node* farSubtree  = (distToPlane < 0) ? node->right : node->left;

	findNearestRecursive(nearSubtree, target, bestPoint, bestDistSq, depth + 1);
	// if hypersphere around target with radius bestDist 
	// itersects splitting hyperplane, farSubtree need to be visited too
	if (distToPlane * distToPlane < bestDistSq)
		findNearestRecursive(farSubtree, target, bestPoint, bestDistSq, depth + 1);
}

template<typename PointType, size_t K>
inline void KDTree<PointType, K>::destroyRecursive(Node* node)
{
	if (node)
	{
		destroyRecursive(node->left);
		destroyRecursive(node->right);
		delete node;
	}
}

template<typename PointType, size_t K>
inline typename KDTree<PointType, K>::Node* KDTree<PointType, K>::copyRecursive(const Node* otherNode)
{
	if (!otherNode)
		return nullptr;
	Node* newNode = new Node(otherNode->point);
	newNode->left = copyRecursive(otherNode->left);
	newNode->right = copyRecursive(otherNode->right);
	return newNode;
}
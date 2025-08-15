#pragma once
#include <cassert>
#include "data_structures/List.hpp"
#include "data_structures/Array.hpp"
#include "data_structures/Map.hpp"
#include "Point.hpp"
#include "Boundaries.hpp"

class Triangulation
{
private:
	struct Vertex;
	struct HalfEdge;
	struct Face;
public:
	static constexpr size_t INVALID_IDX = std::numeric_limits<size_t>::max();
	struct VertexHandle
	{
		size_t idx = INVALID_IDX;
		bool operator==(const VertexHandle& other) const { return idx == other.idx; }
		bool operator!=(const VertexHandle& other) const { return idx != other.idx; }
	};
	static constexpr VertexHandle INVALID_VERTEX_HANDLE{ INVALID_IDX };
	struct Triangulation::HalfEdgeHandle
	{
		size_t idx;
		bool operator==(const HalfEdgeHandle& other) const { return idx == other.idx; }
		bool operator!=(const HalfEdgeHandle& other) const { return idx != other.idx; }
	};
	static constexpr HalfEdgeHandle INVALID_HALFEDGE_HANDLE{ INVALID_IDX };
	struct Triangulation::FaceHandle
	{
		size_t idx = INVALID_IDX;
		bool operator==(const FaceHandle& other) const { return idx == other.idx; }
		bool operator!=(const FaceHandle& other) const { return idx != other.idx; }
	};
	static constexpr FaceHandle INVALID_FACE_HANDLE{ INVALID_IDX };
	class FaceHandle;
	class VertexAccessor;
	class HalfEdgeAccessor;
	class FaceAccessor;
	struct VertexHandleHash;
private:
	Array<Vertex> m_vertices;
	Array<HalfEdge> m_halfEdges;
	Array<Face> m_faces;
	List<size_t> m_activeFaces;
	List<size_t> m_freeFaces;
	List<size_t> m_freeHalfEdges;
	Point m_superPoints[3]; // for initial super triangle
	Array<Point> m_trianglePoints;
	int m_smoothingIterations = 50;
public:
	Triangulation(const Boundaries& boundaries, Array<Point>& innerPoints);
	~Triangulation() = default;
	Triangulation(const Triangulation&) = delete;
	Triangulation(Triangulation&&) = delete;
	Triangulation& operator=(const Triangulation&) = delete;
	Triangulation& operator=(Triangulation&&) = delete;
	const Array<Point>& getTrianglePoints() const; 
	const Point& getVertexPoint(size_t i) const;
	int getVertexBoundaryId(size_t i) const;
	size_t vertexCount() const;
	StaticArray<size_t, 3> getTriangleVertexIndices(size_t i) const;
	size_t getTriangleCount() const;
private:
	// --- half-edge accessors ---
	//  - top-level accessors (get element by its handle) -
	VertexAccessor getVertex(VertexHandle handle);
	HalfEdgeAccessor getHalfEdge(HalfEdgeHandle handle);
	FaceAccessor getFace(FaceHandle handle);
	// - navigation getters (from one element to another) -
	// Vertex
	HalfEdgeAccessor getLeaving(VertexHandle handle);
	Point* getPoint(VertexHandle handle);
	int getBoundaryId(VertexHandle handle);
	// HalfEdge
	VertexAccessor getOrigin(HalfEdgeHandle handle);
	HalfEdgeAccessor getNext(HalfEdgeHandle handle);

	HalfEdgeAccessor getTwin(HalfEdgeHandle handle);
	FaceAccessor getAdjacentFace(HalfEdgeHandle handle);
	// Face
	HalfEdgeAccessor getAdjacentHalfEdge(FaceHandle handle);
	// - setters -
	// Vertex
	void setLeaving(VertexHandle vertexHandle, HalfEdgeHandle heHandle);
	void setPoint(VertexHandle vertexHandle, Point* point);
	void setBoundaryId(VertexHandle vertexHandle, int id);
	// HalfEdge
	void setOrigin(HalfEdgeHandle heHandle, VertexHandle vertexHandle);
	void setNext(HalfEdgeHandle heHandle, HalfEdgeHandle nextHandle);
	void setTwin(HalfEdgeHandle heHandle, HalfEdgeHandle twinHandle);
	void setAdjacentFace(HalfEdgeHandle heHandle, FaceHandle faceHandle);
	// Face
	void setAdjacentHalfEdge(FaceHandle faceHandle, HalfEdgeHandle heHandle);
private:
	void initializeWithSuperTriangle(const Boundaries& boundaries);
	void addVertex(Point* point);
	void removeExteriorTriangles(const Boundaries& boundaries);
	void laplaceSmoothing(int iterations);
	VertexAccessor pushVertex();
	HalfEdgeAccessor pushHalfEdge();
	FaceAccessor pushFace();
	void removeWholeEdge(HalfEdgeHandle halfEdge);
	void removeFace(size_t activeFaceIdx);
	void removeFaces(const List<size_t>& faceIndices);
	void makeCompact();
};

bool isInCircumcircle(Triangulation::FaceAccessor face, const Point& point);

// ----- vertex -----
struct Triangulation::Vertex
{
	Point* point = nullptr; // pointer to geometric point
	size_t leaving = INVALID_IDX; // index of one of the outgoing half-edges
	int boundaryId = -1;
	// reset method
	void reset() { point = nullptr; leaving = INVALID_IDX; boundaryId = -1;	 }
};

struct Triangulation::VertexHandleHash
{
	size_t operator()(const VertexHandle& handle) const
	{
		return std::hash<size_t>()(handle.idx);
	}
};

class Triangulation::VertexAccessor
{
public:
	VertexAccessor(Triangulation* mesh, VertexHandle handle) : m_mesh(mesh), m_handle(handle) {}
	VertexAccessor(Triangulation* mesh, size_t idx) : m_mesh(mesh), m_handle(VertexHandle{ idx }) {}
	// getters
	VertexHandle handle() const { return m_handle; }
	HalfEdgeAccessor leaving() const;
	Point* point() const;
	int boundaryId() const;
	// setters
	void setLeaving(HalfEdgeHandle);
	void setPoint(Point*);
	void setBoundaryId(int);
private:
	Triangulation* m_mesh;
	VertexHandle m_handle;
};
// ----- half-edge -----
struct Triangulation::HalfEdge
{
	size_t origin = INVALID_IDX; // half-edge starting vertex
	size_t twin = INVALID_IDX; // oppositely oriented half-edge
	size_t next = INVALID_IDX; // next in face loop
	size_t adjacentFace = INVALID_IDX; // face on the left of the HE (anti-clockwise orientation)
	// reset method
	void reset() { origin = twin = next = adjacentFace = INVALID_IDX; }
};


class Triangulation::HalfEdgeAccessor
{
public:
	HalfEdgeAccessor(Triangulation* mesh, HalfEdgeHandle handle) : m_mesh(mesh), m_handle(handle) {}
	HalfEdgeAccessor(Triangulation* mesh, size_t idx) : m_mesh(mesh), m_handle(HalfEdgeHandle{ idx }) {}
	// getters
	HalfEdgeHandle handle() const { return m_handle; }
	VertexAccessor origin() const;
	HalfEdgeAccessor next() const;
	HalfEdgeAccessor twin() const;
	FaceAccessor adjacentFace() const;
	//setters
	void setOrigin(VertexHandle);
	void setNext(HalfEdgeHandle);
	void setTwin(HalfEdgeHandle);
	void setAdjacentFace(FaceHandle);
private:
	Triangulation* m_mesh;
	HalfEdgeHandle m_handle;
};

// ----- face -----
struct Triangulation::Face
{
	size_t adjacentHalfEdge = INVALID_IDX; // one of the half-edges
	// reset method
	void reset() { adjacentHalfEdge = INVALID_IDX; }
};


class Triangulation::FaceAccessor
{
public:
	FaceAccessor(Triangulation* mesh, FaceHandle handle) : m_mesh(mesh), m_handle(handle) {}
	FaceAccessor(Triangulation* mesh, size_t idx) : m_mesh(mesh), m_handle(FaceHandle{ idx }) {}
	// getters
	FaceHandle handle() const { return m_handle; }
	HalfEdgeAccessor adjacentHalfEdge() const;
	// setter
	void setAdjacentHalfEdge(HalfEdgeHandle);
private:
	Triangulation* m_mesh;
	FaceHandle m_handle;
};

// --- Accessors implemantations ---

	//  - top-level accessors (get element by its handle) -
Triangulation::VertexAccessor Triangulation::getVertex(VertexHandle handle)
{
	return VertexAccessor{ this, handle };
}
Triangulation::HalfEdgeAccessor Triangulation::getHalfEdge(HalfEdgeHandle handle)
{
	return HalfEdgeAccessor{ this, handle };
}
Triangulation::FaceAccessor Triangulation::getFace(FaceHandle handle)
{
	return FaceAccessor{ this, handle };
}

// - Vertex -
inline Triangulation::HalfEdgeAccessor Triangulation::VertexAccessor::leaving() const
{
	return m_mesh->getLeaving(m_handle);
}
inline Point* Triangulation::VertexAccessor::point() const
{
	return m_mesh->getPoint(m_handle);
}
inline int Triangulation::VertexAccessor::boundaryId() const
{
	return m_mesh->getBoundaryId(m_handle);
}
// setters
inline void Triangulation::VertexAccessor::setLeaving(HalfEdgeHandle he)
{
	m_mesh->setLeaving(m_handle, he);
}
inline void Triangulation::VertexAccessor::setPoint(Point* point)
{
	m_mesh->setPoint(m_handle, point);
} 
inline void Triangulation::VertexAccessor::setBoundaryId(int id)
{
	m_mesh->setBoundaryId(m_handle, id);
}
// - HalfEdge -
inline Triangulation::VertexAccessor Triangulation::HalfEdgeAccessor::origin() const
{
	return m_mesh->getOrigin(m_handle);
}
inline Triangulation::HalfEdgeAccessor Triangulation::HalfEdgeAccessor::next() const
{
	return m_mesh->getNext(m_handle);
}
inline Triangulation::HalfEdgeAccessor Triangulation::HalfEdgeAccessor::twin() const
{
	return m_mesh->getTwin(m_handle);
}
inline Triangulation::FaceAccessor Triangulation::HalfEdgeAccessor::adjacentFace() const
{
	return m_mesh->getAdjacentFace(m_handle);
}
// setters
inline void Triangulation::HalfEdgeAccessor::setOrigin(VertexHandle vertexHandle)
{
	m_mesh->setOrigin(m_handle, vertexHandle);
}
inline void Triangulation::HalfEdgeAccessor::setNext(HalfEdgeHandle nextHandle)
{
	m_mesh->setNext(m_handle, nextHandle);
}
inline void Triangulation::HalfEdgeAccessor::setTwin(HalfEdgeHandle twinHandle)
{
	m_mesh->setTwin(m_handle, twinHandle);
}
inline void Triangulation::HalfEdgeAccessor::setAdjacentFace(FaceHandle faceHandle)
{
	m_mesh->setAdjacentFace(m_handle, faceHandle);
}
// - Face -
inline Triangulation::HalfEdgeAccessor Triangulation::FaceAccessor::adjacentHalfEdge() const
{
	return m_mesh->getAdjacentHalfEdge(m_handle);
}
// setter
inline void Triangulation::FaceAccessor::setAdjacentHalfEdge(HalfEdgeHandle heHandle)
{
	return m_mesh->setAdjacentHalfEdge(m_handle, heHandle);
}
// ---  Triangulation accessors getters/setters ---
// - Vertex -
inline Triangulation::HalfEdgeAccessor Triangulation::getLeaving(VertexHandle handle)
{
	size_t leavingIdx = m_vertices[handle.idx].leaving;
	return { this, HalfEdgeHandle{leavingIdx} };
}
inline Point* Triangulation::getPoint(VertexHandle handle)
{
	return m_vertices[handle.idx].point;
}
inline int Triangulation::getBoundaryId(VertexHandle handle)
{
	return m_vertices[handle.idx].boundaryId;
}
// setters
inline void Triangulation::setLeaving(VertexHandle vertexHandle, HalfEdgeHandle heHandle)
{
	m_vertices[vertexHandle.idx].leaving = heHandle.idx;
}

inline void Triangulation::setPoint(VertexHandle vertexHandle, Point* point)
{
	m_vertices[vertexHandle.idx].point = point;
}
inline void Triangulation::setBoundaryId(VertexHandle vertexHandle, int id)
{
	m_vertices[vertexHandle.idx].boundaryId = id;
}
// - HalfEdge -
inline Triangulation::VertexAccessor Triangulation::getOrigin(HalfEdgeHandle handle)
{
	size_t originIdx = m_halfEdges[handle.idx].origin;
	return { this, VertexHandle{originIdx} };
}

inline Triangulation::HalfEdgeAccessor Triangulation::getNext(HalfEdgeHandle handle)
{
	size_t nextIdx = m_halfEdges[handle.idx].next;
	return { this, HalfEdgeHandle{nextIdx} };
}

inline Triangulation::HalfEdgeAccessor Triangulation::getTwin(HalfEdgeHandle handle)
{
	size_t twinIdx = m_halfEdges[handle.idx].twin;
	return { this, HalfEdgeHandle{twinIdx} };
}

inline Triangulation::FaceAccessor Triangulation::getAdjacentFace(HalfEdgeHandle handle)
{
	size_t faceIdx = m_halfEdges[handle.idx].adjacentFace;
	return { this, FaceHandle{faceIdx} };
}
// setters
inline void Triangulation::setOrigin(HalfEdgeHandle heHandle, VertexHandle vertexHandle)
{
	m_halfEdges[heHandle.idx].origin = vertexHandle.idx;
}

inline void Triangulation::setNext(HalfEdgeHandle heHandle, HalfEdgeHandle nextHandle)
{
	m_halfEdges[heHandle.idx].next = nextHandle.idx;
}

inline void Triangulation::setTwin(HalfEdgeHandle heHandle, HalfEdgeHandle twinHandle)
{
	m_halfEdges[heHandle.idx].twin = twinHandle.idx;
}

inline void Triangulation::setAdjacentFace(HalfEdgeHandle heHandle, FaceHandle faceHandle)
{
	m_halfEdges[heHandle.idx].adjacentFace = faceHandle.idx;
}
// - Face -
inline Triangulation::HalfEdgeAccessor Triangulation::getAdjacentHalfEdge(FaceHandle handle)
{
	size_t heIdx = m_faces[handle.idx].adjacentHalfEdge;
	return { this, HalfEdgeHandle{heIdx} };
}
// setter
inline void Triangulation::setAdjacentHalfEdge(FaceHandle faceHandle, HalfEdgeHandle heHandle)
{
	m_faces[faceHandle.idx].adjacentHalfEdge = heHandle.idx;
}
Triangulation::Triangulation(const Boundaries& boundaries, Array<Point>& innerPoints)
{
	// vertex count (3 for super triangle)
	size_t vertexCount = 3 + innerPoints.size() + boundaries.getOuterBoundary().size();
	for (const auto& inner : boundaries.getInnerBoundaries())
		vertexCount += inner.size();
	//reserve to use push back method
	m_vertices.reserve(vertexCount);
	// estimate number of faces as 2*vertexCount and number of halfEdges as 6*vertexCount. additional x2 for safety
	// resize to use indices
	m_faces.resize(4 * vertexCount);
	m_halfEdges.resize(12 * vertexCount);
	// initally no active faces and half-edges
	for (size_t i = 0; i < m_faces.size(); i++)
		m_freeFaces.pushBack(i);
	for (size_t i = 0; i < m_halfEdges.size(); i++)
		m_freeHalfEdges.pushBack(i);
	initializeWithSuperTriangle(boundaries);
		
	for (auto& point : boundaries.getOuterBoundary())
	{
		addVertex(const_cast<Point*>(&point));
		m_vertices.back().boundaryId = 0;
	}
	for (size_t i = 0; i < boundaries.getInnerBoundaries().size(); i++)
	{
		for (auto& point : (boundaries.getInnerBoundaries())[i])
		{
			addVertex(const_cast<Point*>(&point));
			m_vertices.back().boundaryId = i + 1; // 0 for outer boiundary
		}
	}
	for (auto& point : innerPoints)
	{
		addVertex(&point);
		m_vertices.back().boundaryId = -1;
	}
	removeExteriorTriangles(boundaries);

	// cleanup
	makeCompact();

	laplaceSmoothing(m_smoothingIterations);

	for (size_t i = 0; i < m_faces.size(); i++)
	{
		m_trianglePoints.pushBack(*(getFace({ i }).adjacentHalfEdge().origin().point()));
		m_trianglePoints.pushBack(*(getFace({ i }).adjacentHalfEdge().next().origin().point()));
		m_trianglePoints.pushBack(*(getFace({ i }).adjacentHalfEdge().next().next().origin().point()));
	}

}

inline void Triangulation::initializeWithSuperTriangle(const Boundaries& boundaries)
{
	// start with a equilateral triangle circumscribed about a circle
	// circusscribed about bounding box
	AABB boundingBox = boundaries.getBoundingBox();
	Point center{ (boundingBox.xMin + boundingBox.xMax) / 2.0,
				  (boundingBox.yMin + boundingBox.yMax) / 2.0 };
	//circle radius
	double r = dist(center, Point{ boundingBox.xMax, boundingBox.yMax });
	// equilateral triangle side length
	double a = 2.0 * std::sqrt(3.0) * r;
	// points in counterclockwise orientation
	// upper 
	m_superPoints[0] = center + Point{ 0.0, 2.0 * r };
	// left						Point
	m_superPoints[1] = center + Point{ -0.5 * a, -r };
	// right					Point
	m_superPoints[2] = center + Point{ 0.5 * a, -r };

	// push for initial super vertices
	VertexAccessor v0 = pushVertex();
	VertexAccessor v1 = pushVertex();
	VertexAccessor v2 = pushVertex();
	v0.setPoint(&m_superPoints[0]);
	v1.setPoint(&m_superPoints[1]);
	v2.setPoint(&m_superPoints[2]);
	// push 6 half-edges
	HalfEdgeAccessor he01 = pushHalfEdge();
	HalfEdgeAccessor he12 = pushHalfEdge();
	HalfEdgeAccessor he20 = pushHalfEdge();
		  	
	HalfEdgeAccessor he02 = pushHalfEdge();
	HalfEdgeAccessor he21 = pushHalfEdge();
	HalfEdgeAccessor he10 = pushHalfEdge();
	// set origin, twin and next
	he01.setOrigin(v0.handle());
	he01.setTwin(he10.handle());
	he01.setNext(he12.handle());

	he12.setOrigin(v1.handle());
	he12.setTwin(he21.handle());
	he12.setNext(he20.handle());

	he20.setOrigin(v2.handle());
	he20.setTwin(he02.handle());
	he20.setNext(he01.handle());

	he02.setOrigin(v0.handle());
	he02.setTwin(he20.handle());
	he02.setNext(he21.handle());

	he21.setOrigin(v2.handle());
	he21.setTwin(he12.handle());
	he21.setNext(he10.handle());

	he10.setOrigin(v1.handle());
	he10.setTwin(he01.handle());
	he10.setNext(he02.handle());
	// set half-edge in vertices
	v0.setLeaving(he01.handle());
	v1.setLeaving(he12.handle());
	v2.setLeaving(he20.handle());
	// push face
	FaceAccessor face = pushFace();
	face.setAdjacentHalfEdge(he01.handle());
	// set face in half-edges
	he01.setAdjacentFace(face.handle());
	he12.setAdjacentFace(face.handle());
	he20.setAdjacentFace(face.handle());
	// unnecessary (only for explicitness)
	he02.setAdjacentFace(INVALID_FACE_HANDLE);
	he21.setAdjacentFace(INVALID_FACE_HANDLE);
	he10.setAdjacentFace(INVALID_FACE_HANDLE);
}

inline void Triangulation::addVertex(Point* point)
{
	VertexAccessor newVertex = pushVertex();
	newVertex.setPoint(point);
	newVertex.setLeaving(INVALID_HALFEDGE_HANDLE); // will be set during retriangulation
	// find bad triangles
	List<size_t> badTriangleIndices;
	for (const auto& faceIdx : m_activeFaces)
		if (isInCircumcircle(getFace({faceIdx}), *point))
		{
			badTriangleIndices.pushBack(faceIdx);
		}
	assert(!badTriangleIndices.empty());
	//helper function to check if traingle is bad by index
	auto isTriangleBad = [&](size_t idx) 
		{
			for (const auto& badIdx : badTriangleIndices)  
				if (idx == badIdx) return true; 
			return false; 
		};
	// find all bad edges and hole boundary edges
	List<HalfEdgeHandle> cavityPolygonHE;
	List<HalfEdgeHandle> cavityInternalHE;
	for (const auto& badIdx : badTriangleIndices)
	{
		auto  badTriangle = getFace({ badIdx });
		HalfEdgeAccessor currentHE = badTriangle.adjacentHalfEdge();
		do
		{
			// if on the global boundary
			if (currentHE.twin().adjacentFace().handle() == INVALID_FACE_HANDLE)
				cavityPolygonHE.pushBack(currentHE.handle());
			else
			{
				// if twins face is bad - edge to remove
				if (isTriangleBad(currentHE.twin().adjacentFace().handle().idx))
				{
					if (currentHE.handle().idx < currentHE.twin().handle().idx) // only one half-edge of edge to be removed (method removes both)
						cavityInternalHE.pushBack(currentHE.handle());
				}
				else // twins face good - cavity plygon edge
					cavityPolygonHE.pushBack(currentHE.handle());
			}
			currentHE = currentHE.next();
		} while (currentHE.handle() != badTriangle.adjacentHalfEdge().handle());

	}
	// check if at least 3 hole half-edges
	assert(cavityPolygonHE.size() >= 3);
	// create ordered (CCW) list of hole half-edges
	List<HalfEdgeHandle> orderedCavityPolygonHE;
	orderedCavityPolygonHE.pushBack(cavityPolygonHE.front());
	while (orderedCavityPolygonHE.size() != cavityPolygonHE.size())
	{
		for (auto& cavityHE : cavityPolygonHE)
		{
			VertexAccessor nextOrigin = HalfEdgeAccessor{ this, orderedCavityPolygonHE.back() }.next().origin();
			HalfEdgeAccessor he{ this, cavityHE };
			assert(nextOrigin.handle() != INVALID_VERTEX_HANDLE);
			assert(he.origin().handle() != INVALID_VERTEX_HANDLE);
			if (he.origin().handle() == nextOrigin.handle())
			{
				orderedCavityPolygonHE.pushBack(cavityHE);
				break;
			}
		}
	}
	assert(orderedCavityPolygonHE.size() == cavityPolygonHE.size());

	// retriangulation of polygonal hole
	// first create faces and half-edges (link next, face, origin, NO twins)
	for (auto& cavityHE : orderedCavityPolygonHE)
	{
		FaceAccessor newFace = pushFace();
		newFace.setAdjacentHalfEdge(cavityHE);
		HalfEdgeAccessor he{ this, cavityHE };
		HalfEdgeAccessor heToNewVertex = pushHalfEdge();
		HalfEdgeAccessor heFromNewVertex = pushHalfEdge();
		// origin
		heToNewVertex.setOrigin(he.next().origin().handle());
		heFromNewVertex.setOrigin(newVertex.handle());
		//next
		he.setNext(heToNewVertex.handle());
		heToNewVertex.setNext(heFromNewVertex.handle());
		heFromNewVertex.setNext(cavityHE);
		// face
		he.setAdjacentFace(newFace.handle());
		heToNewVertex.setAdjacentFace(newFace.handle());
		heFromNewVertex.setAdjacentFace(newFace.handle());
		assert(he.origin().handle() != INVALID_VERTEX_HANDLE);
		assert(he.next().origin().handle() != INVALID_VERTEX_HANDLE);
		assert(he.next().next().origin().handle() != INVALID_VERTEX_HANDLE);
	}
	// link twins
	for (List<HalfEdgeHandle>::Iterator heIt = orderedCavityPolygonHE.begin(); heIt != orderedCavityPolygonHE.end(); )
	{
		HalfEdgeAccessor current{ this, *heIt };
		auto nextIterator = ++heIt;
		if (nextIterator == orderedCavityPolygonHE.end())
			nextIterator = orderedCavityPolygonHE.begin();
		HalfEdgeAccessor nextInCavity{ this, *nextIterator };
		assert(current.next().handle() != INVALID_HALFEDGE_HANDLE);
		assert(nextInCavity.next().next().handle() != INVALID_HALFEDGE_HANDLE);
		current.next().setTwin(nextInCavity.next().next().handle());
		nextInCavity.next().next().setTwin(current.next().handle());
	}
	// set newVertex half-edge
	newVertex.setLeaving(HalfEdgeAccessor{ this, orderedCavityPolygonHE.front() }.next().twin().handle());
	// remove bad faces
	removeFaces(badTriangleIndices);
	// remove bad edges
	for (HalfEdgeHandle heToRemove : cavityInternalHE)
		removeWholeEdge(heToRemove);
	// set halfedges of vertices on the hole boundary if were invalidated by half-edges removal
	for (auto& heHandle : orderedCavityPolygonHE)
	{
		HalfEdgeAccessor he{ this, heHandle };
		//if (he.origin().leaving().handle() == INVALID_HALFEDGE_HANDLE)
			he.origin().setLeaving(he.handle());
	}
}

inline void Triangulation::removeExteriorTriangles(const Boundaries& boundaries)
{
	// helper function to check if triangle is exterior
	auto isExterior = [&](FaceAccessor face)
		{
			assert(face.handle() != INVALID_FACE_HANDLE);
			assert((face.adjacentHalfEdge().next().next().next().handle() == face.adjacentHalfEdge().handle()));
			HalfEdgeAccessor he = face.adjacentHalfEdge();
			Point center = 1.0 / 3.0 * (
				*(he.origin().point()) +
				*(he.next().origin().point()) +
				*(he.next().next().origin().point())
				);
			return !boundaries.pointInBoundaries(center);
		};
	// find exterior triangles
	List<size_t> exteriorTriangleIndices;
	for (const auto& faceIdx : m_activeFaces)
	{
		if (isExterior(getFace({faceIdx})))
			exteriorTriangleIndices.pushBack(faceIdx);
	}
	//find half-edges to remove and to relink next (boundary)
	Array<HalfEdgeHandle> heToRemove;
	Array<HalfEdgeHandle> heToRelink; // boundary
	Map<VertexHandle, HalfEdgeHandle, VertexHandleHash> originToHalfEdgeMap; // for relinking
	for (const auto& exteriorIdx : exteriorTriangleIndices)
	{
		auto face = getFace({ exteriorIdx });
		auto current = face.adjacentHalfEdge();
		do
		{
			if (current.twin().adjacentFace().handle() == INVALID_FACE_HANDLE) // edge is part of supertriangle
				heToRemove.pushBack(current.handle());
			else if (isExterior(current.twin().adjacentFace())) // edge is between actual triangulation boundary and supertriangle or inner trinagle inside inner boundary
			{
				if (current.handle().idx < current.twin().handle().idx)
					heToRemove.pushBack(current.handle());
			}
			else if (!isExterior(current.twin().adjacentFace())) // edge is actual boundary
			{
				current.setAdjacentFace(INVALID_FACE_HANDLE);
				heToRelink.pushBack(current.handle());
				originToHalfEdgeMap[current.origin().handle()] = current.handle();
			}

			current = current.next();
		} while (current.handle() != face.adjacentHalfEdge().handle());
	}
	// relink next on the boundary and leaving of vertices
	for (const auto& [originHandle, boundaryHEHandle] : originToHalfEdgeMap)
	{
		// find destination of current half-edge 
		HalfEdgeAccessor current{ this, boundaryHEHandle };
		VertexAccessor nextOrigin = current.twin().origin();
		current.setNext(originToHalfEdgeMap[nextOrigin.handle()]);
	}
	// remove exterior faces and edges
	removeFaces(exteriorTriangleIndices);
	for (auto he : heToRemove)
		removeWholeEdge(he);
	// relink leaving of vertices on the boundary
	for (auto& current : heToRelink)
		HalfEdgeAccessor{ this, current }.origin().setLeaving(current);
}

inline void Triangulation::laplaceSmoothing(int iterations)
{
	for (int iter = 0; iter < iterations; iter++)
	{
		for (size_t i = 3; i < m_vertices.size(); i++)
		{
			auto vertex = getVertex({ i });
			auto current = getLeaving(vertex.handle());
			bool isBoundary = false;
			Point weightedNeighbors{ 0.0, 0.0 };
			double weightSum = 0.0;
			do
			{
				if (current.adjacentFace().handle() == INVALID_FACE_HANDLE || current.twin().adjacentFace().handle() == INVALID_FACE_HANDLE)
				{
					isBoundary = true;
					break;
				}
				auto neighbor = current.twin().origin().point();
				double weight = 1.0;
				weightedNeighbors += *neighbor * weight;
				weightSum += weight;
				current = current.twin().next();
			} while (current.handle() != getLeaving(vertex.handle()).handle());
			if (isBoundary == false)
				*(vertex.point()) = (weightedNeighbors + *(vertex.point())) / (weightSum + 1.0);
		}
	}
}


inline Triangulation::VertexAccessor Triangulation::pushVertex()
{
	m_vertices.pushBack({});
	m_vertices.back().reset();
	return { this, VertexHandle{m_vertices.size() - 1} };
}

inline Triangulation::HalfEdgeAccessor Triangulation::pushHalfEdge()
{
	size_t idx = m_freeHalfEdges.front();
	m_freeHalfEdges.popFront();
	m_halfEdges[idx].reset();
	return { this, HalfEdgeHandle{idx} };
}

inline Triangulation::FaceAccessor Triangulation::pushFace()
{
	size_t idx = m_freeFaces.front();
	m_freeFaces.popFront();
	m_activeFaces.pushBack(idx);
	m_faces[idx].reset();
	return { this, FaceHandle{idx} };
}

inline void Triangulation::removeWholeEdge(HalfEdgeHandle halfEdge)
{
	// access to half-edge and twin
	HalfEdgeAccessor he = { this, halfEdge };
	HalfEdgeAccessor twin = he.twin();
	assert(twin.handle() != INVALID_HALFEDGE_HANDLE);
	assert(twin.twin().handle() != INVALID_HALFEDGE_HANDLE);
	assert(twin.twin().handle() == halfEdge);
	// add indices to free list
	m_freeHalfEdges.pushFront(he.handle().idx);
	m_freeHalfEdges.pushFront(twin.handle().idx);
	// accessors to origins
	VertexAccessor v1 = he.origin();
	VertexAccessor v2 = twin.origin();
	if (v1.leaving().handle() == he.handle())
		v1.setLeaving(INVALID_HALFEDGE_HANDLE);
	if (v2.leaving().handle() == twin.handle())
		v2.setLeaving(INVALID_HALFEDGE_HANDLE);
	//reset
	m_halfEdges[he.handle().idx].reset();
	m_halfEdges[twin.handle().idx].reset();
}




inline void Triangulation::removeFace(size_t activeFaceIdx)
{
	m_faces[activeFaceIdx].reset();
	m_freeFaces.pushFront(activeFaceIdx);
}

inline void Triangulation::removeFaces(const List<size_t>& faceIndices)
{
	if (faceIndices.empty())
		return;
	auto it = m_activeFaces.begin();
	while (it != m_activeFaces.end())
	{
		// check if face should be removed
		bool wasRemoved = false;
		for (auto toRemoveIdx : faceIndices)
		{
			if (toRemoveIdx == *it)
			{
				m_faces[*it].reset();
				m_freeFaces.pushFront(*it);
				it = m_activeFaces.erase(it);
				wasRemoved = true;
				break;
			}
		}
		if (!wasRemoved)
			++it;
	}
}

inline void Triangulation::makeCompact()
{
	Array<Vertex> newVertices;
	Array<HalfEdge> newHalfEdges;
	Array<Face> newFaces;
	newVertices.reserve(m_vertices.size() - 3);
	newHalfEdges.reserve(m_activeFaces.size() * 3);
	newFaces.reserve(m_activeFaces.size());
	// OLD TO NEW id maps
	Array<size_t> vertexMap(m_vertices.size(), INVALID_IDX);
	Array<size_t> halfEdgeMap(m_halfEdges.size(), INVALID_IDX);
	Array<bool> isHalfEdgeLive(m_halfEdges.size(), false); // mask
	Array<size_t> faceMap(m_faces.size(), INVALID_IDX);
	// map vertices (skip 3 super vertices)
	for (size_t oldIdx = 3; oldIdx < m_vertices.size(); oldIdx++)
	{
		assert(m_vertices[oldIdx].point != nullptr);
		size_t newIdx = newVertices.size();
		vertexMap[oldIdx] = newIdx;
		newVertices.pushBack(m_vertices[oldIdx]);
	}
	// map faces and mark live half-edges
	for (const auto& oldIdx : m_activeFaces)
	{
		size_t newIdx = newFaces.size();
		faceMap[oldIdx] = newIdx;
		newFaces.pushBack(m_faces[oldIdx]);
		// map half-edges of a face
		auto he = getFace({ oldIdx }).adjacentHalfEdge();
		for (int i = 0; i < 3; i++)
		{
			// mar half-edge and its twin as live (for exterior boundary halfEdges)
			isHalfEdgeLive[he.handle().idx] = true;
			isHalfEdgeLive[he.twin().handle().idx] = true;
			he = he.next();
		}
	}
	// map half-edges
	for (size_t oldIdx = 0; oldIdx < isHalfEdgeLive.size(); oldIdx++)
	{
		if (isHalfEdgeLive[oldIdx])
		{
			size_t newIdx = newHalfEdges.size();
			halfEdgeMap[oldIdx] = newIdx;
			newHalfEdges.pushBack(m_halfEdges[oldIdx]);
		}
	}
	// helper function for remmaping
	auto remap = [](size_t oldIdx, const Array<size_t>& map) {
		if(oldIdx == INVALID_IDX) return INVALID_IDX;
		return map[oldIdx];
		};
	// reindex half-edges
	for (auto& he : newHalfEdges)
	{
		he.origin = remap(he.origin, vertexMap);
		he.twin = remap(he.twin, halfEdgeMap);
		he.next = remap(he.next, halfEdgeMap);
		he.adjacentFace = remap(he.adjacentFace, faceMap);
	}
	// reindex vertices
	for (auto& v : newVertices)
		v.leaving = remap(v.leaving, halfEdgeMap);
	// reindex faces
	for (auto& f : newFaces)
		f.adjacentHalfEdge = remap(f.adjacentHalfEdge, halfEdgeMap);
	// replace old data
	m_vertices = std::move(newVertices);
	m_halfEdges = std::move(newHalfEdges);
	m_faces = std::move(newFaces);
	// clean up free/active lists
	m_activeFaces.clear();
	m_freeFaces.clear();
	m_freeHalfEdges.clear();
}


const Array<Point>& Triangulation::getTrianglePoints() const
{
	return m_trianglePoints;
}

inline const Point& Triangulation::getVertexPoint(size_t i) const
{
	return *(m_vertices[i].point);
}

inline int Triangulation::getVertexBoundaryId(size_t i) const
{
	return m_vertices[i].boundaryId;
}

inline size_t Triangulation::vertexCount() const
{
	return m_vertices.size();
}

inline StaticArray<size_t, 3> Triangulation::getTriangleVertexIndices(size_t i) const
{
	StaticArray<size_t, 3> vertexIndices;
	const size_t he0Idx = m_faces[i].adjacentHalfEdge;
	const size_t he1Idx = m_halfEdges[he0Idx].next;
	const size_t he2Idx = m_halfEdges[he1Idx].next;
	vertexIndices[0] = m_halfEdges[he0Idx].origin;
	vertexIndices[1] = m_halfEdges[he1Idx].origin;
	vertexIndices[2] = m_halfEdges[he2Idx].origin;
	return vertexIndices;
}

inline size_t Triangulation::getTriangleCount() const
{
	return m_faces.size();
}

bool isInCircumcircle(Triangulation::FaceAccessor face, const Point& point)
{
	const Point& a = *(face.adjacentHalfEdge().origin().point());
	const Point& b = *(face.adjacentHalfEdge().next().origin().point());
	const Point& c = *(face.adjacentHalfEdge().next().next().origin().point());
	// the point p is inside the circumcircle of abc (CCW) if the following determinant is positive:
	// | ax-px  ay-py  (ax-px)^2+(ay-py)^2 |
	// | bx-px  by-py  (bx-px)^2+(by-py)^2 | > 0
	// | cx-px  cy-py  (cx-px)^2+(cy-py)^2 |

	double ax = a[0]; double ay = a[1];
	double bx = b[0]; double by = b[1];
	double cx = c[0]; double cy = c[1];
	double px = point[0]; double py = point[1];

	constexpr double epsilon = 1e-12;

	double det_val = (ax - px) * ((by - py) * (std::pow(cx - px, 2) + std::pow(cy - py, 2)) - (cy - py) * (std::pow(bx - px, 2) + std::pow(by - py, 2))) -
		(ay - py) * ((bx - px) * (std::pow(cx - px, 2) + std::pow(cy - py, 2)) - (cx - px) * (std::pow(bx - px, 2) + std::pow(by - py, 2))) +
		(std::pow(ax - px, 2) + std::pow(ay - py, 2)) * ((bx - px) * (cy - py) - (cx - px) * (by - py));
	return det_val > -epsilon ? true : false;
}
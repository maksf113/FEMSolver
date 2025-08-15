#pragma once
#include "data_structures/StaticArray.hpp"
#include "geometry/Point.hpp"
#include "math/Polynomial.hpp"
#include "math/Vector.hpp"
#include "math/Matrix.hpp"
#include "FiniteElement.hpp"

// forward declaration
template <typename T, int N_NODES>
class Mesh;

template <typename T, int N_NODES>
class ReferenceElement
{
public:
	struct Mapping
	{
		Mat2 JinvT; // jacobian tranpose inverse
		double absDetJ; // absolute value of the determinant of  the jacobian
	};
private:
	StaticArray<Point, N_NODES> m_positions;
	StaticArray<Polynomial<Polynomial<T>>, N_NODES> m_shapeFunctions;
	StaticArray<Vector<Polynomial<Polynomial<T>>, 2>, N_NODES> m_gradients;
public:
	ReferenceElement();
	~ReferenceElement() = default;
	ReferenceElement(const ReferenceElement&) = delete;
	ReferenceElement(ReferenceElement&&) = delete;
	ReferenceElement& operator=(const ReferenceElement&) = delete;
	ReferenceElement& operator=(ReferenceElement&&) = delete;

	const auto& shapeFunctions() const;
	const auto& gradients() const;
	Mapping mapping(const FiniteElement<N_NODES>& element, const Mesh<T, N_NODES>& mesh) const;
};

// linear element
template<typename T, int N_NODES>
inline ReferenceElement<T, N_NODES>::ReferenceElement()
{
	if constexpr (N_NODES == 3)
	{
		// positions
		m_positions[0] = Point{ 0.0, 0.0 };
		m_positions[1] = Point{ 1.0, 0.0 };
		m_positions[2] = Point{ 0.0, 1.0 };

		// hleper inner polynomials of x
		Polynomial<T> zero{ T{0.0} };
		Polynomial<T> x{ T{0.0}, T{1.0} };
		Polynomial<T> minusX{ T{0.0}, T{-1.0} };
		Polynomial<T> one{ T{1.0} };
		Polynomial<T> oneMinusX{ T{1.0}, T{-1.0} };
		// corresponding shape functions
		for (auto& n : m_shapeFunctions)
		{
			n.resizeCoefficients(2);
		}
		// n0 = 1-x-y
		m_shapeFunctions[0][0] = oneMinusX;
		m_shapeFunctions[0][1] = -one;
		// n1 = x
		m_shapeFunctions[1][0] = x;
		m_shapeFunctions[1][1] = zero;
		// n2 = y
		m_shapeFunctions[2][0] = zero;
		m_shapeFunctions[2][1] = one;
		// gradients
		for (int i = 0; i < 3; i++)
		{
			m_gradients[i] = gradient(m_shapeFunctions[i]);
		}
	}
	else
	{
		static_assert(false, "Unsuppeorted element type (incorrect N_NODES in ReferenceElement)\n");
	}
}

template<typename T, int N_NODES>
inline const auto& ReferenceElement<T, N_NODES>::shapeFunctions() const
{
	return m_shapeFunctions;
}

template<typename T, int N_NODES>
inline const auto& ReferenceElement<T, N_NODES>::gradients() const
{
	return m_gradients;
}

template<typename T, int N_NODES>
inline typename ReferenceElement<T, N_NODES>::Mapping ReferenceElement<T, N_NODES>::mapping(const FiniteElement<N_NODES>& element, const Mesh<T, N_NODES>& mesh) const
{
	if constexpr (N_NODES == 3)
	{
		const Point& p0 = mesh.node(element.nodeIdx(0)).position();
		const Point& p1 = mesh.node(element.nodeIdx(1)).position();
		const Point& p2 = mesh.node(element.nodeIdx(2)).position();
		Mat2 J(p1 - p0, p2 - p0);
		return { transpose(inverse(J)), std::abs(det(J)) };
	}
	else
	{
		static_assert(false, "Unsuppeorted element type (incorrect N_NODES in ReferenceElement mapping)\n");
	}
	return Mapping{};
}

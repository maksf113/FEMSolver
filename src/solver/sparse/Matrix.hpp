#pragma once
#include <functional>
#include <utility>
#include "data_structures/Array.hpp"
#include "data_structures/StaticArray.hpp"
#include "Row.hpp"
#include "Vector.hpp"
#include "solver/Mesh.hpp"
#include "solver/MaterialManager.hpp"
#include "solver/BoundaryConditionManager.hpp"
#include "math/Matrix.hpp"
#include "math/Polynomial.hpp"

namespace sparse
{
template<typename T>
class Matrix
{
private:
	Array<Row<T>> m_rows;
public:
	Matrix() = default;
	template<int N_NODES>
	Matrix(const Mesh<T, N_NODES>& mesh, const MaterialManager<T>& materialManager, 
		const BoundaryConditionManager<T>& bcManager, Vector<T>& rhs, const std::function<T(const Point&)>& sourceTerm);
	Matrix(const Matrix& other);
	Matrix(Matrix&& other) noexcept;
	~Matrix() = default;
	Matrix& operator=(const Matrix& other);
	Matrix& operator=(Matrix&& other) noexcept;
	template<int N_NODES>
	void assemble(const Mesh<T, N_NODES>& mesh, const MaterialManager<T>& materialManager,
		const BoundaryConditionManager<T>& bcManager, Vector<T>& rhs, const std::function<T(const Point&)>& sourceTerm);
	const Row<T>& operator[](size_t i) const;
	T getValue(size_t row, size_t col) const;
	size_t rows() const;
	size_t cols() const;
	void zeroColumn(size_t col);
	void setRowIdentity(size_t row); // 0 the row and set 1  on diagonal
	void print() const;
};
template<typename T>
template<int N_NODES>
inline Matrix<T>::Matrix(const Mesh<T, N_NODES>& mesh, const MaterialManager<T>& materialManager, 
	const BoundaryConditionManager<T>& bcManager, Vector<T>& rhs, 
	const std::function<T(const Point&)>& sourceTerm)
{
	assemble(mesh, materialManager, bcManager, rhs, sourceTerm);
}
template<typename T>
inline Matrix<T>::Matrix(const Matrix& other) : m_rows(other.m_rows) {}

template<typename T>
inline Matrix<T>::Matrix(Matrix&& other) noexcept : m_rows(std::move(other.m_rows)) {}

template<typename T>
inline Matrix<T>& Matrix<T>::operator=(const Matrix& other)
{
	if(this!= &other)
	{
		m_rows = other.m_rows;
	}
	return *this;
}

template<typename T>
inline Matrix<T>& Matrix<T>::operator=(Matrix&& other) noexcept
{
	if(this != &other)
	{
		m_rows = std::move(other.m_rows);
	}
	return *this;
}

template<typename T>
template<int N_NODES>
inline void Matrix<T>::assemble(const Mesh<T, N_NODES>& mesh, 
	const MaterialManager<T>& materialManager, 
	const BoundaryConditionManager<T>& bcManager, 
	Vector<T>& rhs, const std::function<T(const Point&)>& sourceTerm)
{
	size_t nodeCount = mesh.nodeCount();
	rhs = Vector<T>(nodeCount);
	m_rows.resize(nodeCount);
	const auto& refElement = mesh.referenceElement();
	const auto& shapeFunctions = refElement.shapeFunctions();
	const auto& refGradients = refElement.gradients();
	for (const auto& elem : mesh)
	{
		const auto& mapping = refElement.mapping(elem, mesh);
		// transform gradients
		StaticArray<::Vector<Polynomial<Polynomial<T>>, 2>, N_NODES> gradients;
		for (int i = 0; i < N_NODES; i++)
		{
			gradients[i] = mapping.JinvT * refGradients[i];
		}
		// diffusion coefficient
		T diffCoeff = materialManager.getMaterial(elem.materialIdx()).diffusionCoeff;
		// source term interpolation plynomial
		Polynomial<Polynomial<T>> sourcePolynomial;
		for (int i = 0; i < N_NODES; i++)
			sourcePolynomial += shapeFunctions[i] * sourceTerm(mesh.node(elem.nodeIdx(i)).position());

		for (int i = 0; i < N_NODES; i++)
		{
			size_t rowIdx = elem.nodeIdx(i);
			// stiffness matrix
			for (int j = 0; j < N_NODES; j++)
			{
				size_t colIdx = elem.nodeIdx(j);
				T val = integral(diffCoeff * dot(gradients[i], gradients[j]) * mapping.absDetJ);
				m_rows[rowIdx].insert({ val, colIdx });
			}
			// rhs vector
			rhs[rowIdx] += integral(sourcePolynomial * shapeFunctions[i] * mapping.absDetJ);
		}
	}
}

template<typename T>
inline const Row<T>& Matrix<T>::operator[](size_t i) const
{
	return m_rows[i];
}

template<typename T>
inline void Matrix<T>::zeroColumn(size_t col)
{
	for (auto& row : m_rows)
	{
		row.set({ T{}, col });
	}
}

template<typename T>
inline size_t Matrix<T>::rows() const
{
	return m_rows.size();
}

template<typename T>
inline size_t Matrix<T>::cols() const
{
	size_t cols = 0;
	for (const auto& row : m_rows)
	{
		if (row.size() > cols)
		{
			cols = row.size();
		}
	}
	return cols;
}

template<typename T>
inline void Matrix<T>::setRowIdentity(size_t row)
{
	m_rows[row].clear();
	m_rows[row].set({T{1}, row});
}

template<typename T>
inline void Matrix<T>::print() const
{
	for (size_t i = 0; i < rows(); i++)
	{
		std::cout << "Row " << i << ":\n";
		for (const auto& elem : m_rows[i])
			std::cout << "(" << elem.val() << ", " << elem.col() << ") ";
		std::cout << "\n";
	}
}

template<typename T>
T Matrix<T>::getValue(size_t row, size_t col) const
{
	return m_rows[row][col];
}
// non-member
template <typename T>
Vector<T> operator*(const Matrix<T>& A, const Vector<T>& v)
{
	size_t rowCount = A.rows();
	Vector<T> result(rowCount);
	for (size_t i = 0; i < rowCount; i++)
	{
		result[i] = A[i] * v;
	}
	return result;
}

}
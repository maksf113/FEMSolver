#pragma once
#include "Vector.hpp"

template <typename T, size_t N, size_t M>
class Matrix;

using Mat2 = Matrix<double, 2, 2>;

template <typename T, size_t N, size_t M>
Matrix<T, M, N> transpose(const Matrix<T, N, M>& A);

template <typename T, size_t N, size_t M>
class Matrix : public Vector<Vector<T, M>, N>
{
	using Base = Vector<Vector<T, M>, N>;
public:
	using Base::Base;

	Matrix(const Vector<T, 2>& u, const Vector<T, 2>& v);

	constexpr size_t rows() const;
	constexpr size_t cols() const;

	T& operator()(size_t row, size_t col);
	const T& operator()(size_t row, size_t col) const;

	Vector<T, N> getColumn(size_t colIdx) const;

	static Matrix<T, N, M> identity();
};

template<typename T, size_t N, size_t M>
inline Matrix<T, N, M>::Matrix(const Vector<T, 2>& u, const Vector<T, 2>& v)
{
	if constexpr (N == 2 && M == 2)
	{
		(*this)[0][0] = u[0]; (*this)[0][1] = v[0];
		(*this)[1][0] = u[1]; (*this)[1][1] = v[1];
	}
	else
	{
		static_assert(false, "Inapropriate use of Matrix 2x2 constructor\n");
	}
}

template<typename T, size_t N, size_t M>
inline constexpr size_t Matrix<T, N, M>::rows() const
{
	return N;
}

template<typename T, size_t N, size_t M>
inline constexpr size_t Matrix<T, N, M>::cols() const
{
	return M;
}

template<typename T, size_t N, size_t M>
inline T& Matrix<T, N, M>::operator()(size_t row, size_t col)
{
	return (*this)[row][col];
}

template<typename T, size_t N, size_t M>
inline const T& Matrix<T, N, M>::operator()(size_t row, size_t col) const
{
	return (*this)[row][col];
}

template<typename T, size_t N, size_t M>
inline Vector<T, N> Matrix<T, N, M>::getColumn(size_t colIdx) const
{
	Vector<T, N> column;
	for (size_t i = 0; i < N; i++)
		column[i] = (*this)[i][colIdx];
	return column;
}

template<typename T, size_t N, size_t M>
inline Matrix<T, N, M> Matrix<T, N, M>::identity()
{
	static_assert(N == M, "Identity matrix must be square");
	Matrix<T, N, M> one;
	for (size_t i = 0; i < N; i++)
		one(i, i) = static_cast<T>(1);
	return one;
}

// Matrix - Vector multiplication
template <typename T, size_t N, size_t M>
inline Vector<T, N> operator*(const Matrix<T, N, M>& A, const Vector<T, M>& v)
{
	Vector<T, N> result;
	for (size_t i = 0; i < N; i++)
		result[i] = dot(A[i], v);
	return result;
}

// Matrix - Vector of different type multiplication
template <typename T, typename S, size_t N, size_t M>
inline Vector<S, N> operator*(const Matrix<T, N, M>& A, const Vector<S, M>& v)
{
	Vector<S, N> result;
	for (size_t i = 0; i < N; i++)
	{
		for (size_t j = 0; j < M; j++)
		{
			result[i] += A[i][j] * v[j];
		}
	}
	return result;
}

// Matrix - Matrix multiplication
template <typename T, size_t N, size_t M, size_t K>
inline Matrix<T, N, M> operator*(const Matrix<T, N, K>& A, const Matrix<T, K, M>& B)
{
	Matrix<T, N, M> result;
	for (size_t i = 0; i < N; i++)
		for (size_t j = 0; j < M; j++)
			result(i, j) = dot(A[i], B.getColumn(j));
	return result;
}

// transpose
template <typename T, size_t N, size_t M>
Matrix<T, M, N> transpose(const Matrix<T, N, M>& A)
{
	Matrix<T, M, N> result;
	for (size_t i = 0; i < M; i++)
		for (size_t j = 0; j < N; j++)
			result(i, j) = A(j, i);
	return result;
}

// 2x2 determinant

template <typename T>
T det(const Matrix<T, 2, 2>& A)
{
	return A(0, 0) * A(1, 1) - A(0, 1) * A(1, 0);
}

template <typename T>
Matrix<T, 2, 2> inverse(const Matrix<T, 2, 2>& A)
{
	T detInv = static_cast<T>(1) / det(A);
	Matrix<T, 2, 2> result;
	result(0, 0) =  A(1, 1) * detInv;
	result(0, 1) = -A(0, 1) * detInv;
	result(1, 0) = -A(1, 0) * detInv;
	result(1, 1) =  A(0, 0) * detInv;
	return result;
}
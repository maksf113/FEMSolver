#pragma once
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include "data_structures/Array.hpp"
#include "Vector.hpp"

template<typename T>
class Polynomial
{
private:
	Array<T> m_coeffs;
public:
	Polynomial(size_t n = 0);
	Polynomial(size_t n, const T& a);
	Polynomial(const Polynomial<T>& other);
	Polynomial(Polynomial<T>&& other) noexcept;
	Polynomial(std::initializer_list<T> coeffs);
	~Polynomial() = default;
	Polynomial<T>& operator=(const Polynomial<T>& other);
	Polynomial<T>& operator=(Polynomial<T>&& other) noexcept;
	bool operator==(const Polynomial<T>& other) const;
	bool operator!=(const Polynomial<T>& other) const;
	int degree() const;
	const T& operator[](size_t i) const;
	T& operator[](size_t i);
	Polynomial<T>& operator+=(const Polynomial<T>& other);
	Polynomial<T>& operator-=(const Polynomial<T>& other);
	template<typename S>
	Polynomial<T>& operator*=(const S& a);
	template<typename S>
	Polynomial<T>& operator/=(const S& a);
	Polynomial<T>& operator*=(const Polynomial<T>& other);
	T operator()(const T& x) const;
	template<typename S>
	S operator()(const S& x, const S& y) const;
	template<typename S>
	S operator()(const S& x, const S& y, const S& z) const;
	// indefinite integral with 0 constant
	Polynomial<T> indefiniteIntegral() const;
	Polynomial<T> derivative() const;
	void normalize(); // trim 0s
	void resizeCoefficients(size_t n);
};

// default rule
template<typename T>
struct is_polynomial : std::false_type {};
// specific for polynomials
template<typename T>
struct is_polynomial<Polynomial<T>> : std::true_type {};
// helper variable (variable template)
template<typename T>
inline constexpr bool is_polynomial_v = is_polynomial<T>::value;

// trait to recursively get scalar type
// default
template<typename T>
struct get_scalar_type
{
	using type = T;
};
// for polynomial
template<typename U>
struct get_scalar_type<Polynomial<U>>
{
	using type = typename get_scalar_type<U>::type;
};
// helper alias template
template<typename T>
using get_scalar_type_t = typename get_scalar_type<T>::type;

template<typename T>
inline Polynomial<T>::Polynomial(size_t n) : m_coeffs(n) {}

template<typename T>
inline Polynomial<T>::Polynomial(size_t n, const T& a) : m_coeffs(n, a) {}

template<typename T>
inline Polynomial<T>::Polynomial(const Polynomial<T>& other) : m_coeffs(other.m_coeffs) {}

template<typename T>
inline Polynomial<T>::Polynomial(Polynomial<T>&& other) noexcept : m_coeffs(std::move(other.m_coeffs)) {}

template<typename T>
inline Polynomial<T>::Polynomial(std::initializer_list<T> coeffs) : m_coeffs(coeffs) {}

template<typename T>
inline Polynomial<T>& Polynomial<T>::operator=(const Polynomial<T>& other)
{
	m_coeffs = other.m_coeffs;
	return *this;
}

template<typename T>
inline Polynomial<T>& Polynomial<T>::operator=(Polynomial<T>&& other) noexcept
{
	m_coeffs = std::move(other.m_coeffs);
	return *this;
}

template<typename T>
inline bool Polynomial<T>::operator==(const Polynomial<T>& other) const
{
	int deg = degree();
	int otherDeg = other.degree();
	if (deg != otherDeg)
		return false;
	if (deg == -1)
		return true;
	for (int i = 0; i <= deg; i++)
	{
		if ((*this)[i] != other[i])
			return false;
	}
	return true;
}

template<typename T>
inline bool Polynomial<T>::operator!=(const Polynomial<T>& other) const
{
	return !(*this == other);
}

template<typename T>
inline int Polynomial<T>::degree() const
{
	for (int i = m_coeffs.size() - 1; i >= 0; i--)
	{
		if (m_coeffs[i] != T{})
			return i;
	}
	return -1; // the 0 polynomial
}

template<typename T>
inline const T& Polynomial<T>::operator[](size_t i) const
{
	return m_coeffs[i];
}

template<typename T>
inline T& Polynomial<T>::operator[](size_t i)
{
	return m_coeffs[i];
}

template<typename T>
inline Polynomial<T>& Polynomial<T>::operator+=(const Polynomial<T>& other)
{
	if (other.m_coeffs.size() > m_coeffs.size())
		m_coeffs.resize(other.m_coeffs.size());
	for (size_t i = 0; i < other.m_coeffs.size(); i++)
		m_coeffs[i] += other[i];
	normalize();
	return *this;
}

template<typename T>
inline Polynomial<T>& Polynomial<T>::operator-=(const Polynomial<T>& other)
{
	if (other.m_coeffs.size() > m_coeffs.size())
		m_coeffs.resize(other.m_coeffs.size());
	for (size_t i = 0; i < other.m_coeffs.size(); i++)
		m_coeffs[i] -= other[i];
	normalize();
	return *this;
}

template<typename T>
template<typename S>
inline Polynomial<T>& Polynomial<T>::operator*=(const S& a)
{
	for (auto& coeff : m_coeffs)
		coeff *= a;
	return *this;
}

template<typename T>
template<typename S>
inline Polynomial<T>& Polynomial<T>::operator/=(const S& a)
{
	for (auto& coeff : m_coeffs)
		coeff /= a;
	return *this;
}

template<typename T>
inline Polynomial<T>& Polynomial<T>::operator*=(const Polynomial<T>& other)
{
	int thisDeg = degree();
	int otherDeg = other.degree();
	if (thisDeg == -1 || otherDeg == -1)
	{
		m_coeffs.resize(0);
		return *this;
	}
	Array<T> newCoeffs(thisDeg + otherDeg + 1);
	for (int i = 0; i <= thisDeg; i++)
		for (int j = 0; j <= otherDeg; j++)
			newCoeffs[i + j] += (*this)[i] * other[j];
	m_coeffs = std::move(newCoeffs);
	normalize();
	return *this;
}

template<typename T>
inline T Polynomial<T>::operator()(const T& x) const
{
	int deg = degree();
	if (deg == -1)
		return T{};
	T result{};
	for (int i = deg; i >= 0; i--) // int or int (size_t loops around 0)
	{
		result *= x;
		result += m_coeffs[i];
	}
	return result;
}

template<typename T>
template<typename S>
inline S Polynomial<T>::operator()(const S& x, const S& y) const
{
	return (*this)(y)(x);
}

template<typename T>
template<typename S>
inline S Polynomial<T>::operator()(const S& x, const S& y, const S& z) const
{
	return (*this)(z)(y)(x);
}

template<typename T>
inline Polynomial<T> Polynomial<T>::indefiniteIntegral() const
{
	int deg = degree();
	if (deg == -1)
		return Polynomial<T>(0);
	Polynomial<T> result(deg + 2);
	for (size_t i = 0; i <= deg; i++)
	{
		if constexpr (is_polynomial_v<T>)
		{
			using scalar_t = get_scalar_type_t<T>;
			result[i + 1] = (*this)[i] / static_cast<scalar_t>(i + 1);
		}
		else
		{
			result[i + 1] = (*this)[i] / static_cast<T>(i + 1);
		}
	}
		
	return result;
}

template<typename T>
inline Polynomial<T> Polynomial<T>::derivative() const
{
	int deg = degree();
	if (deg <= 0)
		return Polynomial<T>(0);
	Polynomial<T> result(deg);
	for (int i = 0; i <= deg - 1; i++)
	{
		if constexpr (is_polynomial_v<T>)
		{
			using scalar_t = get_scalar_type_t<T>;
			result[i] = (*this)[i + 1] * static_cast<scalar_t>(i + 1);
		}
		else
		{
			result[i] = (*this)[i + 1] * static_cast<T>(i + 1);
		}
	}
	return result;
}

template<typename T>
inline void Polynomial<T>::normalize()
{
	int deg = degree();
	if (deg == -1)
	{
		m_coeffs.clear();
	}
	else
	{
		m_coeffs.resize(deg + 1);
	}
}

template<typename T>
inline void Polynomial<T>::resizeCoefficients(size_t n)
{
	assert(n >= m_coeffs.size());
	m_coeffs.resize(n);
}

// non-member operators and functions

template<typename T>
Polynomial<T> operator-(const Polynomial<T>& p)
{
	if (p.degree() == -1) // 0 polynomial
		return p;
	Polynomial<T> result(p.degree() + 1);
	for (size_t i = 0; i <= p.degree(); i++)
		result[i] = -p[i];
	return result;
}

template<typename T>
Polynomial<T> operator+(const Polynomial<T>& p, const Polynomial<T>& q)
{
	Polynomial<T> result(p);
	return result += q;
}

template<typename T>
Polynomial<T> operator-(const Polynomial<T>& p, const Polynomial<T>& q)
{
	Polynomial<T> result(p);
	return result -= q;
}

template<typename T, typename S>
Polynomial<T> operator*(const Polynomial<T>& p, const S& a)
{
	Polynomial<T> result(p);
	return result *= a;
}

template<typename T, typename S>
Polynomial<T> operator*(const S& a, const Polynomial<T>& p)
{
	Polynomial<T> result(p);
	return result *= a;
}

template<typename T, typename S>
Polynomial<T> operator/(const Polynomial<T>& p, const S& a)
{
	Polynomial<T> result(p);
	return result /= a;
}

template<typename T>
Polynomial<T> operator*(const Polynomial<T>& p, const Polynomial<T>& q)
{
	Polynomial<T> result(p);
	return result *= q;
}
//derivative
template<typename T>
inline Polynomial<T> derivative(const Polynomial<T>& p)
{
	return p.derivative();
}
// indefinite integral with cont = 0
template<typename T>
inline Polynomial<T> indefiniteIntegral(const Polynomial<T>& p)
{
	return p.indefiniteIntegral();
}
// integral over unit interval [0, 1]
template<typename T>
inline T integral(const Polynomial<T>& p)
{
	return (indefiniteIntegral(p))(T(1));
}
// integral over reference triangle (1,0), (0,0), (0,1)
template<typename T>
inline T integral(const Polynomial<Polynomial<T>>& p)
{
	Polynomial<T> oneMinusX(2);
	oneMinusX[0] = T(1);
	oneMinusX[1] = T(-1);
	return integral(indefiniteIntegral(p)(oneMinusX));
}
// integration over reference tetrahedron (0,0,0), (1,0,0), (0,1,0), (0,0,1)
template<typename T>
inline T integral(const Polynomial<Polynomial<Polynomial<T>>>& p)
{
	Polynomial<T> minus1(1, T(-1));
	Polynomial<T> oneMinusX(2);
	oneMinusX[0] = T(1);
	oneMinusX[1] = T(-1);
	Polynomial<Polynomial<T>> oneMinusXminusY(2, oneMinusX);
	oneMinusXminusY[1] = minus1;
	return integral(indefiniteIntegral(p)(oneMinusXminusY));
}

// --- GRADIENTS ---

// 1D gradient
template<typename T, typename = std::enable_if_t<!is_polynomial_v<T>>>
auto gradient(const Polynomial<T>& p) -> Vector<Polynomial<T>, 1>
{
	using PolyT = Polynomial<T>;
	Vector<PolyT, 1> grad;
	grad[0] = derivative(p);
	return grad;
}

// 2D gradient
template<typename T, typename = std::enable_if_t<!is_polynomial_v<T>>>
auto gradient(const Polynomial<Polynomial<T>>& p) -> Vector<Polynomial<Polynomial<T>>, 2>
{
	using PolyT = Polynomial<T>;
	using Poly2T = Polynomial<PolyT>;

	Vector<Poly2T, 2> grad;
	int deg = p.degree();
	// df/dy
	grad[1] = derivative(p);
	if (deg == -1)
	{
		grad[0] = Poly2T(0);
	}
	else
	{
		// df/dx
		// f(x, y) = sum c_i(x) * y^i
		// df/dx = sum c_i'(x) * y^i
		Poly2T df_dx(deg + 1);
		for (int i = 0; i <= deg; i++)
		{
			df_dx[i] = derivative(p[i]);
		}
		df_dx.normalize();
		grad[0] = df_dx;
	}
	return grad;
}

// 3D gradient
template<typename T, typename = std::enable_if_t<!is_polynomial_v<T>>>
auto gradient(const Polynomial< Polynomial< Polynomial<T>>>& p)
-> Vector<Polynomial<Polynomial<Polynomial<T>>>, 3>
{
	using PolyT = Polynomial<T>;
	using Poly2T = Polynomial<PolyT>;
	using Poly3T = Polynomial<Poly2T>;
	Vector<Poly3T, 3> grad;
	grad[2] = derivative(p);
	int deg = p.degree();
	if (deg == -1)
	{
		grad[0] = Poly3T(0);
		grad[1] = Poly3T(0);
	}
	else
	{
		// f(x, y, z) = sum c_i(x, y) * z^i
		// df/dx = sum dc_i(x, y)/dx * z^i
		// df/dy = sum dc_i(x, y)/dy * z^i
		Poly3T df_dx(deg + 1);
		Poly3T df_dy(deg + 1);
		for (int i = 0; i <= deg; i++)
		{
			auto coeffGrad = gradient(p[i]);
			df_dx[i] = coeffGrad[0];
			df_dy[i] = coeffGrad[1];
		}
		df_dx.normalize();
		df_dy.normalize();
		grad[0] = df_dx;
		grad[1] = df_dy;
	}
	return grad;
}
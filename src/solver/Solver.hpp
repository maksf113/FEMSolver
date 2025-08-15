#pragma once
#include <limits>
#include "Mesh.hpp"
#include "MaterialManager.hpp"
#include "BoundaryConditionManager.hpp"
#include "sparse/Matrix.hpp"
#include "sparse/Vector.hpp"
#include "geometry/Triangulation.hpp"
#include "geometry/Point.hpp"

class Solver
{
	using Matrix = sparse::Matrix<double>;
	using Vector = sparse::Vector<double>;
private:
	Mesh<double, 3> m_mesh;
	MaterialManager<double> m_materialManager;
	BoundaryConditionManager<double> m_bcManager;
	Matrix m_systemMatrix;
	Vector m_solution;
	Vector m_rhs;
public:
	Solver(const Triangulation& triangulation);
	~Solver() = default;
	Solver(const Solver&) = delete;
	Solver(Solver&&) = delete;
	Solver& operator=(const Solver&) = delete;
	Solver& operator=(Solver&&) = delete;
	void conjugateGradient();
	void getVertices(Array<Point>& vertices) const;
	void getIndices(Array<uint32_t>& indices) const;
	void getSolution(Array<double>& solution) const;
private:
	void applyDirichletBC();
};

Solver::Solver(const Triangulation& triangulation) : m_mesh(triangulation)
{
	m_materialManager.addMaterial({ 1.0 });
	//m_bcManager.addBC({ [](const Point& p) { return -0.5 * std::sin(3.0 * p[0]); } }); // 0 boundary id (outer)
	//m_bcManager.addBC({ [](const Point& p) { return -1.0; } }); // 1 boundary id (inner)
	//m_bcManager.addBC({ [](const Point& p) { return 1.0; } }); // 2 boundary id (inner)

	// Outer boundary (ID 0): value depends on angle
	m_bcManager.addBC({ [](const Point& p) {
		return 5.0 * atan2(p[1], p[0]);
	} });

	// Inner hole 1 (ID 1)
	m_bcManager.addBC({ [](const Point& p) {
		return 20.0;
	} });

	// Inner hole 2 (ID 2)
	m_bcManager.addBC({ [](const Point& p) {
		return -5.0;
	} });

	// source term (rhs of PDE)
	std::function<double(const Point& p)> source = [](const Point& p) {return 0.0; };
	m_systemMatrix.assemble(m_mesh, m_materialManager, m_bcManager, m_rhs, source);
	//m_systemMatrix.print();
	applyDirichletBC();
	conjugateGradient();
}

inline void Solver::conjugateGradient()
{
	const size_t nodeCount = m_mesh.nodeCount();
	m_solution.resize(nodeCount);
	// residual
	sparse::Vector<double> r = m_rhs - m_systemMatrix * m_solution;
	sparse::Vector<double> prevR(r.dim());
	// searchh direction 
	sparse::Vector<double> p = r;
	double rNormSq = dot(r, r);
	double rNormSqInitial = rNormSq;
	std::cout << "Initial residual magnitude squared = " << rNormSq << "\n";
	if (std::sqrt(rNormSq) < 1e-24)
	{
		std::cout << "Inintial guess is alraedy the solution\n";
		return;
	}
	// counter to prevent infinite loop
	int iterations = 0;
	// step size
	double alpha = 0.0;
	// improvement factor
	double beta = 0.0;
	// A*p product
	sparse::Vector<double> Ap(m_rhs.dim());
	const double toleranceSq = 1e-12;
	const double absToleranceSq = 1e-12;
	const double relToleranceSq = 1e-12 * normSq(m_rhs);
	const size_t maxIterations = 10000;

	for(size_t k = 0; k < maxIterations; k++)
	{
		// precomputin A*p product
		Ap = m_systemMatrix * p;
		// step size
		alpha = dot(r, r) / (dot(p, Ap));
		// update solution
		m_solution = m_solution + alpha * p;
		// update residual
		prevR = r;
		r = r - alpha * (Ap);
		rNormSq = dot(r, r);
		if (std::sqrt(rNormSq) < absToleranceSq + relToleranceSq)
		{
			std::cout << "CG converged in " << k << " iterations\n";
			return;
		}
		// improvement factor
		beta = rNormSq / dot(prevR, prevR);
		// update search direction
		p = r + beta * p;
	}
	std::cout << "CG failed to converge within " << maxIterations << " iterations\n";
}

inline void Solver::getVertices(Array<Point>& vertices) const
{
	size_t nodeCount = m_mesh.nodeCount();
	vertices.resize(nodeCount);
	for (size_t i = 0; i < nodeCount; i++)
	{
		vertices[i] = m_mesh.node(i).position();
	}
}

inline void Solver::getIndices(Array<uint32_t>& indices) const
{
	size_t elementCount = m_mesh.elementCount();
	indices.resize(elementCount * 3);
	for (size_t i = 0; i < elementCount; i++)
	{
		indices[i * 3 + 0] = static_cast<unsigned int>(m_mesh.element(i).nodeIdx(0));
		indices[i * 3 + 1] = static_cast<unsigned int>(m_mesh.element(i).nodeIdx(1));
		indices[i * 3 + 2] = static_cast<unsigned int>(m_mesh.element(i).nodeIdx(2));
	}
}

inline void Solver::getSolution(Array<double>& solution) const
{
	size_t nodeCount = m_mesh.nodeCount();
	solution.resize(nodeCount);
	for (size_t i = 0; i < nodeCount; i++)
	{
		solution[i] = m_solution[i];
	}
}

inline void Solver::applyDirichletBC()
{
	size_t nodeCount = m_mesh.nodeCount();
	// modify RHS
	for (size_t i = 0; i < nodeCount; i++)
	{
		const Node& node = m_mesh.node(i);
		const int boundaryId = node.boundaryId(); // assumming only 1  bc per node
		if (boundaryId > -1)
		{
			// boundary value
			double g_i = m_bcManager.getBC(boundaryId).getValue(node.position());
			// update rhs vector F_j = F_j - K_ij * g_i
			for (size_t j = 0; j < nodeCount; j++)
			{
				if (i == j)
					continue; // boundary row value will be set explicitly
				double K_ji = m_systemMatrix.getValue(j, i);
				if (K_ji != 0.0)
				{
					m_rhs[j] -= K_ji * g_i;
				}
			}
		}
	}
	// modify stiffness matrix and set boundary vzlues in rhs
	for (size_t i = 0; i < nodeCount; i++)
	{
		const Node& node = m_mesh.node(i);
		const int boundaryId = node.boundaryId(); // assumming only 1  bc per node
		if (boundaryId > -1)
		{
			// modify matrix
			m_systemMatrix.zeroColumn(i);
			m_systemMatrix.setRowIdentity(i);
			// boundary value
			double g_i = m_bcManager.getBC(boundaryId).getValue(node.position());
			m_rhs[i] = g_i;
		}
	}
}

#pragma once
#include <string>
#include <iomanip>
#include <memory>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Framebuffer.hpp"
#include "VBO.hpp"
#include "VAO.hpp"
#include "VBL.hpp"
#include "EBO.hpp"
#include "ColorMaps.hpp"
#include "UniformBuffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "TextRenderer.hpp"
#include "Geometry/Point.hpp"
#include "solver/Solver.hpp"

class Renderer : public InputReciever
{
public:
	Renderer(uint32_t width, uint32_t height);
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator()(const Renderer&) = delete;
	Renderer& operator()(Renderer&&) = delete;
	~Renderer() = default;
	void draw();

	void setVertices(const Solver& solver);

	void processInput(const InputManager& im);
	bool mouseWheelEvent(double x, double y);
	bool resizeEvent(uint32_t width, uint32_t height) override;

	Camera& getCamera();
	void createGrid();

	double scaleMin() const;
	double scaleMax() const;
	int labelCount() const;
	int& getLabelCount();
	ColorPalette& getPalette();
	bool& getDrawPlot();
	bool& getDrawWireframe();
	bool& getDrawIsolines();
	bool& getDrawMesh();
	bool& getDrawGrid();
private:
	void setMeshVertices(const Solver& solver);
	void setPlotVertices(const Solver& solver);
	void createColorMapTextures();
	void createBackground();
	
	void drawPlot();
	void drawMesh();
	void drawGrid();
	void drawBackground();
private:
	uint32_t m_width;
	uint32_t m_height;
	std::unique_ptr<Shader> m_shaderPlot;
	std::unique_ptr<Shader> m_shaderMesh;
	std::unique_ptr<Shader> m_shaderGrid;
	std::unique_ptr<Shader> m_shaderBackground;

	std::unique_ptr<Framebuffer> m_framebuffer;
	std::unique_ptr<UniformBuffer> m_uniformBuffer;
	std::unique_ptr<VAO> m_solutionPlotVAO;
	std::unique_ptr<VBO> m_solutionPlotVBO;
	std::unique_ptr<EBO> m_solutionPlotEBO;
	std::unique_ptr<VAO> m_meshVAO;
	std::unique_ptr<VBO> m_meshVBO;
	std::unique_ptr<EBO> m_meshEBO;
	std::unique_ptr<VAO> m_gridVAO;
	std::unique_ptr<VBO> m_gridVBO;
	std::unique_ptr<VAO> m_backgroundVAO;

	std::unique_ptr<TextRenderer> m_textRenderer;

	Map<ColorPalette, std::unique_ptr<Texture>> m_colorMapTextures;

	bool m_drawPlot = true;
	bool m_drawWireframe = false;
	bool m_drawIsolines = true;
	bool m_drawMesh = false;
	bool m_drawGrid = true;
	
	Camera m_camera;

	float m_scaleMin;
	float m_scaleMax;
	float m_solutionScaleFactor;
	int m_labelCount = 5; // scale division
	ColorPalette m_palette = ColorPalette::INFERNO;

	glm::vec3 m_plotMinBounds;
	glm::vec3 m_plotMaxBounds;
};

Renderer::Renderer(uint32_t width, uint32_t height) : m_width(width), m_height(height)
{
	if (glewInit() != GLEW_OK)
		printf("%s error: failed to initialize glew\n", __FUNCTION__);
	int majorVersion, minorVersion;
	GL(glGetIntegerv(GL_MAJOR_VERSION, &majorVersion));
	GL(glGetIntegerv(GL_MINOR_VERSION, &minorVersion));
	//GL(glEnable(GL_CULL_FACE));
	GL(glEnable(GL_DEPTH_TEST));
	GL(glEnable(GL_LINE_SMOOTH));
	GL(glEnable(GL_POINT_SMOOTH));
	GL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));
	GL(glHint(GL_POINT_SMOOTH_HINT, GL_NICEST));
	GL(glPointSize(3));
	m_framebuffer = std::make_unique<Framebuffer>(width, height);
	m_shaderPlot = std::make_unique<Shader>("shaders/plot.vert", "shaders/plot.frag");
	m_shaderMesh = std::make_unique<Shader>("shaders/mesh.vert", "shaders/mesh.frag");
	m_shaderGrid = std::make_unique<Shader>("shaders/grid.vert", "shaders/grid.frag");
	m_shaderBackground = std::make_unique<Shader>("shaders/background.vert", "shaders/background.frag");

	m_shaderPlot->bindUniformBlock("u_data", 0);
	m_shaderMesh->bindUniformBlock("u_data", 0);
	m_shaderGrid->bindUniformBlock("u_data", 0);

	m_uniformBuffer = std::make_unique<UniformBuffer>();
	m_meshVAO = std::make_unique<VAO>();
	m_solutionPlotVAO = std::make_unique<VAO>();
	m_camera.setAspectRatio(width, height);
	m_textRenderer = std::make_unique<TextRenderer>(width, height);
	m_textRenderer->loadFont("fonts/Roboto_Condensed-Black.ttf", 48);
	createColorMapTextures();
	createGrid();
	createBackground();
}

bool Renderer::resizeEvent(uint32_t newWidth, uint32_t newHeight)
{
	if (newWidth == 0 || newHeight == 0)
		return true;
	m_framebuffer->resize(newWidth, newHeight);
	m_width = newWidth;
	m_height = newHeight;
	GL(glViewport(0, 0, newWidth, newHeight));
	m_camera.setAspectRatio(newWidth, newHeight);
	m_textRenderer->setProjection(newWidth, newHeight);
	return true;
}


void Renderer::draw()
{

	m_framebuffer->bind();
	GL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GL(glClearDepth(1.0f));
	GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// background
	drawBackground();
	
	GL(glViewport(0, 0, m_framebuffer->width(), m_framebuffer->height()));

	glm::mat4 view = m_camera.view();
	glm::mat4 projection = m_camera.projection();

	m_uniformBuffer->data(view, projection, m_camera.is2D(), m_labelCount, m_drawIsolines);
	// cartesian grids
	if (m_drawGrid && !m_camera.is2D())
	{
		drawGrid();
	}
	// clear depth buffer so that cartesian grid is always behind the plot
	GL(glClear(GL_DEPTH_BUFFER_BIT));
	// 3D plot
	if (m_drawPlot)
	{
		drawPlot();
	}
	// 2D mesh
	if (m_drawMesh)
	{
		drawMesh();
	}

	m_framebuffer->drawToScreen(m_width, m_height);
}

inline void Renderer::setVertices(const Solver& solver)
{
	setMeshVertices(solver);
	setPlotVertices(solver);
}


inline void Renderer::setMeshVertices(const Solver& solver)
{
	Array<Point> points;
	Array<double> solution;
	Array<uint32_t> indices;
	solver.getVertices(points);
	solver.getIndices(indices);
	size_t vertCount = points.size();
	// mesh
	Array<float> meshVertices(points.size() * 3);
	for (size_t i = 0; i < vertCount; i++)
	{
		// position
		meshVertices[i * 3 + 0] = static_cast<float>(points[i][0]); // x
		meshVertices[i * 3 + 1] = static_cast<float>(points[i][1]); // y
		meshVertices[i * 3 + 2] = 0.0f; // z = 0
	}
	m_meshEBO = std::make_unique<EBO>(indices);
	m_meshVBO = std::make_unique<VBO>(meshVertices);
	m_meshVAO = std::make_unique<VAO>();
	VBL layoutMesh;
	layoutMesh.push<float>(3);
	m_meshVAO->addBuffer(*m_meshVBO, layoutMesh);
	m_meshVAO->bind();
	m_meshEBO->bind();
	m_meshVAO->unbind();
}

inline void Renderer::setPlotVertices(const Solver& solver)
{
	// solution
	Array<Point> points;
	Array<double> solution;
	Array<uint32_t> indices;
	solver.getVertices(points);
	solver.getSolution(solution);
	solver.getIndices(indices);
	size_t vertCount = points.size();
	// 3 for position + 3 for normal + 3 for color + 1 for normalized solution value;
	Array<float> vertices(points.size() * 7);
	double min = std::numeric_limits<double>::max();
	double max = -std::numeric_limits<double>::max();
	for (const auto& val : solution)
	{
		if (val < min)
			min = val;
		if (val > max)
			max = val;
	}
	m_scaleMin = min;
	m_scaleMax = max;
	double minCoord = std::numeric_limits<double>::max();
	double maxCoord = -std::numeric_limits<double>::max();
	for (const auto& p : points)
	{
		if (p[0] < minCoord)
			minCoord = p[0];
		if (p[1] < minCoord)
			minCoord = p[1];
		if (p[0] > maxCoord)
			maxCoord = p[0];
		if (p[1] > maxCoord)
			maxCoord = p[1];
	}
	// plot height scale
	m_solutionScaleFactor = (maxCoord - minCoord) * 0.35f;
	for (size_t i = 0; i < vertCount; i++)
	{
		float normalizedSolution = (solution[i] - min) / (max - min);
		// position
		vertices[i * 7 + 0] = static_cast<float>(points[i][0]); // x
		vertices[i * 7 + 1] = static_cast<float>(points[i][1]); // y
		vertices[i * 7 + 2] = static_cast<float>(m_solutionScaleFactor * normalizedSolution); // z
		//normal initialize to 0
		vertices[i * 7 + 3] = 0.0f;
		vertices[i * 7 + 4] = 0.0f; 
		vertices[i * 7 + 5] = 0.0f;
		// normalized sloution
		vertices[i * 7 + 6] = normalizedSolution;
	}
	// calculate normals
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		// every 3 indices form a triangle
		// accumulate normal of every triangle in corresponding vertices
		size_t i0 = indices[i + 0];
		size_t i1 = indices[i + 1];
		size_t i2 = indices[i + 2];

		Vector<float, 3> v0{
			vertices[i0 * 7 + 0],
			vertices[i0 * 7 + 1],
			vertices[i0 * 7 + 2] };
		Vector<float, 3> v1{
			vertices[i1 * 7 + 0],
			vertices[i1 * 7 + 1],
			vertices[i1 * 7 + 2] };
		Vector<float, 3> v2{
			vertices[i2 * 7 + 0],
			vertices[i2 * 7 + 1],
			vertices[i2 * 7 + 2] };
		Vector<float, 3> normal = cross(v1 - v0, v2 - v0);
		if (normal[2] < 0.0f)
		{
			normal *= -1.0f;
		}
		vertices[i0 * 7 + 3] += normal[0];
		vertices[i0 * 7 + 4] += normal[1];
		vertices[i0 * 7 + 5] += normal[2];

		vertices[i1 * 7 + 3] += normal[0];
		vertices[i1 * 7 + 4] += normal[1];
		vertices[i1 * 7 + 5] += normal[2];

		vertices[i2 * 7 + 3] += normal[0];
		vertices[i2 * 7 + 4] += normal[1];
		vertices[i2 * 7 + 5] += normal[2];

	}
	// normalize accumulated normals
	for (size_t i = 0; i < vertCount; i++)
	{
		float nx = vertices[i * 7 + 3];
		float ny = vertices[i * 7 + 4];
		float nz = vertices[i * 7 + 5];
		float length = std::sqrt(nx * nx + ny * ny + nz * nz);
		vertices[i * 7 + 3]	/= length;
		vertices[i * 7 + 4]	/= length;
		vertices[i * 7 + 5]	/= length;
	}
	// calculate bounding values
	m_plotMinBounds = glm::vec3(std::numeric_limits<float>::max());
	m_plotMaxBounds = glm::vec3(-std::numeric_limits<float>::max());
	for (size_t i = 0; i < vertCount; i++)
	{
		float x = vertices[i * 7 + 0];
		float y = vertices[i * 7 + 1];
		float z = vertices[i * 7 + 2];
		m_plotMinBounds.x = std::min(m_plotMinBounds.x, x);
		m_plotMinBounds.y = std::min(m_plotMinBounds.y, y);
		m_plotMinBounds.z = std::min(m_plotMinBounds.z, z);

		m_plotMaxBounds.x = std::max(m_plotMaxBounds.x, x);
		m_plotMaxBounds.y = std::max(m_plotMaxBounds.y, y);
		m_plotMaxBounds.z = std::max(m_plotMaxBounds.z, z);
	}
	VBL layoutPlot;
	layoutPlot.push<float>(3); // position
	layoutPlot.push<float>(3); // normal
	layoutPlot.push<float>(1); // normalized solution
	m_solutionPlotEBO = std::make_unique<EBO>(indices);
	m_solutionPlotVBO = std::make_unique<VBO>(vertices);
	m_solutionPlotVAO->addBuffer(vertices, layoutPlot);
	m_solutionPlotVAO->bind();
	m_solutionPlotEBO->bind();
	m_solutionPlotVAO->unbind();
}


inline void Renderer::processInput(const InputManager& im)
{
	m_camera.processInput(im);
}

inline bool Renderer::mouseWheelEvent(double x, double y)
{
	m_camera.mouseWheelEvent(x, y);
	return true;
}

inline Camera& Renderer::getCamera()
{
	return m_camera;
}

inline double Renderer::scaleMin() const
{
	return m_scaleMin;
}

inline double Renderer::scaleMax() const
{
	return m_scaleMax;
}

inline int Renderer::labelCount() const
{
	return m_labelCount;
}

inline int& Renderer::getLabelCount()
{
	return m_labelCount;
}

inline ColorPalette& Renderer::getPalette()
{
	return m_palette;
}

inline bool& Renderer::getDrawPlot()
{
	return m_drawPlot;
}

inline bool& Renderer::getDrawWireframe()
{
	return m_drawWireframe;
}

inline bool& Renderer::getDrawIsolines()
{
	return m_drawIsolines;
}

inline bool& Renderer::getDrawMesh()
{
	return m_drawMesh;
}

inline bool& Renderer::getDrawGrid()
{
	return m_drawGrid;
}

inline void Renderer::createColorMapTextures()
{
	for (int i = 0; i <= static_cast<int>(ColorPalette::JET); i++)
	{
		ColorPalette palette = static_cast<ColorPalette>(i);
		Array<Color> paletteData = ColorMaps::getPaletteData(palette);
		auto texture = std::make_unique<Texture>(paletteData.size());
		texture->bind();
		texture->data(GL_RGB, GL_RGB, GL_FLOAT, paletteData.data());
		m_colorMapTextures[palette] = std::move(texture);
	}
}

inline void Renderer::createBackground()
{
	// vertices directly in vertex shader
	m_backgroundVAO = std::make_unique<VAO>();
}

inline void Renderer::createGrid()
{
	m_gridVAO.reset();
	m_gridVBO.reset();
	Array<float> vertices;
	int gridLines = m_labelCount;
	float step = 1.0f / gridLines;

	for(int i = 0; i <= gridLines; i++)
	{
		float pos = -0.5f + i * step;
		// lines parallel to x axis
		vertices.pushBack(pos);
		vertices.pushBack(-0.5f);
		vertices.pushBack(0.0f);

		vertices.pushBack(pos);
		vertices.pushBack(0.5f);
		vertices.pushBack(0.0f);

		// lineas parallel to y axis
		vertices.pushBack(-0.5f);
		vertices.pushBack(pos);
		vertices.pushBack(0.0f);

		vertices.pushBack(0.5f);
		vertices.pushBack(pos);
		vertices.pushBack(0.0f);
	}
	m_gridVBO = std::make_unique<VBO>(vertices, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	VBL layout;
	layout.push<float>(3);
	m_gridVAO = std::make_unique<VAO>();
	m_gridVAO->addBuffer(*m_gridVBO, layout);
}

inline void Renderer::drawPlot()
{
	m_shaderPlot->bind();
	m_colorMapTextures[m_palette]->bind(0);
	m_shaderPlot->setUniform("u_colorMap", 0);
	glm::mat4 model = glm::mat4(1.0f);
	m_shaderPlot->setUniform("u_model", model);
	if (m_drawWireframe)
	{
		GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	else
	{
		GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}
	m_solutionPlotVAO->bind();
	GL(glDrawElements(GL_TRIANGLES, m_solutionPlotEBO->count(), GL_UNSIGNED_INT, (void*)0));
	GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

inline void Renderer::drawMesh()
{
	m_shaderMesh->bind();
	glm::mat4 model = glm::mat4(1.0f);
	m_shaderMesh->setUniform("u_model", model);
	GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	m_meshVAO->bind();
	GL(glDrawElements(GL_TRIANGLES, m_meshEBO->count(), GL_UNSIGNED_INT, (void*)0));
	GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
}

inline void Renderer::drawGrid()
{
	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); 

	m_shaderGrid->bind();
	m_gridVAO->bind();

	glm::vec3 viewDir = m_camera.viewDir();
	glm::vec3 boxSize = m_plotMaxBounds - m_plotMinBounds;
	glm::vec3 boxCenter = (m_plotMaxBounds + m_plotMinBounds) / 2.0f;

	// normals for 6 faces of the bounding box
	StaticArray<glm::vec3, 6> normals = {
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)
	};
	StaticArray<float, 6> planeAlphas;
	// draw grids
	for (int i = 0; i < 6; i++)
	{
		planeAlphas[i] = (glm::dot(viewDir, normals[i]) > 0.0f) ? glm::abs(glm::dot(viewDir, normals[i])) : 0.0f;
		if (planeAlphas[i] > 0.01f)
		{
			m_shaderGrid->setUniform("u_alpha", planeAlphas[i]);
			glm::mat4 model(1.0f);
			if (i == 0)     // +X
			{
				model = glm::translate(model, glm::vec3(m_plotMaxBounds.x, boxCenter.y, boxCenter.z));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(boxSize.z, boxSize.y, 1.0f));
			}
			else if (i == 1) // -X
			{
				model = glm::translate(model, glm::vec3(m_plotMinBounds.x, boxCenter.y, boxCenter.z));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(boxSize.z, boxSize.y, 1.0f));
			}
			else if (i == 2) // +Y
			{
				model = glm::translate(model, glm::vec3(boxCenter.x, m_plotMaxBounds.y, boxCenter.z));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(boxSize.x, boxSize.z, 1.0f));
			}
			else if (i == 3) // -Y
			{
				model = glm::translate(model, glm::vec3(boxCenter.x, m_plotMinBounds.y, boxCenter.z));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(boxSize.x, boxSize.z, 1.0f));
			}
			else if (i == 4) // +Z
			{
				model = glm::translate(model, glm::vec3(boxCenter.x, boxCenter.y, m_plotMaxBounds.z));
				model = glm::scale(model, glm::vec3(boxSize.x, boxSize.y, 1.0f));
			}
			else if (i == 5) // -Z
			{
				model = glm::translate(model, glm::vec3(boxCenter.x, boxCenter.y, m_plotMinBounds.z));
				model = glm::scale(model, glm::vec3(boxSize.x, boxSize.y, 1.0f));
			}
			m_shaderGrid->setUniform("u_model", model);
			GL(glDrawArrays(GL_LINES, 0, m_gridVBO->count()));

		}
	}
	m_gridVAO->unbind();
	
	// draw text labels
	glm::mat4 view = m_camera.view();
	glm::mat4 projection = m_camera.projection();
	glm::vec3 camPos = m_camera.pos3D();
	glm::vec4 viewport = glm::vec4(0, 0, m_width, m_height);

	// determine back corner
	glm::vec3 backCorner;
	backCorner.x = (viewDir.x < 0) ? m_plotMinBounds.x : m_plotMaxBounds.x;
	backCorner.y = (viewDir.y < 0) ? m_plotMinBounds.y : m_plotMaxBounds.y;
	backCorner.z = (viewDir.z < 0) ? m_plotMinBounds.z : m_plotMaxBounds.z;


	// helper lambda to draw text
	auto drawLabel = [&](const glm::vec3& worldPos, const std::string& text, const glm::vec3& color, float alpha, const glm::vec3& axis)
		{
			// projext 3D point onto 2D screen space
			glm::vec3 screenPos = glm::project(worldPos, view, projection, viewport);
			// only draw labels that are actually on the screen
			if (screenPos.z > 1.0f ||
				screenPos.x < 0 || screenPos.x > m_width ||
				screenPos.y < 0 || screenPos.y > m_height)
				return;

			// shift labels slightly to avoid overlap
			glm::vec3 offsetDir = glm::normalize(glm::cross(axis, viewDir));
			screenPos += offsetDir * 5.0f; // 5 pixel offset
			m_textRenderer->renderText(text, screenPos.x, screenPos.y, 0.4f, color, alpha, TextAlign::RIGHT);
		};

	// --- Define the 3 framing axes based on camera direction ---

	// First, determine the "closest" and "furthest" bounds for each axis
	float closestX = (viewDir.x < 0) ? m_plotMaxBounds.x : m_plotMinBounds.x;
	float furthestX = (viewDir.x < 0) ? m_plotMinBounds.x : m_plotMaxBounds.x;
	float closestY = (viewDir.y < 0) ? m_plotMaxBounds.y : m_plotMinBounds.y;
	float furthestY = (viewDir.y < 0) ? m_plotMinBounds.y : m_plotMaxBounds.y;
	float closestZ = (viewDir.z < 0) ? m_plotMaxBounds.z : m_plotMinBounds.z;
	float furthestZ = (viewDir.z < 0) ? m_plotMinBounds.z : m_plotMaxBounds.z;

	// Now, define the origin for each axis by mixing closest/furthest bounds
	// This ensures the chosen edges do not meet at the same corner.
	glm::vec3 xAxisOrigin = glm::vec3(m_plotMinBounds.x, closestY, furthestZ);
	glm::vec3 yAxisOrigin = glm::vec3(furthestX, m_plotMinBounds.y, closestZ);
	glm::vec3 zAxisOrigin = glm::vec3(closestX, furthestY, m_plotMinBounds.z);

	// --- Draw the labels along each of the three calculated frame edges ---
	StaticArray<glm::vec3, 3> axis = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
	// Draw X-Axis Labels
	{
		// Determine the normals of the two planes that form the X-axis edge
		glm::vec3 yPlaneNormal = (closestY == m_plotMaxBounds.y) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 zPlaneNormal = (furthestZ == m_plotMaxBounds.z) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 0.0f, -1.0f);
		// Calculate the alpha for each plane
		float alphaY = (glm::dot(viewDir, yPlaneNormal) > 0.0f) ? glm::abs(glm::dot(viewDir, yPlaneNormal)) : 0.0f;
		float alphaZ = (glm::dot(viewDir, zPlaneNormal) > 0.0f) ? glm::abs(glm::dot(viewDir, zPlaneNormal)) : 0.0f;
		// Use the brightest of the two alphas for the labels
		float labelAlpha = glm::max(alphaY, alphaZ);

		glm::vec3 xAxisDir = glm::vec3(1.0f, 0.0f, 0.0f);
		float xStep = boxSize.x / m_labelCount;
		for (int i = 0; i <= m_labelCount; i++) {
			glm::vec3 labelPos = xAxisOrigin + xAxisDir * (static_cast<float>(i) * xStep);
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << labelPos.x;
			drawLabel(labelPos, ss.str(), glm::vec3(0.8f), labelAlpha, xAxisDir);
		}
	}

	// Draw Y-Axis Labels
	{
		glm::vec3 xPlaneNormal = (furthestX == m_plotMaxBounds.x) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 zPlaneNormal = (closestZ == m_plotMaxBounds.z) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 0.0f, -1.0f);
		float alphaX = (glm::dot(viewDir, xPlaneNormal) > 0.0f) ? glm::abs(glm::dot(viewDir, xPlaneNormal)) : 0.0f;
		float alphaZ = (glm::dot(viewDir, zPlaneNormal) > 0.0f) ? glm::abs(glm::dot(viewDir, zPlaneNormal)) : 0.0f;
		float labelAlpha = glm::max(alphaX, alphaZ);

		glm::vec3 yAxisDir = glm::vec3(0.0f, 1.0f, 0.0f);
		float yStep = boxSize.y / m_labelCount;
		for (int i = 0; i <= m_labelCount; i++) {
			glm::vec3 labelPos = yAxisOrigin + yAxisDir * (static_cast<float>(i) * yStep);
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << labelPos.y;
			drawLabel(labelPos, ss.str(), glm::vec3(0.8f), labelAlpha, yAxisDir);
		}
	}

	// Draw Z-Axis Labels
	{
		glm::vec3 xPlaneNormal = (closestX == m_plotMaxBounds.x) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 yPlaneNormal = (furthestY == m_plotMaxBounds.y) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
		float alphaX = (glm::dot(viewDir, xPlaneNormal) > 0.0f) ? glm::abs(glm::dot(viewDir, xPlaneNormal)) : 0.0f;
		float alphaY = (glm::dot(viewDir, yPlaneNormal) > 0.0f) ? glm::abs(glm::dot(viewDir, yPlaneNormal)) : 0.0f;
		float labelAlpha = glm::max(alphaX, alphaY);

		glm::vec3 zAxisDir = glm::vec3(0.0f, 0.0f, 1.0f);
		float zStep = boxSize.z / m_labelCount;
		for (int i = 0; i <= m_labelCount; i++) {
			glm::vec3 labelPos = zAxisOrigin + zAxisDir * (static_cast<float>(i) * zStep);
			std::stringstream ss;
			float value = (labelPos.z / m_solutionScaleFactor) * (m_scaleMax - m_scaleMin) + m_scaleMin;
			ss << std::fixed << std::setprecision(2) << value;
			drawLabel(labelPos, ss.str(), glm::vec3(0.8f), labelAlpha, zAxisDir);
		}
	}
	GL(glDisable(GL_BLEND));
}

inline void Renderer::drawBackground()
{
	GL(glDisable(GL_DEPTH_TEST));
	m_shaderBackground->bind();
	m_backgroundVAO->bind();
	GL(glDrawArrays(GL_TRIANGLES, 0, 6));
	m_backgroundVAO->unbind();
	GL(glEnable(GL_DEPTH_TEST));
}

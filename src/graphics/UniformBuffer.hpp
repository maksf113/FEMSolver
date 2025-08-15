#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gl/glew.h>

struct UBOData
{
	glm::mat4 view;
	glm::mat4 projection;
	int is2D; // bool as int to avoid alignment std140 issues
	int labelCount;
	int drawIsolines;
};

class UniformBuffer
{
public:
	UniformBuffer();
	~UniformBuffer();
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer(UniformBuffer&&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;
	UniformBuffer& operator=(UniformBuffer&&) = delete;
	void data(const glm::mat4& view, const glm::mat4& projection, bool is2D, int labelCount, int drawIsolines);
private:
	uint32_t m_id;
	UBOData m_data;
};

UniformBuffer::UniformBuffer()
{
	GL(glGenBuffers(1, &m_id));
	GL(glBindBuffer(GL_UNIFORM_BUFFER, m_id));
	GL(glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOData), nullptr, GL_STATIC_DRAW));
	GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

UniformBuffer::~UniformBuffer()
{
	GL(glDeleteBuffers(1, &m_id));
}

void UniformBuffer::data(const glm::mat4& view, const glm::mat4& projection, bool is2D, int labelCount, int drawIsolines)
{
	m_data.view = view;
	m_data.projection = projection;
	m_data.is2D = is2D;
	m_data.labelCount = labelCount;
	m_data.drawIsolines = drawIsolines;
	GL(glBindBuffer(GL_UNIFORM_BUFFER, m_id));
	GL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOData), &m_data));
	GL(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_id, 0, sizeof(UBOData)));
	GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

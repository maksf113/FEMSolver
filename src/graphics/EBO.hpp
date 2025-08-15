#pragma once
#include <GL/glew.h>
#include "GLError.hpp"
#include "data_structures/Array.hpp"

class EBO
{
private:
	uint32_t m_id;
	uint32_t m_count;
	GLenum m_mode;
public:
	EBO(GLenum mode = GL_STATIC_DRAW);
	EBO(const Array<uint32_t>& indices, GLenum mode = GL_STATIC_DRAW);
	~EBO();
	EBO(const EBO&) = delete;
	EBO(EBO&&) = delete;
	EBO& operator=(const EBO&) = delete;
	EBO& operator=(EBO&&) = delete;
	void bind() const;
	void unbind() const;
	void data(const Array<uint32_t>& indices);
	size_t count() const;
};

EBO::EBO(GLenum mode) : m_mode(mode)
{
	GL(glGenBuffers(1, &m_id));
}
inline EBO::EBO(const Array<uint32_t>& indices, GLenum mode) : m_mode(mode)
{
	GL(glGenBuffers(1, &m_id));
	EBO::data(indices);
}
inline EBO::~EBO()
{
	GL(glDeleteBuffers(1, &m_id));
}

inline void EBO::bind() const
{
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}
inline void EBO::unbind() const
{
	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

inline void EBO::data(const Array<uint32_t>& indices)
{
	bind();
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(uint32_t)), indices.data(), m_mode));
	m_count = indices.size();
	unbind();
}

inline size_t EBO::count() const
{
	return m_count;
}

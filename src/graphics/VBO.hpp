#pragma once
#include <GL/glew.h>
#include <data_structures/Array.hpp>
#include "GLError.hpp"

class VBO
{
private:
	GLuint m_id;
	GLenum m_target;
	GLenum m_mode;
	uint32_t m_count;
public:
	VBO(GLenum target = GL_ARRAY_BUFFER, GLenum mode = GL_STATIC_DRAW);
	template<typename T>
	VBO(const Array<T>& vertices, GLenum target = GL_ARRAY_BUFFER, GLenum mode = GL_STATIC_DRAW);
	~VBO();
	VBO(const VBO&) = delete;
	VBO(VBO&&) = delete;
	VBO& operator=(const VBO&) = delete;
	VBO& operator=(VBO&&) = delete;
	void bind() const;
	void unbind() const;
	template<typename T>
	void data(const Array<T>& vertices);
	void data(size_t byteCount, void* data);
	void subData(size_t offset, size_t byteCount, const void* data);
	uint32_t count() const;
};

VBO::VBO(GLenum target, GLenum mode) : m_target(target), m_mode(mode)
{
	GL(glGenBuffers(1, &m_id));
}

template<typename T>
inline VBO::VBO(const Array<T>& vertices, GLenum target, GLenum mode) : m_target(target), m_mode(mode)
{
	GL(glGenBuffers(1, &m_id));
	VBO::data(vertices);
}
inline VBO::~VBO()
{
	GL(glDeleteBuffers(1, &m_id));
}

inline void VBO::bind() const
{
	GL(glBindBuffer(m_target, m_id));
}
inline void VBO::unbind() const
{
	GL(glBindBuffer(m_target, 0));
}

inline void VBO::data(size_t byteCount, void* data)
{
	GL(glBufferData(m_target, byteCount, data, m_mode));
}

inline void VBO::subData(size_t offset, size_t byteCount, const void* data)
{
	GL(glBufferSubData(m_target, offset, byteCount, data));
}

inline uint32_t VBO::count() const
{
	return m_count;
}

template<typename T>
inline void VBO::data(const Array<T>& vertices)
{
	bind();
	GL(glBufferData(m_target, GLsizeiptr(vertices.size() * sizeof(T)), vertices.data(), m_mode));
	unbind();
	m_count = vertices.size();
}
#pragma once
#include  <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLError.hpp"

class Renderbuffer
{
public:
	Renderbuffer();
	Renderbuffer(uint32_t width, uint32_t height);
	Renderbuffer(uint32_t width, uint32_t height, uint32_t samples);
	~Renderbuffer();
	Renderbuffer(const Renderbuffer&) = delete;
	Renderbuffer(Renderbuffer&&) = delete;
	Renderbuffer& operator=(const Renderbuffer&) = delete;
	Renderbuffer& operator=(Renderbuffer&&) = delete;
	void storage(uint32_t width, uint32_t height);
	void bind() const;
	void unbind() const;
	uint32_t id() const;
private:
	uint32_t m_id;
};

Renderbuffer::Renderbuffer()
{
	GL(glGenRenderbuffers(1, &m_id));
}

Renderbuffer::Renderbuffer(uint32_t width, uint32_t height)
{
	GL(glGenRenderbuffers(1, &m_id));
	bind();
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
	unbind();
}

inline Renderbuffer::Renderbuffer(uint32_t width, uint32_t height, uint32_t samples)
{
	GL(glGenRenderbuffers(1, &m_id));
	bind();
	GL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height));
	unbind();
}

Renderbuffer::~Renderbuffer()
{
	GL(glDeleteRenderbuffers(1, &m_id));
}

void Renderbuffer::storage(uint32_t width, uint32_t height)
{
	bind();
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
}

void Renderbuffer::bind() const
{
	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_id));
}

void Renderbuffer::unbind() const
{
	GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

uint32_t Renderbuffer::id() const
{
	return m_id;
}
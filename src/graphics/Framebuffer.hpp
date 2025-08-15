#pragma once
#include <memory>
#include  <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Texture.hpp"
#include "Renderbuffer.hpp"
#include "GLerror.hpp"

class Framebuffer
{
public:
	Framebuffer(uint32_t width, uint32_t height);
	~Framebuffer();
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&&) = delete;
	Framebuffer(Framebuffer&&) = delete;
	void resize(uint32_t width, uint32_t height);
	void bind() const;
	void unbind() const;
	void drawToScreen(uint32_t width, uint32_t height) const;
	uint32_t width() const;
	uint32_t height() const;
private:
	bool isComplete();
private:
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_id;
	std::unique_ptr<Texture> m_texture;
	std::unique_ptr<Renderbuffer> m_renderbuffer;

};

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
	: m_width(width), m_height(height), m_texture(std::make_unique<Texture>(width, height)), m_renderbuffer(std::make_unique<Renderbuffer>(width, height))
{
	GL(glGenFramebuffers(1, &m_id));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
	//GL(glEnable(GL_FRAMEBUFFER_SRGB));

	m_texture->bind();
	GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->id(), 0));

	m_renderbuffer->bind();
	GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer->id()));
	m_renderbuffer->unbind();

	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Framebuffer::~Framebuffer()
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GL(glDeleteFramebuffers(1, &m_id));
}

void Framebuffer::resize(uint32_t newWidth, uint32_t newHeight)
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	m_width = newWidth;
	m_height = newHeight;
	m_texture.reset(new Texture(newWidth, newHeight));
	m_renderbuffer.reset(new Renderbuffer(newWidth, newHeight));
	GL(glGenFramebuffers(1, &m_id));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
	GL(glEnable(GL_FRAMEBUFFER_SRGB));

	m_texture->bind();
	GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->id(), 0));

	m_renderbuffer->bind();
	GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer->id()));
	m_renderbuffer->unbind();

	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::bind() const
{
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id));
}

void Framebuffer::unbind() const
{
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void Framebuffer::drawToScreen(uint32_t width, uint32_t height) const
{
	GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id));
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, width, height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}

uint32_t Framebuffer::width() const
{
	return m_width;
}

uint32_t Framebuffer::height() const
{
	return m_height;
}

bool Framebuffer::isComplete()
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
	GL(GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return true;
}
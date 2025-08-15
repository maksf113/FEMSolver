#pragma once
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLError.hpp"

class Texture
{
public:
	Texture(uint32_t width, uint32_t height);
	explicit Texture(uint32_t width);
	Texture(uint32_t width, uint32_t height, GLenum internalFormat, GLenum format, GLenum dtatType, const void* data);
	~Texture();
	Texture(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) = delete;
	void cleanup();
	void bind() const;
	void bind(int unit) const;
	void unbind() const;
	void minFilter(GLenum min);
	void magFilter(GLenum mag);
	void wrap(GLenum wrapping);
	void data(GLenum internalFormat, GLenum format, GLenum dataType, void* data);
	uint32_t id() const;
private:
	uint32_t m_id = 0;
	uint32_t m_target;
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	uint32_t m_chanels = 0;
	std::string m_name = "";
};

Texture::Texture(uint32_t width, uint32_t height) : m_target(GL_TEXTURE_2D), m_width(width), m_height(height), m_chanels(4)
{
	GL(glGenTextures(1, &m_id));
	bind();
	minFilter(GL_LINEAR);
	magFilter(GL_LINEAR);
	wrap(GL_CLAMP_TO_EDGE);
	data(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}
inline Texture::Texture(uint32_t width) : m_target(GL_TEXTURE_1D), m_width(width), m_height(1), m_chanels(3)
{
	GL(glGenTextures(1, &m_id));
	bind();
	minFilter(GL_LINEAR);
	magFilter(GL_LINEAR);
	wrap(GL_CLAMP_TO_EDGE);
}
inline Texture::Texture(uint32_t width, uint32_t height, GLenum internalFormat, GLenum format, GLenum dataType, const void* data) :
	m_target(GL_TEXTURE_2D), m_width(width), m_height(height)
{
	GL(glGenTextures(1, &m_id));
	bind();
	minFilter(GL_LINEAR);
	magFilter(GL_LINEAR);
	wrap(GL_CLAMP_TO_EDGE);
	// storage mode - for non-RGB textures (glyph mask)
	GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	this->data(internalFormat, format, dataType, const_cast<void*>(data));
	//reset pixel storage mode
	GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
}
void Texture::cleanup()
{
	GL(glDeleteTextures(1, &m_id));
	m_id = 0;
	m_chanels = 0;
	m_width = 0;
	m_height = 0;
}
Texture::~Texture()
{
	GL(glDeleteTextures(1, &m_id));
}

void Texture::bind() const
{
	GL(glBindTexture(m_target, m_id));
}

void Texture::bind(int unit) const
{
	GL(glActiveTexture(GL_TEXTURE0 + unit));
	GL(glBindTexture(m_target, m_id));
}

void Texture::unbind() const
{
	GL(glBindTexture(m_target, 0));
}

void Texture::minFilter(GLenum min)
{
	GL(glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, min));
}

void Texture::magFilter(GLenum mag)
{
	GL(glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, mag));
}

void Texture::wrap(GLenum wrapping)
{
	GL(glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrapping));
	if(m_target == GL_TEXTURE_2D)
	{
		GL(glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrapping));
	}
}

void Texture::data(GLenum internalFormat, GLenum format, GLenum dataType, void* data)
{
	bind();
	if (m_target == GL_TEXTURE_1D)
	{
		GL(glTexImage1D(m_target, 0, internalFormat, m_width, 0, format, dataType, data));
	}
	else
	{
		GL(glTexImage2D(m_target, 0, internalFormat, m_width, m_height, 0, format, dataType, data));
	}
}

uint32_t Texture::id() const
{
	return m_id;
}

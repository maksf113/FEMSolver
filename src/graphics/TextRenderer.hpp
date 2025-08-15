#pragma once
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "data_structures/Map.hpp"

#include "Shader.hpp"
#include "Texture.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

struct Character
{
	std::unique_ptr<Texture> texture;
	glm::ivec2 size; // size of glyph
	glm::ivec2 bearing; // offset from baseline to left/top of glyph
	uint32_t advance; // horizontal offset to advance to next glyph
};

enum class TextAlign
{
	LEFT,
	CENTER,
	RIGHT
};

class TextRenderer
{
public:
	TextRenderer(uint32_t width, uint32_t height);
	~TextRenderer() = default;
	TextRenderer(const TextRenderer&) = delete;
	TextRenderer(TextRenderer&&) = delete;
	TextRenderer& operator()(const TextRenderer&) = delete;
	TextRenderer& operator()(TextRenderer&&) = delete;

	void loadFont(const std::string& fontPath, uint32_t fontSize);
	void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, float alpha, TextAlign align);
	void setProjection(uint32_t width, uint32_t height);
	glm::vec2 getTextSize(const std::string& text, float scale);
private:
	Map<char, Character> m_characters;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<VAO> m_vao;
	std::unique_ptr<VBO> m_vbo;
};

TextRenderer::TextRenderer(uint32_t width, uint32_t height)
{
	m_shader = std::make_unique<Shader>("shaders/text.vert", "shaders/text.frag");
	m_shader->bind();
	m_shader->setUniform("u_projection", glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height)));
	m_shader->setUniform("u_mask", 0);
	// configure buffers for texture quads
	m_vao = std::make_unique<VAO>();
	m_vbo = std::make_unique<VBO>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	m_vbo->bind();
	m_vbo->data(sizeof(float) * 6 * 4, nullptr);
	VBL layout;
	layout.push<float>(2);
	layout.push<float>(2);
	m_vao->bind();
	m_vao->addBuffer(*m_vbo, layout);
	m_vbo->unbind();
	m_vao->unbind();
}

inline void TextRenderer::loadFont(const std::string& fontPath, uint32_t fontSize)
{
	m_characters.clear();
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: could not init FreeType Libraru" << std::endl;
		return;
	}
	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	// load first 128 ASCII characters
	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
			continue;
		}
		auto texture = std::make_unique<Texture>(
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			GL_RED,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// map character
		Character character = {
			std::move(texture),
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<uint32_t>(face->glyph->advance.x)
		};
		m_characters.insert(std::pair<char, Character>(c, std::move(character)));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

inline void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, float alpha, TextAlign align)
{
	if (align == TextAlign::RIGHT)
	{
		x -= getTextSize(text, scale).x;
	}
	else if (align == TextAlign::CENTER)
	{
		x -= getTextSize(text, scale).x / 2.0f;
	}
	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GL(glDisable(GL_DEPTH_TEST));
	m_shader->bind();
	m_shader->setUniform("u_color", color.r, color.g, color.b);
	m_shader->setUniform("u_alpha", alpha);
	GL(glActiveTexture(GL_TEXTURE0));
	m_vao->bind();

	for (const char& c : text)
	{
		Character& ch = m_characters[c];
		float xPos = x + ch.bearing.x * scale;
		float yPos = y - (ch.size.y - ch.bearing.y) * scale;
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		float vertices[6][4] =
		{
			{ xPos,     yPos + h,   0.0f, 0.0f },
			{ xPos,     yPos,       0.0f, 1.0f },
			{ xPos + w, yPos,       1.0f, 1.0f },
			{ xPos,     yPos + h,   0.0f, 0.0f },
			{ xPos + w, yPos,       1.0f, 1.0f },
			{ xPos + w, yPos + h,   1.0f, 0.0f }
		};
		if (ch.texture != nullptr)
		{
			ch.texture->bind();
		}
		m_vbo->bind();
		m_vbo->subData(0, sizeof(vertices), vertices);
		m_vbo->unbind();
		GL(glDrawArrays(GL_TRIANGLES, 0, 6));
		x += (ch.advance >> 6) * scale;
	}
	m_vao->unbind();
	GL(glDisable(GL_BLEND));
	GL(glEnable(GL_DEPTH_TEST));
}

inline void TextRenderer::setProjection(uint32_t width, uint32_t height)
{
	m_shader->bind();
	m_shader->setUniform("u_projection", glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height)));
}

inline glm::vec2 TextRenderer::getTextSize(const std::string& text, float scale)
{
	glm::vec2 size = { 0.0f, 0.0f };
	float maxHeight = 0.0f;
	for (const char& c : text)
	{
		Character& ch = m_characters[c];
		// total width is the sum of advances
		size.x += (ch.advance >> 6) * scale;
		if (ch.size.y * scale > size.y)
		{
			size.y = ch.size.y * scale;
		}
	}
	return size;
}

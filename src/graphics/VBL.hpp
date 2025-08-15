#pragma once
#include <GL/glew.h>
#include "data_structures/Array.hpp"
#include "GLError.hpp"

struct LayoutElement
{
	unsigned int type;
	unsigned int count;
	bool normalized;
	unsigned int offset;
	LayoutElement(unsigned int _type = 0, unsigned int _count = 0, bool _normalized = 0, unsigned int _offset = 0) :
		type(_type), count(_count), normalized(_normalized), offset(_offset) {}
};

class VBL
{
private:
	Array<LayoutElement> m_elements;
	uint32_t m_stride = 0;
public:
	VBL() = default;
	~VBL() = default;
	VBL(const VBL&) = default;
	VBL(VBL&&) = default;
	VBL& operator=(const VBL&) = default;
	VBL& operator=(VBL&&) = default;
	template<typename T>
	void push(unsigned int count, bool normalized = false)
	{
		std::runtime_error(false);
	}
	const Array<LayoutElement>& getElements() const;
	const unsigned int getStride() const;
};

template<>
void VBL::push<float>(unsigned int count, bool normalized)
{
	m_elements.pushBack({ GL_FLOAT, count, normalized, m_stride });
	m_stride += count * sizeof(float);
}

template<>
void VBL::push<unsigned int>(unsigned int count, bool normalized)
{
	m_elements.pushBack(LayoutElement(GL_UNSIGNED_INT, count, normalized, m_stride));
	m_stride += count * sizeof(unsigned int);
}

template<>
void VBL::push<unsigned char>(unsigned int count, bool normalized)
{
	m_elements.pushBack(LayoutElement(GL_UNSIGNED_BYTE, count, normalized, m_stride));
	m_stride += count * sizeof(unsigned char);
}

inline const Array<LayoutElement>& VBL::getElements() const
{
	return m_elements;
}

inline const unsigned int VBL::getStride() const
{
	return m_stride;
}
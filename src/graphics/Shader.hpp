#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "GLError.hpp"
#include "data_structures/Array.hpp"
#include "data_structures/Map.hpp"

class Shader
{
private:
	GLuint m_id;
	Map<std::string, int> m_uniformLocationCache;
	int getUniformLocation(const std::string& name);
public:
	Shader(const std::string&, const std::string&);
	~Shader();
	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;
	void bind() const;
	void bindUniformBlock(const std::string& name, int i);
	void setUniform(const std::string& name, int i);
	void setUniform(const std::string& name, float v0);
	void setUniform(const std::string& name, float v0, float v1);
	void setUniform(const std::string& name, float v0, float v1, float v2);
	void setUniform(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform(const std::string& name, const glm::mat4& mat);
};

const std::string readShaderSource(const std::string& filePath) 
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	if (fileStream.is_open() == false)
	{
		std::cout << "Cannot open file" << std::endl;
		return content;
	}
	std::string line = "";
	while (!fileStream.eof()) 
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}


void printShaderLog(GLuint shader)
{
	int length = 0;
	GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
	if (length > 0)
	{
		Array<char> log(length + 1); // +1 for null terminator
		GL(glGetShaderInfoLog(shader, length, nullptr, &log[0]));
		std::cout << "Shader info log: " << log.data() << std::endl;
	}
}

void printProgramLog(GLuint prog)
{
	int length = 0;
	GL(glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length));
	if (length > 0)
	{
		Array<char> log(length + 1); // +1 for null terminator
		GL(glGetProgramInfoLog(prog, length, nullptr, &log[0]));
		std::cout << "Shader info log: " << log.data() << std::endl;
	}
}

Shader::Shader(const std::string& vertPath, const std::string& fragPath)
{
	const std::string vertSource = readShaderSource(vertPath);
	const std::string fragSource = readShaderSource(fragPath);
	const char* vertSrcCstr = vertSource.c_str();
	const char* fragSrcCstr = fragSource.c_str();
	GL(GLuint vs = glCreateShader(GL_VERTEX_SHADER));
	GL(GLuint fs = glCreateShader(GL_FRAGMENT_SHADER));
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	GL(glShaderSource(vs, 1, &vertSrcCstr, NULL));
	GL(glCompileShader(vs));
	GL(glGetShaderiv(vs, GL_COMPILE_STATUS, &vertCompiled));
	if (vertCompiled != 1)
	{
		std::cout << "Veretx compilation failed" << std::endl;
		printShaderLog(vs);
	}

	GL(glShaderSource(fs, 1, &fragSrcCstr, NULL));
	GL(glCompileShader(fs));
	GL(glGetShaderiv(fs, GL_COMPILE_STATUS, &fragCompiled));
	if (fragCompiled != 1)
	{
		std::cout << "Veretx compilation failed" << std::endl;
		printShaderLog(fs);
	}

	GL(m_id = glCreateProgram());
	GL(glAttachShader(m_id, vs));
	GL(glAttachShader(m_id, fs));
	GL(glLinkProgram(m_id));

	GL(glGetProgramiv(m_id, GL_LINK_STATUS, &linked));
	if (linked != 1)
	{
		std::cout << "Program linking failed" << std::endl;
		printProgramLog(m_id);
	}
	GL(glDeleteShader(vs));
	GL(glDeleteShader(fs));
}

Shader::~Shader()
{
	GL(glDeleteProgram(m_id));
}

void Shader::bind() const
{
	GL(glUseProgram(m_id));
}

void Shader::bindUniformBlock(const std::string& name, int i)
{
	GL(int uboIndex = glGetUniformBlockIndex(m_id, name.c_str()));
	GL(glUniformBlockBinding(m_id, uboIndex, i));
}

inline void Shader::setUniform(const std::string& name, int i)
{
	int location = getUniformLocation(name);
	GL(glUniform1i(location, i));
}

inline void Shader::setUniform(const std::string& name, float v0)
{
	int location = getUniformLocation(name);
	GL(glUniform1f(location, v0));
}

inline void Shader::setUniform(const std::string& name, float v0, float v1)
{
	int location = getUniformLocation(name);
	GL(glUniform2f(location, v0, v1));
}

inline void Shader::setUniform(const std::string& name, float v0, float v1, float v2)
{
	int location = getUniformLocation(name);
	GL(glUniform3f(location, v0, v1, v2));
}

inline void Shader::setUniform(const std::string& name, float v0, float v1, float v2, float v3)
{
	int location = getUniformLocation(name);
	GL(glUniform4f(location, v0, v1, v2, v3));
}

inline void Shader::setUniform(const std::string& name, const glm::mat4& mat)
{
	int location = getUniformLocation(name);
	GL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
}

inline int Shader::getUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		return m_uniformLocationCache[name];
	GL(int location = glGetUniformLocation(m_id, name.c_str()));
	m_uniformLocationCache[name] = location;
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' does not exist!" << std::endl;
	return location;
}



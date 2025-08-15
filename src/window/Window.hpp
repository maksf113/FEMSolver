#pragma once
#include <string>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "graphics/Renderer.hpp"

#include "InputManager.hpp"
#include "InputReciever.hpp"

class Window : public InputReciever
{
public:
	Window(uint32_t width, uint32_t height, const std::string& title);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	GLFWwindow* get();
	bool shouldClose() const;
	void pollEvents();
	void swapBuffers();
	bool mouseButtonEvent(int button, int action, int mods);
private:
	bool init(uint32_t width, uint32_t height, const std::string& title);
private:
	GLFWwindow* m_handle = nullptr;
	std::string m_title;
	uint32_t m_width;
	uint32_t m_height;

	std::shared_ptr<Renderer> m_renderer;
};

Window::Window(uint32_t width, uint32_t height, const std::string& title)
	: m_width(width), m_height(height), m_title(title)
{
	if (!init(width, height, title))
		printf("%s: Error: Window init error\n", __FUNCTION__);
	glfwSwapInterval(1);
	glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Window::~Window()
{
	printf("%s: Terminating window\n", __FUNCTION__);
	glfwDestroyWindow(m_handle);
	glfwTerminate();
}

inline GLFWwindow* Window::get()
{
	return m_handle;
}
inline bool Window::init(uint32_t width, uint32_t height, const std::string& title)
{
	if (!glfwInit())
	{
		printf("%s: glfwInit() error\n", __FUNCTION__);
		return false;
	}

	m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!m_handle)
	{
		printf("%s: Could not create window\n", __FUNCTION__);
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_handle);
	return true;
}


inline bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_handle);
}

inline void Window::pollEvents()
{
	glfwPollEvents();
}

inline void Window::swapBuffers()
{ 
	glfwSwapBuffers(m_handle);
}

inline bool Window::mouseButtonEvent(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_RAW_MOUSE_MOTION);
		}
		else if (action == GLFW_RELEASE)
		{
			glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	return true;
}


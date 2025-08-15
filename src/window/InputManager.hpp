#pragma once
#include <memory>

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>

#include "data_structures/Array.hpp"
#include "data_structures/StaticArray.hpp"

#include "window/InputReciever.hpp"

class InputManager
{
private:
	Array<std::weak_ptr<InputReciever>> m_recievers;
	GLFWwindow* m_window = nullptr;
	StaticArray<bool, GLFW_KEY_LAST + 1> m_keyStates;
	StaticArray<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseButtonStates;
	double m_cursorLastX;
	double m_cursorLastY;
	double m_cursorDX = 0.0;
	double m_cursorDY = 0.0;
	bool m_firstCursorMove = true;
public:
	void setCallbacks(GLFWwindow* window);
	void addReciever(std::shared_ptr<InputReciever> reciever);
	void dispatchKeyEvent(int key, int scancode, int action, int mods);
	void dispatchMouseButtonEvent(int button, int action, int mods);
	void dispatchMousePositionEvent(double x, double y);
	void dispatchMouseWheelEvent(double x, double y);
	void dispatchResizeEvent(uint32_t width, uint32_t height);
	bool isKeyPressed(int key) const;
	bool isMouseButtonPressed(int button) const;
	double cursorDX() const;
	double cursorDY() const;
	void endFrame();
private:
	void setKeyState(int key, int action);
	void setMouseButtonState(int button, int action);
};

inline void InputManager::setCallbacks(GLFWwindow* win)
{
	m_window = win;
	glfwSetWindowUserPointer(win, this);
	// Setting callbacks
	glfwSetWindowSizeCallback(win, [](GLFWwindow* win, int width, int height)
		{auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
	im->dispatchResizeEvent(static_cast<uint32_t>(width), static_cast<uint32_t>(height)); });
	glfwSetKeyCallback(win, [](GLFWwindow* win, int key, int scancode, int action, int mods)
		{auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
	im->dispatchKeyEvent(key, scancode, action, mods); });
	glfwSetMouseButtonCallback(win, [](GLFWwindow* win, int button, int action, int mods) {
		auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
		im->dispatchMouseButtonEvent(button, action, mods);
		});
	glfwSetCursorPosCallback(win, [](GLFWwindow* win, double x, double y) {
		auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
		im->dispatchMousePositionEvent(x, y);
		});
	glfwSetScrollCallback(win, [](GLFWwindow* win, double x, double y) {
		auto im = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
		im->dispatchMouseWheelEvent(x, y);
		});
}

inline void InputManager::addReciever(std::shared_ptr<InputReciever> reciever)
{
	m_recievers.pushBack(reciever);
}

inline void InputManager::dispatchKeyEvent(int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(m_window, key, scancode, action, mods);
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;
	setKeyState(key, action);
	for (const auto& weakReciever : m_recievers)
	{
		if (auto sharedReciever = weakReciever.lock())
		{
			sharedReciever->keyEvent(key, scancode, action, mods);
		}
	}
}

inline void InputManager::dispatchMouseButtonEvent(int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(m_window, button, action, mods);
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;
	setMouseButtonState(button, action);
	for (const auto& weakReciever : m_recievers)
	{
		if (auto sharedReciever = weakReciever.lock())
		{
			sharedReciever->mouseButtonEvent(button, action, mods);
		}
	}
}

inline void InputManager::dispatchMousePositionEvent(double x, double y)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
	{
		m_firstCursorMove = true;
		return;
	}
	if (m_firstCursorMove)
	{
		m_cursorLastX = x;
		m_cursorLastY = y;
		m_firstCursorMove = false;
	}
	m_cursorDX = x - m_cursorLastX;
	m_cursorDY = m_cursorLastY - y;
	m_cursorLastX = x;
	m_cursorLastY = y;
	for (const auto& weakReciever : m_recievers)
	{
		if (auto sharedReciever = weakReciever.lock())
		{
			sharedReciever->mousePositionEvent(x, y);
		}
	}
}

inline void InputManager::dispatchMouseWheelEvent(double x, double y)
{
	for (const auto& weakReciever : m_recievers)
	{
		if (auto sharedReciever = weakReciever.lock())
		{
			sharedReciever->mouseWheelEvent(x, y);
		}
	}
}

inline void InputManager::dispatchResizeEvent(uint32_t width, uint32_t height)
{
	for (const auto& weakReciever : m_recievers)
	{
		if (auto sharedReciever = weakReciever.lock())
		{
			sharedReciever->resizeEvent(width, height);
		}
	}
}

inline bool InputManager::isKeyPressed(int key) const
{
	if (key >= 0 && key < m_keyStates.size())
		return m_keyStates[key];
	else
		return false;
}

inline bool InputManager::isMouseButtonPressed(int button) const
{
	if (button >= 0 && button < m_mouseButtonStates.size())
		return m_mouseButtonStates[button];
	else
		return false;
}

inline double InputManager::cursorDX() const
{
	return m_cursorDX;
}

inline double InputManager::cursorDY() const
{
	return m_cursorDY;
}

inline void InputManager::endFrame()
{
	m_cursorDX = 0.0;
	m_cursorDY = 0.0;
}

inline void InputManager::setKeyState(int key, int action)
{
	if (key >= 0 && key < m_keyStates.size())
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			m_keyStates[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			m_keyStates[key] = false;
		}
	}
}
inline void InputManager::setMouseButtonState(int button, int action)
{
	if (button >= 0 && button < m_mouseButtonStates.size())
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			m_mouseButtonStates[button] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			m_mouseButtonStates[button] = false;
		}
	}
}

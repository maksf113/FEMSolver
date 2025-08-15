#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window/InputManager.hpp"

class Camera
{
public:
	Camera();
	~Camera() = default;
	Camera(const Camera&) = default;
	Camera(Camera&&) = default;
	Camera& operator=(const Camera&) = default;
	Camera& operator=(Camera&&) = default;
	glm::mat4 view() const;
	glm::mat4 projection() const;
	glm::vec3 target() const;
	glm::vec3 viewDir() const;
	glm::vec3 pos3D() const;
	void setAspectRatio(uint32_t width, uint32_t height);
	void processInput(const InputManager& im);
	void mouseWheelEvent(double x, double y);
	void zoom2D(float y);
	void pan2D(double x, double y);
	void zoom3D(float y);
	void orbit3D(double x, double y);
	bool is2D() const;
	void toggleMode();
private:
	void updateVectors3D();
private:
	bool m_mode2D = true;
	float m_aspectRatio = 16.0f / 9.0f;

	// 2D
	glm::vec3 m_pos2D = glm::vec3(0.0f, 0.0f, 120.0f);

	float m_near2D = -150.0f;
	float m_far2D = 150.0f;
	
	float m_scale2D = 1.0f;

	float m_panSensitivity2D = 0.002f;
	float m_zoomSensitivity2D = 0.05f;

	// 3D
	glm::vec3 m_pos3D;
	glm::vec3 m_target = glm::vec3(0.0f);
	float m_distToTarget = 3.2f;

	float m_near3D = 0.05f;
	float m_far3D = 100.0f;
	float m_fov = 45.0f;

	glm::vec3 m_viewDir = glm::vec3(0.0f);
	glm::vec3 m_rightDir = glm::vec3(0.0f);
	glm::vec3 m_upDir = glm::vec3(0.0f);
	glm::vec3 m_worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
	float m_azimuth = -45.0f;
	float m_elevation = 30.0f;
	float m_orbitSensitivity = 0.1f;
	float m_zoomSensitivity3D = 0.05f;
};

inline Camera::Camera()
{
	updateVectors3D();
}

inline glm::mat4 Camera::view() const
{
	if (m_mode2D)
	{
		return glm::ortho(-m_scale2D * m_aspectRatio + m_pos2D.x, m_scale2D * m_aspectRatio + m_pos2D.x, -m_scale2D + m_pos2D.y, m_scale2D + m_pos2D.y, m_near2D, m_far2D);
	}
	else
	{
		return glm::lookAt(m_pos3D, m_target, m_upDir);
	}
	
}

inline glm::mat4 Camera::projection() const
{
	if (m_mode2D)
	{
		return glm::mat4(1.0f);
	}
	else
	{
		return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near3D, m_far3D);
	}
}

inline glm::vec3 Camera::target() const
{
	return m_target;
}

inline glm::vec3 Camera::viewDir() const
{
	return m_viewDir;
}

inline glm::vec3 Camera::pos3D() const
{
	return m_pos3D;
}

inline void Camera::zoom2D(float y)
{
	m_scale2D -= y * m_scale2D * m_zoomSensitivity2D;
	if (m_scale2D < 0.05f)
		m_scale2D = 0.05f;
}

inline void Camera::pan2D(double x, double y)
{
	m_pos2D.x -= static_cast<float>(x) * m_scale2D * m_panSensitivity2D;
	m_pos2D.y -= static_cast<float>(y) * m_scale2D * m_panSensitivity2D;
}

inline void Camera::zoom3D(float y)
{
	m_distToTarget -= y * m_distToTarget * m_zoomSensitivity3D;
	if (m_distToTarget < 0.05f)
		m_distToTarget = 0.05f;
	updateVectors3D();
}

inline void Camera::orbit3D(double x, double y)
{
	m_azimuth -= static_cast<float>(x) * m_orbitSensitivity;
	m_elevation -= static_cast<float>(y) * m_orbitSensitivity;
	if (m_azimuth < 0.0f)
	{
		m_azimuth += 360.0f;
	}
	else if (m_azimuth >= 360.0f)
	{
		m_azimuth -= 360.0f;
	}
	
	if (m_elevation > 89.0)
	{
		m_elevation = 89.0f;
	}
	else if (m_elevation < -89.0)
	{
		m_elevation = -89.0f;
	}
	updateVectors3D();
}

inline bool Camera::is2D() const
{
	return m_mode2D;
}

inline void Camera::toggleMode()
{
	m_mode2D = !m_mode2D;
	if (!m_mode2D)
		updateVectors3D();
}

inline void Camera::updateVectors3D()
{
	float azimRad = glm::radians(m_azimuth);
	float elevRad = glm::radians(m_elevation);

	float sinAzim = sin(azimRad);
	float cosAzim = cos(azimRad);
	float sinElev = sin(elevRad);
	float cosElev = cos(elevRad);

	m_pos3D = m_target + m_distToTarget * glm::vec3(cosAzim * cosElev, cosElev * sinAzim, sinElev);
	m_viewDir = glm::normalize(m_target - m_pos3D);
	m_rightDir = glm::normalize(glm::cross(m_viewDir, m_worldUp));
	m_upDir = glm::normalize(glm::cross(m_rightDir, m_viewDir));
}


inline void Camera::setAspectRatio(uint32_t width, uint32_t height)
{
	m_aspectRatio = static_cast<float>(width) / height;
}

inline void Camera::processInput(const InputManager& im)
{
	if (m_mode2D)
	{
		if(im.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			pan2D(im.cursorDX(), im.cursorDY());
		}
	}
	else
	{
		if (im.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			orbit3D(im.cursorDX(), im.cursorDY());
		}
	}
}

inline void Camera::mouseWheelEvent(double x, double y)
{
	if (m_mode2D)
	{
		zoom2D(y);
	}
	else
	{
		zoom3D(y);
	}
}

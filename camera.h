#include "glm/glm.hpp"

class camera {
	glm::vec3 m_vCameraPos;
	glm::vec3 m_vCameraFront;
	float m_fPitch, m_fRoll, m_fYaw;
	bool ortho = false;
	float m_fFov = 90.f;
	
	auto update_front() {
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		front.y = sin(glm::radians(m_fPitch));
		front.z = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		m_vCameraFront = glm::normalize(front);
	}

public:

	[[nodiscard]]
	auto right() const -> glm::vec3 {
		const auto up = glm::vec3(0.f, 1.f, 0.f);
		return glm::normalize(glm::cross(m_vCameraFront, up));
	}

	[[nodiscard]]
	auto up() const -> glm::vec3 {
		return glm::cross(right(), m_vCameraFront);
	}

	[[nodiscard]]
	auto get_view_matrix() const -> glm::mat4 {
		return glm::lookAt(m_vCameraPos, m_vCameraPos + m_vCameraFront, up());
	}

	[[nodiscard]]
	auto get_projection_matrix() const -> glm::mat4 {
		if (!ortho) {
			return glm::perspective(glm::radians(m_fFov), 16.f / 9.f, 0.1f, 100.0f);
		}
		return glm::ortho(0, 1600, 0, 900);
	}

	auto look_at(const glm::vec3& pos) -> void {
		m_vCameraFront = glm::normalize(pos - m_vCameraPos);
		m_fPitch = glm::degrees(asin(-m_vCameraFront.y));
		m_fYaw = glm::degrees(atan2(m_vCameraFront.x, m_vCameraFront.z));
	}

	auto set_position(const glm::vec3&& pos) {
		m_vCameraPos = pos;
	}

	[[nodiscard]]
	auto get_pos() const {
		return m_vCameraPos;
	}
	
	auto set_pitch(const float pitch) {
		m_fPitch = pitch;
		m_fPitch = std::clamp(m_fPitch, -89.f, 89.f);
		update_front();
	}

	auto add_pitch(const float pitch) {
		set_pitch(pitch + m_fPitch);
	}

	auto set_yaw(const float yaw) {
		m_fYaw = yaw;
		while (m_fYaw > 360.f) {
			m_fYaw -= 360.f;
		}
		while (m_fYaw < 0.f) {
			m_fYaw += 360.f;
		}
		update_front();
	}

	auto add_yaw(const float yaw) {
		set_yaw(yaw + m_fYaw);
	}

	auto move_right(const float amount) {
		m_vCameraPos += right() * amount;
	}

	auto move_left(const float amount) {
		m_vCameraPos -= right() * amount;
	}

	auto move_forward(const float amount) {
		m_vCameraPos += m_vCameraFront * amount;
	}

	auto move_backward(const float amount) {
		m_vCameraPos -= m_vCameraFront * amount;
	}

	auto move_up(const float amount) {
		m_vCameraPos += glm::vec3(0.f, 1.f, 0.f) * amount;
	}

	auto move_down(const float amount) {
		m_vCameraPos -= glm::vec3(0.f, 1.f, 0.f) * amount;
	}

	auto adjust_fov(const float amount) {
		m_fFov += amount;
		if (m_fFov < 5.f) {
			m_fFov = 5.f;
		} else if (m_fFov > 120.f) {
			m_fFov = 120.f;
		}
	}

	camera(glm::vec3&& pos) : m_vCameraPos{ pos } {}
};
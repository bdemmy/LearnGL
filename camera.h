#include "glm/glm.hpp"

class camera {
private:
	glm::vec3 m_vCameraPos;
	glm::vec3 m_vCameraFront;
	float m_fPitch, m_fRoll, m_fYaw;

public:
	auto updateFront() {
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		front.y = sin(glm::radians(m_fPitch));
		front.z = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		m_vCameraFront = glm::normalize(front);
	}

	auto right() const -> glm::vec3 {
		const auto up = glm::vec3(0.f, 1.f, 0.f);
		return glm::normalize(glm::cross(m_vCameraFront, up));
	}

	auto up() const -> glm::vec3 {
		return glm::cross(right(), m_vCameraFront);
	}

	auto matrix() const -> glm::mat4 {
		return glm::lookAt(m_vCameraPos, m_vCameraPos + m_vCameraFront, up());
	}

	auto look_at(const glm::vec3& pos) -> void {
		m_vCameraFront = glm::normalize(pos - m_vCameraPos);
		m_fPitch = glm::degrees(asin(-m_vCameraFront.y));
		m_fYaw = glm::degrees(atan2(m_vCameraFront.x, m_vCameraFront.z));
	}

	auto set_position(const glm::vec3&& pos) {
		m_vCameraPos = pos;
	}

	auto get_pos() const {
		return m_vCameraPos;
	}
	
	auto set_pitch(const float pitch) {
		m_fPitch = pitch;
		updateFront();
	}

	auto set_yaw(const float yaw) {
		m_fYaw = yaw;
		updateFront();
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

	camera(glm::vec3&& pos) : m_vCameraPos{ pos } {}
};
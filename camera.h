#include "glm/glm.hpp"

class camera {
private:
	glm::vec3 m_vCameraPos;
	glm::vec3 m_vCameraFront;
	float m_fPitch, m_fRoll, m_fYaw;

public:
	auto updateFront() {
		glm::vec3 direction;
		direction.x = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		direction.y = sin(glm::radians(m_fPitch));
		direction.z = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
		m_vCameraFront = glm::normalize(direction);
	}

	auto right() const -> glm::vec3 {
		const auto up = glm::vec3(0.f, 1.f, 0.f);
		return glm::normalize(glm::cross(up, m_vCameraFront));
	}

	auto up() const -> glm::vec3 {
		return glm::cross(m_vCameraFront, right());
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

	camera(glm::vec3&& pos) : m_vCameraPos{ pos } {}
};
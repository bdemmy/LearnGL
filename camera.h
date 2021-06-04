#include "glm/glm.hpp"

class camera {
private:
	glm::vec3 m_vCameraPos;
	glm::vec3 m_vCameraFront;

public:
	camera(glm::vec3&& pos, glm::vec3&& target) : m_vCameraPos{ pos }, m_vCameraFront{ glm::normalize(target - m_vCameraPos) } {}

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

	auto look_at(const glm::vec3&& pos) {
		m_vCameraFront = glm::normalize(pos - m_vCameraPos);
	}

	auto set_position(const glm::vec3&& pos) {
		m_vCameraPos = pos;
	}

	auto move_right(const float amount) {
		m_vCameraPos += right() * amount;
	}

	auto move_left(const float amount) {
		m_vCameraPos -= right() * amount;
	}
};
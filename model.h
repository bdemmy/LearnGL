#ifndef MODEL_H
#define MODEL_H
#include <memory>

#include "resource_manager.h"
#include "shader.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/transform.hpp"

class mesh;

class model {
	std::shared_ptr<mesh> m_mMesh;
	std::shared_ptr<shader> m_mShader;
	glm::vec3 m_vPosition {0};
	glm::vec4 m_vColor {1};
	glm::vec3 m_vScale {1.0};
	float m_fPitch, m_fYaw;

public:
	model(std::shared_ptr<mesh> &&mesh, std::shared_ptr<shader> &&shader) : m_mMesh(std::move(mesh)), m_mShader(std::move(shader)) {}
	model(std::string mesh, std::string shader) : m_mMesh(resource_manager::load_mesh(mesh)), m_mShader(resource_manager::load_shader(shader)) {}

	auto set_color(glm::vec4 &col) {
		m_vColor = col;
	}

	[[nodiscard]]
	auto get_color() const {
		return m_vColor;
	}
	
	auto set_position(glm::vec3& pos) {
		m_vPosition = pos;
	}

	[[nodiscard]]
	auto get_position() const {
		return m_vPosition;
	}
	
	auto set_pitch(const float pitch) {
		m_fPitch = pitch;
	}
	
	auto set_yaw(const float yaw) {
		m_fYaw = yaw;
	}

	auto set_scale(const glm::vec3& scale) {
		m_vScale = scale;
	}

	// SCALE ROTATE TRANSFORM
	[[nodiscard]]
	glm::mat4 get_transform() const;

	void draw() const;
};
#endif // MODEL_H

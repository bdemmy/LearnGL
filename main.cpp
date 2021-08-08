//
// Created by demmyb on 8/13/20.
//

// STD includes
#include <iostream>

// Library includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

// Local includes
#include "shader.h"
#include "resource_manager.h"
#include "window.h"
#include "camera.h"
#include "mesh.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.h"
#include "utils.h"

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;
constexpr auto CAMERA_SPEED = 1.f;
constexpr auto MOUSE_SENSITIVITY = 0.1f;

// Global data
std::shared_ptr<shader> mainShader;
std::shared_ptr<shader> lightingShader;
std::shared_ptr<shader> lightSourceShader;
std::shared_ptr<shader> skyboxShader;
unsigned int texContainer, texFace, texCapsule, texTerrain, texSphere;
unsigned int texSkybox;
float deltaTime{0}, lastTime{0};
auto cam1 = camera(glm::vec3(0, 0, 3));

// Function prototypes
void process_input_for_window(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool initialize_shaders();
void initialize_textures();
void initialize_skybox();
void init_matrix_ubo();
void update_matrix_ubo(const glm::mat4&& view, const glm::mat4&& projection, const glm::vec3&& cameraPosition);
void bind_matrix_ubo(const GLuint shader);

std::shared_ptr<mesh> meshSphere;
std::shared_ptr<mesh> meshCube;
std::shared_ptr<mesh> meshCapsule;
std::shared_ptr<mesh> meshCooper;
std::shared_ptr<mesh> meshTerrain;
std::shared_ptr<mesh> meshSkybox;

std::shared_ptr<model> modelSphere;
std::shared_ptr<model> modelLight;

void RenderLight() {
	glm::vec3 lightPos(sin(glfwGetTime()) * 1, 0.25, cos(glfwGetTime()) * 1);
	lightingShader->setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
	modelLight->set_position(lightPos);
	modelLight->set_scale({ 0.2f, 0.2f, 0.2f });
	modelLight->draw();
}

void RenderLitCubes() {
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};
	
	lightingShader->setVec3("lightColor", 1.f, 1.f, 1.0f);

	static float rotation = 0.f;
	rotation += 120.f * deltaTime;

	// SCALE TRANSLATE ROTATE
	for (auto& pos : cubePositions) {
		lightingShader->setInt("textured", 0);
		modelSphere->set_position(pos); 
		modelSphere->set_yaw(rotation);
		modelSphere->draw();
	}
}

void RenderSkybox() {
	glDepthMask(GL_FALSE);
	skyboxShader->use();
	glActiveTexture(GL_TEXTURE3); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);
	lightingShader->setInt("skybox", 3); 
	skyboxShader->setInt("skybox", 3);
	skyboxShader->setMatrix("view", glm::mat4(glm::mat3(cam1.get_view_matrix())));
	skyboxShader->setMatrix("projection", cam1.get_projection_matrix());
	meshSkybox->Draw();
	glDepthMask(GL_TRUE);
}

int main() {
	// Initialize the window optionally using opengl settings
	auto* window = init_window({
		.glContextMajor = 3,
		.glContextMinor = 3,
		.glMSAASamples = 2,
		.width = 1600,
		.height = 900,
		.vsync = true,
		.msaa = true,
		.title = "Main Window"
		});
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Initialize our shaders
	if (!initialize_shaders()) {
		return -1;
	}
	init_matrix_ubo();
	bind_matrix_ubo(mainShader->getProgram());
	bind_matrix_ubo(lightingShader->getProgram());
	bind_matrix_ubo(lightSourceShader->getProgram());

	// Generate and setup the textures (just one for now)
	initialize_textures();
	initialize_skybox();

	// Main camera
	cam1.look_at({ 0, 0, 0 });

	// Our main render loop
	modelSphere = std::make_shared<model>("test.mesh", "genericLit");
	modelLight = std::make_shared<model>("sphere.mesh", "genericLight");
	meshSkybox = resource_manager::load_mesh("skybox.mesh");

	while (!glfwWindowShouldClose(window)) {
		const float curTime = glfwGetTime();
		deltaTime = curTime - lastTime;
		lastTime = curTime;

		glfwPollEvents();
		process_input_for_window(window);

		glClearColor(0.02f, 0.02f, 0.02f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_matrix_ubo(cam1.get_projection_matrix(), cam1.get_view_matrix(), cam1.get_pos());

		RenderSkybox();
		RenderLight();
		RenderLitCubes();

		glfwSwapBuffers(window);
	}

	return 0;
}

GLuint uboMatrices = 0;
GLuint binding_point_index = 10;
void init_matrix_ubo() {
	// Generate the uniform buffer
	// Bind it, copy the data over, and unbind
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Index 10 just to test
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, uboMatrices);
}

void update_matrix_ubo(const glm::mat4&& view, const glm::mat4&& projection, const glm::vec3&& cameraPosition) {
	shader_data.view = view;
	shader_data.projection = projection;
	shader_data.cameraPosition = cameraPosition;

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(shader_data.projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shader_data.view));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(shader_data.cameraPosition));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void bind_matrix_ubo(const GLuint shader) {
	const auto block_index = glGetUniformBlockIndex(shader, "shader_data");
	glUniformBlockBinding(shader, block_index, binding_point_index); 
}

bool initialize_shaders() {
	mainShader = resource_manager::load_shader("generic", "vertex.vert", "fragment.frag");
	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	lightingShader = resource_manager::load_shader("genericLit", "lighting.vert", "lighting.frag");
	if (lightingShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightingShader->log << std::endl;
		return false;
	}

	lightSourceShader = resource_manager::load_shader("genericLight", "lighting.vert", "lightsource.frag");
	if (lightSourceShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightSourceShader->log << std::endl;
		return false;
	}

	skyboxShader = resource_manager::load_shader("skybox", "skybox.vert", "skybox.frag");
	if (skyboxShader->error) {
		std::cerr << "Error compiling shaders: \n" << skyboxShader->log << std::endl;
		return false;
	}

	return true;
}

void initialize_textures() {
	//texContainer = resource_manager::load_texture("container.jpg");
	//texFace = resource_manager::load_texture("awesomeface.png", true);
	//texCapsule = resource_manager::load_texture("capsule0.jpg");
	//texTerrain = resource_manager::load_texture("tex_u1_v1.jpg");
	texSphere = resource_manager::load_texture("korn.jpg");
}

void initialize_skybox() {
	const std::vector<std::string> skybox_faces {
		"./textures/skybox/right.jpg",
		"./textures/skybox/left.jpg",
		"./textures/skybox/top.jpg",
		"./textures/skybox/bottom.jpg",
		"./textures/skybox/front.jpg",
		"./textures/skybox/back.jpg"
	};
	texSkybox = resource_manager::load_cubemap(skybox_faces);
}

void process_input_for_window(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Camera Movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam1.move_forward(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam1.move_backward(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam1.move_left(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam1.move_right(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		cam1.move_up(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		cam1.move_down(CAMERA_SPEED * deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true;
	static float lastX = 400, lastY = 300;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	xoffset *= MOUSE_SENSITIVITY;
	yoffset *= MOUSE_SENSITIVITY;

	cam1.add_yaw(xoffset);
	cam1.add_pitch(yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam1.adjust_fov(-2.f * yoffset);
}

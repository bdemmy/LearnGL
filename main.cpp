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

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;
constexpr auto CAMERA_SPEED = 1.f;

// Global data
std::unique_ptr<shader> mainShader;
std::unique_ptr<shader> lightingShader;
std::unique_ptr<shader> lightSourceShader;
unsigned int texContainer, texFace, texCapsule;
float deltaTime = 0.f;
float lastTime = 0.f;
auto cam1 = camera(glm::vec3(0, 0, 3));

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input_for_window(GLFWwindow* window);
void render_square();
bool initialize_shaders();
void initialize_textures();
void render_cubes();
void init_matrix_ubo();
void update_matrix_ubo(const glm::mat4&& view, const glm::mat4&& projection);
void bind_matrix_ubo(const GLuint shader);

std::unique_ptr<mesh> meshSphere;
std::unique_ptr<mesh> meshCube;
std::unique_ptr<mesh> meshCapsule;

void RenderLight() {
	const auto camX = sin(glfwGetTime()) * 1;
	const auto camZ = cos(glfwGetTime()) * 1;
	glm::vec3 lightPos(camX, 0.25, camZ);
	lightingShader->setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos); 
	model = glm::scale(model, glm::vec3(0.2f));
	lightSourceShader->use();
	lightSourceShader->setMatrix("model", model);
	meshSphere->Draw(); 
}

void RenderLitCube() {
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

	lightingShader->use();
	lightingShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightingShader->setVec3("lightColor", 1.f, 0.f, 1.0f); 
	lightingShader->setVec3("viewPos", cam1.get_pos());

	// SCALE TRANSLATE ROTATE
	for (int i = 0; i < 10; i++) {
		const float angle = 20.f * i;

		auto modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(.8));
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]); 
		modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		lightingShader->setMatrix("normalModel", glm::inverseTranspose(modelMatrix)); 
		lightingShader->setMatrix("model", modelMatrix);
		meshSphere->Draw();
	}
}

int main() {
	// Initialize the window optionally using opengl settings
	auto window = init_window({
		.glContextMajor = 3,
		.glContextMinor = 3,
		.glMSAASamples = 2,
		.width = 1600,
		.height = 900,
		.vsync = true,
		.msaa = true,
		.title = "Main Window"
		});

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

	// Main camera
	cam1.look_at({0, 0, 0});

	// Our main render loop
	meshSphere = resource_manager::load_mesh("sphere.mesh");
	meshCube = resource_manager::load_mesh("test.mesh");
	meshCapsule = resource_manager::load_mesh("capsule.mesh");

	while (!glfwWindowShouldClose(window)) {
		float curTime = glfwGetTime();
		deltaTime = curTime - lastTime;
		lastTime = curTime;
		
		glfwPollEvents();
		process_input_for_window(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_matrix_ubo(glm::perspective(glm::radians(60.f), 16.f / 9.f, 0.1f, 100.0f), cam1.matrix());

		RenderLight();
		RenderLitCube();
		//render_cubes();

		glfwSwapBuffers(window);
	}

	return 0;
}

void process_input_for_window(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Camera Movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam1.move_forward(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		cam1.move_backward(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam1.move_left(CAMERA_SPEED * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		cam1.move_right(CAMERA_SPEED * deltaTime);
	}
}

void render_cubes() {
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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texContainer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texFace);

	mainShader->setInt("tex1", 0);
	mainShader->setInt("tex2", 1);
	lightingShader->setInt("tex1", 0);

	for (int i = 0; i < 10; i++) { 
		auto modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(20.f * i), glm::vec3(1.0f, 0.3f, 0.5f));
		mainShader->setMatrix("model", modelMatrix);
		meshCube->Draw();
	}
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

void update_matrix_ubo(const glm::mat4&& view, const glm::mat4&& projection) {
	shader_data.view = view;
	shader_data.projection = projection;

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(shader_data.projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shader_data.view));
}

void bind_matrix_ubo(const GLuint shader) {
	unsigned int block_index = glGetUniformBlockIndex(shader, "shader_data"); 
	glUniformBlockBinding(shader, block_index, binding_point_index);
}

bool initialize_shaders() {
	mainShader = resource_manager::load_shader("vertex", "fragment");
	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	lightingShader = resource_manager::load_shader("lighting.vs", "lighting.fs");
	if (lightingShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightingShader->log << std::endl;
		return false;
	}

	lightSourceShader = resource_manager::load_shader("lighting.vs", "lightsource.fs");
	if (lightSourceShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightSourceShader->log << std::endl;
		return false;
	}

	mainShader->setInt("tex1", 0);
	mainShader->setInt("tex2", 1);

	lightingShader->setInt("tex1", 0);
	lightingShader->setInt("tex2", 1);

	return true;
}

void initialize_textures() {
	texContainer = resource_manager::load_texture("container.jpg");
	texFace = resource_manager::load_texture("awesomeface.png", true);
	texCapsule = resource_manager::load_texture("capsule0.jpg");
}
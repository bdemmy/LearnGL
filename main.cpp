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
#include "glm/glm/glm.hpp"

// Local includes
#include "shader.h"
#include "texture.h"
#include "window.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include "glm/glm/ext/matrix_transform.hpp"
#include "glm/glm/ext/matrix_clip_space.hpp"

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;

// Global data
std::unique_ptr<shader> mainShader;
unsigned int texture, texture2;
glm::vec3 v_cameraPos;
glm::vec3 v_cameraTarget;
glm::vec3 v_cameraDirection;
glm::vec3 v_cameraRight;
glm::vec3 v_cameraUp;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input_for_window(GLFWwindow* window);
void render_triangle();
void render_square();
bool initialize_shaders();
void initialize_textures();
void init_camera();

std::string load_file_to_str(const std::string& path) {
	const auto ifs = std::ifstream(path);
	auto sb = std::stringstream{};

	if (ifs) {
		sb << ifs.rdbuf();
	}

	return sb.str();
}

int main() {
	// Initialize the window optionally using opengl settings
	auto window = init_window(WINDOW_WIDTH, WINDOW_HEIGHT);

	// Initialize our shaders
	if (!initialize_shaders()) {
		return -1;
	}
	mainShader->use();

	// Generate and setup the textures (just one for now)
	initialize_textures();

	// Our main render loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		process_input_for_window(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		mainShader->setMatrix("model", glm::rotate(glm::mat4(1.0), glm::radians(-55.f), glm::vec3(1.0, 0.0, 0.0)));
		mainShader->setMatrix("projection", glm::perspective(glm::radians(60.f), 16.f / 9.f, 0.1f, 100.0f));
		mainShader->setMatrix("view", glm::translate(glm::mat4(1.0f), glm::vec3(0,0, -3.0)));

		render_square();

		glfwSwapBuffers(window);
	}

	return 0;
}

void process_input_for_window(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

struct vertex_t {
	float pos[3];
	float color[3];
	float uv[2];
};

void render_square() {
	const vertex_t vertices[] = {
			{.pos = {-1, -1, 0}, .color = {1, 1, 1}, .uv = {0, 0}},
			{.pos = {-1,  1, 0}, .color = {1, 1, 1}, .uv = {0, 1}},
			{.pos = { 1,  1, 0}, .color = {1, 1, 1}, .uv = {1, 1}},
			{.pos = { 1, -1, 0}, .color = {1, 1, 1}, .uv = {1, 0}},
	};

	const uint32_t indices[]{
			0, 1, 3, 1, 2, 3
	};

	static bool initialized = false;
	static unsigned int VBO;
	static unsigned int VAO;
	static unsigned int EBO;

	if (!initialized) {
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		initialized = true;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool initialize_shaders() {
	const auto VERT_SHADER_S = load_file_to_str("shaders/vertex");
	const auto FRAG_SHADER_S = load_file_to_str("shaders/fragment");

	mainShader = std::make_unique<shader>(VERT_SHADER_S, FRAG_SHADER_S);

	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	mainShader->setInt("texContainer", 0);
	mainShader->setInt("texFace", 1);

	return true;
}

void initialize_textures() {
	texture = load_texture("textures/container.jpg");
	texture2 = load_texture("textures/awesomeface.png", true);
}

void init_camera() {
	v_cameraPos = glm::vec3(0, 0, 3);
	v_cameraTarget = glm::vec3(0, 0, 0);
	v_cameraDirection = glm::normalize(v_cameraPos - v_cameraTarget);
	const auto up = glm::vec3(0, 1, 0);
	v_cameraRight = glm::normalize(glm::cross(up, v_cameraDirection));
	v_cameraUp = glm::cross(v_cameraDirection, v_cameraRight);
}
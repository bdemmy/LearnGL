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
#include "window.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;

// Global data
shader* mainShader;
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

void render_triangle() {
	const float vertexData[]{
			-0.7f, -0.3f, 0,
			-0.5f, 0.3f, 0,
			-0.3f, -0.3f, 0
	};

	static bool initialized = false;
	static unsigned int VBO;
	static unsigned int VAO;

	if (!initialized) {
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		initialized = true;
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

struct vertex_t {
	float pos[3];
	float color[3];
	float uv[2];
};

void render_square() {
	const vertex_t vertices[] = {
			{.pos = {-0.5, -0.5, 0}, .color = {0, 0, 1}, .uv = {0, 0}},
			{.pos = {-0.5,  0.5, 0}, .color = {1, 1, 0}, .uv = {0, 1}},
			{.pos = { 0.5,  0.5, 0}, .color = {1, 0, 0}, .uv = {1, 1}},
			{.pos = { 0.5, -0.5, 0}, .color = {0, 1, 0}, .uv = {1, 0}},
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

	mainShader = new shader(VERT_SHADER_S, FRAG_SHADER_S);

	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	mainShader->setInt("texContainer", 0);
	mainShader->setInt("texFace", 1);

	return true;
}

void initialize_textures() {
	// Load our image from disk
	int width, height, channels;
	auto* const data = stbi_load("textures/container.jpg", &width, &height, &channels, 0);

	// Create the texture object, bind it, copy the data, then gen the mipmaps
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Only do the last two if the file exists
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Free the texture
	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(true);
	auto* const data2 = stbi_load("textures/awesomeface.png", &width, &height, &channels, 0);

	// Create the texture object, bind it, copy the data, then gen the mipmaps
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// Only do the last two if the file exists
	if (data2) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Free the texture
	stbi_image_free(data2);
}

void init_camera() {
	v_cameraPos = glm::vec3(0, 0, 3);
	v_cameraTarget = glm::vec3(0, 0, 0);
	v_cameraDirection = glm::normalize(v_cameraPos - v_cameraTarget);
	const auto up = glm::vec3(0, 1, 0);
	v_cameraRight = glm::normalize(glm::cross(up, v_cameraDirection));
	v_cameraUp = glm::cross(v_cameraDirection, v_cameraRight);
}
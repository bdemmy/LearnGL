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
#include "texture.h"
#include "window.h"
#include "camera.h"
#include "mesh.h"
#include <glm/gtc/matrix_inverse.hpp>

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;

// Global data
std::unique_ptr<shader> mainShader;
std::unique_ptr<shader> lightingShader;
std::unique_ptr<shader> lightSourceShader;
unsigned int texture, texture2;
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

std::unique_ptr<mesh> testMesh;

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
	testMesh->Draw(); 
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

	for (int i = 0; i < 10; i++) {
		auto modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(.8));
		const float angle = 20.f * i;
		modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); 
		lightingShader->setMatrix("normalModel", glm::inverseTranspose(modelMatrix)); 
		lightingShader->setMatrix("model", modelMatrix);
		testMesh->Draw();
	}
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

	// Main camera
	cam1.set_yaw(-90); 

	// Our main render loop
	testMesh = std::make_unique<mesh>("meshes/sphere.mesh"); 

	while (!glfwWindowShouldClose(window)) {
		float curTime = glfwGetTime();
		deltaTime = curTime - lastTime;
		lastTime = curTime;

		glfwPollEvents();
		process_input_for_window(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cam1.set_pitch(0);

		mainShader->setMatrix("model", glm::rotate(glm::mat4(1.0), glm::radians(-55.f), glm::vec3(1.0, 0.0, 0.0)));
		mainShader->setMatrix("projection", glm::perspective(glm::radians(60.f), 16.f / 9.f, 0.1f, 100.0f));
		mainShader->setMatrix("view", cam1.matrix());

		lightSourceShader->setMatrix("projection", glm::perspective(glm::radians(60.f), 16.f / 9.f, 0.1f, 100.0f));
		lightSourceShader->setMatrix("view", cam1.matrix());

		lightingShader->setMatrix("projection", glm::perspective(glm::radians(60.f), 16.f / 9.f, 0.1f, 100.0f));
		lightingShader->setMatrix("view", cam1.matrix());

		//render_cubes(); 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		RenderLight();
		RenderLitCube();

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
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	for (int i = 0; i < 10; i++) { 
		auto modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
		const float angle = 20.f * i;
		modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		mainShader->setMatrix("model", modelMatrix);
		testMesh->Draw();
	}
}
 
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
	mainShader = std::make_unique<shader>("shaders/vertex", "shaders/fragment");
	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	lightingShader = std::make_unique<shader>("shaders/lighting.vs", "shaders/lighting.fs");
	if (lightingShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightingShader->log << std::endl;
		return false;
	}

	lightSourceShader = std::make_unique<shader>("shaders/lighting.vs", "shaders/lightsource.fs");
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
	texture = load_texture("textures/container.jpg");
	texture2 = load_texture("textures/awesomeface.png", true);
}
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
constexpr auto MOUSE_SENSITIVITY = 0.1f;

// Global data
std::unique_ptr<shader> mainShader;
std::unique_ptr<shader> lightingShader;
std::unique_ptr<shader> lightSourceShader;
std::unique_ptr<shader> skyboxShader;
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
void render_cubes();
void init_matrix_ubo();
void update_matrix_ubo(const glm::mat4&& view, const glm::mat4&& projection, const glm::vec3&& cameraPosition);
void bind_matrix_ubo(const GLuint shader);

std::unique_ptr<mesh> meshSphere;
std::unique_ptr<mesh> meshCube;
std::unique_ptr<mesh> meshCapsule;
std::unique_ptr<mesh> meshCooper;
std::unique_ptr<mesh> meshTerrain;
std::unique_ptr<mesh> meshSkybox;

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

	lightingShader->use();
	lightingShader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
	lightingShader->setVec3("lightColor", 1.f, 1.f, 1.0f);
	lightingShader->setVec3("viewPos", cam1.get_pos());

	static float rotation = 0.f;
	rotation += 120.f * deltaTime;

	// SCALE TRANSLATE ROTATE
	for (auto i = 0; i < 10; i++) {
		const auto angle = (20.f * i);

		auto modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(.8));
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f));
		//modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		lightingShader->setMatrix("normalModel", glm::inverseTranspose(modelMatrix));
		lightingShader->setMatrix("model", modelMatrix);
		lightingShader->setInt("textured", 1);
		meshSphere->Draw();
	}
}

void RenderTerrain() {
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f)
	};

	lightingShader->use();
	lightingShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightingShader->setVec3("lightColor", 1.f, 1.f, 1.0f);
	lightingShader->setVec3("viewPos", cam1.get_pos());

	// SCALE TRANSLATE ROTATE
	for (auto i = 0; i < 1; i++) {
		auto modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(.02));
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]);

		lightingShader->setMatrix("normalModel", glm::inverseTranspose(modelMatrix));
		lightingShader->setMatrix("model", modelMatrix);
		lightingShader->setInt("textured", 1);
		meshTerrain->Draw();
	}
}

void RenderSkybox() {
	glDepthMask(GL_FALSE);
	skyboxShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);
	skyboxShader->setInt("skybox", 0);
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
	meshSphere = resource_manager::load_mesh("sphere.mesh");
	meshSkybox = resource_manager::load_mesh("skybox.mesh");
	//meshCube = resource_manager::load_mesh("test.mesh");
	//meshCapsule = resource_manager::load_mesh("capsule.mesh");
	//meshCooper = resource_manager::load_mesh("cooper.mesh");
	//meshTerrain = resource_manager::load_mesh("terrain.mesh");

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
		//render_cubes();
		//RenderTerrain();

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
	cam1.adjust_fov(-2 * yoffset);
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
	mainShader = resource_manager::load_shader("vertex.vert", "fragment.frag");
	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	lightingShader = resource_manager::load_shader("lighting.vert", "lighting.frag");
	if (lightingShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightingShader->log << std::endl;
		return false;
	}

	lightSourceShader = resource_manager::load_shader("lighting.vert", "lightsource.frag");
	if (lightSourceShader->error) {
		std::cerr << "Error compiling shaders: \n" << lightSourceShader->log << std::endl;
		return false;
	}

	skyboxShader = resource_manager::load_shader("skybox.vert", "skybox.frag");
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

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
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
	texSkybox = loadCubemap(skybox_faces);
}
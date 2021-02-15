//
// Created by demmyb on 8/13/20.
//

const char* VERT_SHADER_S =
R"====(

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV;

out vec3 bColor;
out vec2 bUV;

void main() {
    gl_Position = vec4(aPos.xyz, 1.0);

    bColor = aColor;
    bUV = aUV;
}

)====";

const char* FRAG_SHADER_S =
R"====(

#version 330 core

in vec3 bColor;
in vec2 bUV;

out vec4 FragColor;

uniform sampler2D texContainer;
uniform sampler2D texFace;

void main() {
    FragColor = mix(texture(texContainer, bUV), texture(texFace, bUV), 0.15);
}

)===="
;

// STD includes
#include <iostream>

// Library includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// Local includes
#include "shader.h"

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;

// Global data
shader* mainShader;
unsigned int texture, texture2;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input_for_window(GLFWwindow* window);
void render_triangle();
void render_square();
bool initialize_shaders();
void initialize_textures();

int main() {
	// Initialize GLFW and specify our profile
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Attempt to spawn a window, if this fails, notify the user and exit
	auto* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "First Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to initialize GLFW window." << std::endl;

		glfwTerminate();
		return -1;
	}

	// If we succeeded then we need to hold onto the context
	glfwMakeContextCurrent(window);

	// Now initialize glad, or throw an error if we can't
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::cerr << "Failed to initialize GLAD." << std::endl;

		return -1;
	}

	// Set our viewport.  Match the window dimensions as we aren't doing anything outside of the viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Set our callback for when we resize the window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
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
	mainShader = new shader(&VERT_SHADER_S, &FRAG_SHADER_S);

	if (mainShader->error) {
		std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
		return false;
	}

	mainShader->setInt("texFace", 1);

	return true;
}

void initialize_textures() {
	// Load our image from disk
	int width, height, channels;
	auto* const data = stbi_load("container.jpg", &width, &height, &channels, 0);

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
	auto* const data2 = stbi_load("awesomeface.png", &width, &height, &channels, 0);

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
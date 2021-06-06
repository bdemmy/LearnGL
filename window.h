
#ifndef OPENGL_CONTEXTV_MAJOR
#define OPENGL_CONTEXTV_MAJOR 3
#endif

#ifndef OPENGL_CONTEXTV_MINOR
#define OPENGL_CONTEXTV_MINOR 3
#endif

#include <glad/glad.h>
#include <glfw/glfw3.h>

auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void {
	glViewport(0, 0, width, height);
}

auto init_window(int width, int height) -> GLFWwindow* {
	// Initialize GLFW and specify our profile
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_CONTEXTV_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_CONTEXTV_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Attempt to spawn a window, if this fails, notify the user and exit
	auto* window = glfwCreateWindow(width, height, "First Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to initialize GLFW window." << std::endl;

		glfwTerminate();
		return nullptr;
	}

	// If we succeeded then we need to hold onto the context
	glfwMakeContextCurrent(window);

	// Now initialize glad, or throw an error if we can't
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::cerr << "Failed to initialize GLAD." << std::endl;

		return nullptr;
	}

	// Set our viewport.  Match the window dimensions as we aren't doing anything outside of the viewport
	glViewport(0, 0, width, height);

	// Set our callback for when we resize the window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	glfwSwapInterval(1);

	return window;
}
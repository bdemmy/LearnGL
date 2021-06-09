#include <glad/glad.h>
#include <glfw/glfw3.h>

struct window_config_t {
	unsigned glContextMajor, glContextMinor;
	unsigned glMSAASamples;
	unsigned width, height;
	bool vsync, msaa;
	const char* title;
};

auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void {
	glViewport(0, 0, width, height);
}

GLFWwindow* init_window(window_config_t config) {
	// Initialize GLFW and specify our profile
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.glContextMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.glContextMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// MSAA
	if (config.msaa) {
		glfwWindowHint(GLFW_SAMPLES, config.glMSAASamples);
	}

	// Attempt to spawn a window, if this fails, notify the user and exit
	auto* window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
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
	glViewport(0, 0, config.width, config.height);

	// Set our callback for when we resize the window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Z Test
	glEnable(GL_DEPTH_TEST);

	// Face culling, clockwise triangles are front
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	// Enable MSAA
	if (config.msaa) {
		glEnable(GL_MULTISAMPLE);
	}

	glfwSwapInterval(config.vsync ? 1 : 0);

	return window;
}

//
// Created by demmyb on 8/13/20.
//

#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "shader.h"

// Constant data
constexpr auto WINDOW_WIDTH = 1366;
constexpr auto WINDOW_HEIGHT = 768;

const char *VERT_SHADER_S =
        #include "shaders/vertex"

const char *FRAG_SHADER_S =
        #include "shaders/fragment"

// Global data
shader *mainShader;
unsigned int texture, texture2;


// Function prototypes
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void processInputForWindow(GLFWwindow *window);

void renderTriangle();

void renderSquare();

int initializeShaders();

void initializeTextures();

int main() {
    // Initialize GLFW and specify our profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Attempt to spawn a window, if this fails, notify the user and exit
    auto *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "First Window", nullptr, nullptr);
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
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize our shaders
    if (!initializeShaders()) {
        return -1;
    }
    mainShader->use();

    // Generate and setup the textures (just one for now)
    initializeTextures();

    // Our main render loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInputForWindow(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderSquare();

        glfwSwapBuffers(window);
    }

    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInputForWindow(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void renderTriangle() {
    const float vertexData[]{
            -0.7, -0.3, 0,
            -0.5, 0.3, 0,
            -0.3, -0.3, 0
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

void renderSquare() {
    const vertex_t vertices[] = {
            {.pos = {-0.5, -0.5, 0}, .color = {0, 0, 1}, .uv = {0, 0}},
            {.pos = {-0.5,  0.5, 0}, .color = {1, 1, 0}, .uv = {0, 1}},
            {.pos = { 0.5,  0.5, 0}, .color = {1, 0, 0}, .uv = {1, 1}},
            {.pos = { 0.5, -0.5, 0}, .color = {0, 1, 0}, .uv = {1, 0}},
    };

    const uint indices[]{
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

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
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

int initializeShaders() {
    mainShader = new shader(&VERT_SHADER_S, &FRAG_SHADER_S);

    if (mainShader->error) {
        std::cerr << "Error compiling shaders: \n" << mainShader->log << std::endl;
        return 0;
    }

    mainShader->setInt("texFace", 1);

    return 1;
}

void initializeTextures() {
    // Load our image from disk
    int width, height, channels;
    const auto data = stbi_load("container.jpg", &width, &height, &channels, 0);

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
    const auto data2 = stbi_load("awesomeface.png", &width, &height, &channels, 0);

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
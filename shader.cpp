//
// Created by demmyb on 8/17/20.
//

#include <glad/glad.h>
#include "shader.h"

#include <iostream>

int compileShader(GLuint shader, char *log) {
    int status;

    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glGetShaderInfoLog(shader, 512, nullptr, log);

        return 0;
    }

    return 1;
}

shader::shader(const char **vertex, const char **fragment) {
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, vertex, nullptr);
    glShaderSource(fragment_shader, 1, fragment, nullptr);

    log = new char[512];

    if (!::compileShader(vertex_shader, log)) {
        std::cerr << "Error compiling vertex shader\n" << log << std::endl;

        glDeleteShader(vertex_shader);

        error = true;
        return;
    }

    if (!::compileShader(fragment_shader, log)) {
        std::cerr << "Error compiling fragment shader\n" << log << std::endl;

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        error = true;
        return;
    }

    m_uProgram = glCreateProgram();
    glAttachShader(m_uProgram, vertex_shader);
    glAttachShader(m_uProgram, fragment_shader);
    glLinkProgram(m_uProgram);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void shader::setFloat(const char *name, const float &v) {
    const auto location = glGetUniformLocation(m_uProgram, name);
    glUseProgram(m_uProgram);
    glUniform1f(location, v);
}

void shader::setInt(const char *name, const int &v) {
    const auto location = glGetUniformLocation(m_uProgram, name);
    glUseProgram(m_uProgram);
    glUniform1i(location, v);
}

void shader::use() {
    glUseProgram(m_uProgram);
}

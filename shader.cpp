//
// Created by demmyb on 8/17/20.
//

#include <glad/glad.h>
#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/glm/gtc/type_ptr.hpp"

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

std::string load_file_to_str(const std::string& path) {
    const auto ifs = std::ifstream(path);
    auto sb = std::stringstream{};

    if (ifs) {
        sb << ifs.rdbuf();
    }

    return sb.str();
}

shader::shader(const std::string &vertex, const std::string& fragment) {
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const auto vertex_str = load_file_to_str(vertex);
    const auto vertex_source = vertex_str.c_str();

    const auto frag_str = load_file_to_str(fragment);
    const auto frag_source = frag_str.c_str();

    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glShaderSource(fragment_shader, 1, &frag_source, nullptr);

    log = new char[512];

    if (!compileShader(vertex_shader, log)) {
        std::cerr << "Error compiling vertex shader\n" << log << std::endl;

        glDeleteShader(vertex_shader);

        error = true;
        return;
    }

    if (!compileShader(fragment_shader, log)) {
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

void shader::setMatrix(const char* name, const glm::mat4& v) {
    const auto location = glGetUniformLocation(m_uProgram, name);
    glUseProgram(m_uProgram);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
}

void shader::setVec3(const char* name, const glm::vec3&& v) {
    const auto location = glGetUniformLocation(m_uProgram, name);
    glUseProgram(m_uProgram);
    glUniform3f(location, v.x, v.y, v.z);
}

void shader::setVec3(const char* name, const float x, const float y, const float z) {
    setVec3(name, glm::vec3(x, y, z));
}

void shader::use() {
    glUseProgram(m_uProgram);
}


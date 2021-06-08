//
// Created by demmyb on 8/17/20.
//

#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

#include <string>
#include "glm/glm/matrix.hpp"

inline struct shader_data {
    glm::mat4 view;
    glm::mat4 projection;
} shader_data;

class shader {
public:
    bool error = false;
    char *log = nullptr;

private:
    unsigned int m_uProgram;

public:
    shader(const std::string& vertex, const std::string& fragment);

    void use();

    void setFloat(const char *name, const float &v);

    void setInt(const char *name, const int &v);

    void setMatrix(const char* name, const glm::mat4& v);

    void setVec3(const char* name, const glm::vec3&& v);

    void setVec3(const char* name, const float x, const float y, const float z);

    const unsigned int getProgram() const;
};


#endif //LEARNGL_SHADER_H

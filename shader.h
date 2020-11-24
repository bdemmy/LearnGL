//
// Created by demmyb on 8/17/20.
//

#ifndef LEARNGL_SHADER_H
#define LEARNGL_SHADER_H

#include <string>

class shader {
public:
    bool error = false;
    char *log = nullptr;

private:
    unsigned int m_uProgram;

public:
    shader(const char **vertex, const char **fragment);

    void use();

    void setFloat(const char *name, const float &v);

    void setInt(const char *name, const int &v);
};


#endif //LEARNGL_SHADER_H

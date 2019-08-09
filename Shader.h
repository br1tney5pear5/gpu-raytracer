#pragma once

#include <vector>
#include <variant>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

using UniformName = std::string;
using UniformValue = std::variant<
    GLint , GLint* ,
    GLuint , GLuint* ,
    GLfloat , GLfloat*>;

struct GL_UniformInfo {
    GLint   location;
    GLsizei size;
    GLenum  type;
};
class Shader {
public:

    constexpr static size_t UNIFORM_NAME_MAX_LEN = 32;
    Shader();
    GLuint program;
    Shader(const GLchar*, const GLchar* );
    Shader(const GLchar*, const GLchar*, const GLchar* );
    void use();
    void set_uniforms(std::vector<std::pair<UniformName,UniformValue>> supplied_uniforms);
    void set_uniform(UniformName name,UniformValue value);

    void set_sampler2D_uniform(UniformName name,UniformValue value);
    void set_float_uniform(UniformName name,    UniformValue value);
    void set_vec2_uniform (UniformName name,    UniformValue value);
    void set_vec3_uniform (UniformName name,    UniformValue value);
    void set_vec4_uniform (UniformName name,    UniformValue value);

    void set_uniform(UniformName name, GLenum type ,UniformValue value);
private:


    //TODO: rename to create_gl_shader ?
    GLuint create_shader(GLenum shader_type, const GLchar* shader_path);
    std::vector<std::pair<UniformName,GL_UniformInfo>> get_uniforms_list();
    void set_specific_uniform(GL_UniformInfo info, UniformValue value);
};

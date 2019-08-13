#include "Shader.h"
#include "Logger.h"
#include <assert.h>

Shader::Shader(){}
Shader::Shader(const GLchar* vert_path, const GLchar* frag_path)
    : Shader(vert_path, nullptr, frag_path)
{}
Shader::Shader(const GLchar* vert_path, const GLchar* geom_path, const GLchar* frag_path){
    GLuint vert, geom, frag;

    program = glCreateProgram();

    vert = create_shader(GL_VERTEX_SHADER  , vert_path);
    glAttachShader(program, vert);

    if(geom_path != nullptr){
        geom = create_shader(GL_GEOMETRY_SHADER, geom_path);
        glAttachShader(program, geom);
    }

    frag = create_shader(GL_FRAGMENT_SHADER, frag_path);
    glAttachShader(program, frag);

    GLint status;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(!status){ 
        LOG( "shader linking failed" ); 
        char info_log[256];
        glGetProgramInfoLog(program, sizeof(info_log), NULL, &info_log[0]);
        LOG(info_log);
    }

    glDeleteShader(vert);
    if(geom_path != nullptr)
        glDeleteShader(geom);
    glDeleteShader(frag);
}

void Shader::use(){
    glUseProgram(this->program);
}

std::vector<std::pair<UniformName,GL_UniformInfo>> Shader::get_uniforms_list(){
    int uniforms_count;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniforms_count);

    std::vector<std::pair<UniformName, GL_UniformInfo>> ret((size_t) uniforms_count);

    for(int i=0; i<uniforms_count; i++){
        char cname[Shader::UNIFORM_NAME_MAX_LEN];

        GLsizei namelen, size; GLenum type;
        glGetActiveUniform(program, (GLuint)i, Shader::UNIFORM_NAME_MAX_LEN, 
                &namelen , &size,  &type, cname);

        GLint location = glGetUniformLocation(program, cname);

        ret[i] = std::pair<UniformName, GL_UniformInfo>(
            std::string(cname),
            GL_UniformInfo{location, size, type}
        );
    }
    return ret;
}

GLuint Shader::create_shader(GLenum shader_type, const GLchar* shader_path){
    const char * shader_name;
    switch(shader_type){
        case GL_VERTEX_SHADER:
            shader_name = "vertex"; break;
        case GL_GEOMETRY_SHADER:
            shader_name = "geometry"; break;
        case GL_FRAGMENT_SHADER:
            shader_name = "fragment"; break;
    }
    std::string shader_code;
    std::ifstream shader_file;

    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream shader_stream;
    try{
        shader_file.open(shader_path);
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        shader_code = shader_stream.str();
    }catch(std::ifstream::failure e){
        LOG("reading ", shader_name, " shader file failed.");
    }
    GLint status;
    GLuint shader = glCreateShader(shader_type);
    auto shader_code_cstr = shader_code.c_str();

    glShaderSource(shader, 1, &shader_code_cstr, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(!status){
        LOG(shader_name, " shader compilation failed.");
        char info_log[256];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, &info_log[0]);
        LOG(info_log);
    }
    return shader;
}
void Shader::set_uniforms(std::vector<std::pair<UniformName,UniformValue>> supplied_uniforms){
    auto unifoms_map = get_uniforms_list();
    for(auto& s_uni : supplied_uniforms){
        for(auto& uni : unifoms_map){
            if(std::get<UniformName>(uni) == std::get<UniformName>(s_uni))
                set_specific_uniform(std::get<GL_UniformInfo>(uni), std::get<UniformValue>(s_uni));
        }
    }
}

void Shader::set_uniform(UniformName, UniformValue ){
    //GLint location = glGetUniformLocation(program, name.c_str());
    //auto cname = name.c_str(); 
    //uint32_t index;
    //GL_UniformInfo info;
    //glGetUnformIndices(program, 1,  &cname, &index); 
    assert(0);
}

void Shader::set_uniform(UniformName name, GLenum type, UniformValue value){
    GLint location = glGetUniformLocation(program, name.c_str());
    GL_UniformInfo info{location, 1, type};
    set_specific_uniform(info, value);
}

void Shader::set_specific_uniform(GL_UniformInfo info, UniformValue value){
    switch(info.type){
        case GL_SAMPLER_2D:
            glUniform1i(info.location, std::get<GLuint>(value));
            break;
        case GL_FLOAT:
            glUniform1f(info.location, std::get<GLfloat>(value));
            break;
        case GL_FLOAT_VEC2:
            glUniform2fv(info.location, info.size, std::get<GLfloat*>(value));
            break;
        case GL_FLOAT_VEC3:
            glUniform3fv(info.location, info.size, std::get<GLfloat*>(value));
            break;
        case GL_FLOAT_VEC4:
            glUniform4fv(info.location, info.size, std::get<GLfloat*>(value));
            break;
        case GL_UNSIGNED_INT:
            glUniform1ui(info.location, std::get<GLuint>(value));
            break;
        case GL_UNSIGNED_INT_VEC2:
            glUniform2uiv(info.location, info.size, std::get<GLuint*>(value));
            break;
        case GL_UNSIGNED_INT_VEC3:
            glUniform3uiv(info.location, info.size, std::get<GLuint*>(value));
            break;
        case GL_UNSIGNED_INT_VEC4:
            glUniform4uiv(info.location, info.size, std::get<GLuint*>(value));
            break;
        case GL_INT:
            glUniform1i(info.location, std::get<GLint>(value));
            break;
        case GL_INT_VEC2:
            glUniform2iv(info.location, info.size, std::get<GLint*>(value));
            break;
        case GL_INT_VEC3:
            glUniform3iv(info.location, info.size, std::get<GLint*>(value));
            break;
        case GL_INT_VEC4:
            glUniform4iv(info.location, info.size, std::get<GLint*>(value));
            break;
        case GL_FLOAT_MAT2:
            glUniformMatrix2fv(info.location, info.size, GL_FALSE, std::get<GLfloat*>(value));
            break;
        case GL_FLOAT_MAT3:
            glUniformMatrix3fv(info.location, info.size, GL_FALSE, std::get<GLfloat*>(value));
            break;
        case GL_FLOAT_MAT4:
            glUniformMatrix4fv(info.location, info.size, GL_FALSE, std::get<GLfloat*>(value));
            break;
        default:
            assert(0);
    }
}

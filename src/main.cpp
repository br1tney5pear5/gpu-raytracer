#define GLFW_RAW_MOUSE_MOTION
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Logger.h"
#include "modular-glsl.h"

float quad_vertices[] = {
    -1.0f,-1.0f, 0.0f,
     1.0f,-1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
     1.0f, 1.0f, 0.0f
};

static void key_callback(GLFWwindow* win,
                         int key, int /*scancode*/, int action, int /*mods*/){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win, GLFW_TRUE);
}

GLFWwindow * window;
int main(){
    if( !glfwInit() ) {
        std::cout << "glfwInit() failed";
        return 1;}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(1024,1024, "byheart", NULL, NULL);
    if( !window ) {
        std::cout << "glfwCreateWindow() failed";
        return 2;}

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if( glewInit() ) {
        std::cout << "glewInit() failed";
        return 3;}

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);


    ShaderBuilder builder;
    std::error_code ec;

    builder.register_log_callback([](std::string msg) -> void {
                                      std::cout << msg << '\n'; });

    builder.add_include_dir("../shaders/", ec);
    if(ec) { std::cout << ec.message(); }

    builder.import_modules_from_file("glslmodules", ec);
    if(ec) { std::cout << ec.message(); }

    builder.set_header ("#version 450\n");

    std::string vert_src = builder.build("mainvert", ec);
    if(ec) { std::cout << ec.message(); }

    std::string frag_src = builder.build("mainfrag", ec);
    if(ec) { std::cout << ec.message(); }

    std::ofstream vert_file("shader.vert");
    std::ofstream frag_file("shader.frag");

    vert_file << vert_src;
    frag_file << frag_src;

    // std::cout << vert_src;
    // std::cout << "------------------\n";
    // std::cout << frag_src;

    vert_file.close();
    frag_file.close();

    Shader shader("shader.vert", "shader.frag");

    GLuint quad_VAO, quad_VBO;
    glGenVertexArrays(1, &quad_VAO);
    glGenBuffers     (1, &quad_VBO);

    glBindVertexArray                 (quad_VAO);
    glBindBuffer     (GL_ARRAY_BUFFER, quad_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 3*sizeof(float), (void*)0);
    glBindVertexArray                 (0);
    glBindBuffer     (GL_ARRAY_BUFFER, 0);

    GLuint ssbo;

    // glGenBuffers(1, &ssbo);
    // glBindVertexArray                 (quad_VAO);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    // glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * teapot_count,
    //              teapot, GL_STATIC_DRAW);
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind


    shader.use();

    glBindVertexArray(quad_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents(); 
    }
    return 0;
}//main



#include <chrono>

#define GLFW_RAW_MOUSE_MOTION
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Logger.h"

#include <stdint.h>
#include <stdlib.h>


#include <string.h>

glm::vec2 input_axis;
float cam_angle = 0, cam_dist = 0;

static void key_callback(GLFWwindow* win, int key, int /*scancode*/, int action, int /*mods*/){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win, GLFW_TRUE);
}

GLFWwindow* window;
constexpr size_t screen_x=1024, screen_y=1024;
constexpr size_t pixels_count = screen_x * screen_y;

struct Pixel {
    float x,y;
    float r,g,b;
};
Pixel * pixels = new Pixel[pixels_count];


void randomize_pixels() {
    for(size_t i=0; i<(pixels_count); i++) {
        pixels[pixels_count].x = float(0.5);
        pixels[pixels_count].y = float(i / screen_x);
        pixels[pixels_count].r = 1.0f;
        pixels[pixels_count].g = 1.0f;
    }
}
int main(){
    randomize_pixels();
//=============================================================================
//init
    if( !glfwInit() ){ LOG( "!glfwInit\n"); return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR ,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR ,2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    window = glfwCreateWindow(screen_x, screen_y, "floating", NULL, NULL);
    if(!window){ LOG("!window\n"); glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    //glfwSetWindowSizeCallback(window, window_size_callback);
    //
//=============================================================================
//shader

    Shader pass("../shaders/pass.vert", "../shaders/pass.frag");
//=============================================================================
//binding

    GLuint pixels_VAO, pixels_VBO;
    glGenVertexArrays(1, &pixels_VAO);
    glGenBuffers(1, &pixels_VBO);
    glBindVertexArray(pixels_VAO); {
        glBindBuffer(GL_ARRAY_BUFFER, pixels_VBO); {
            glBufferData(GL_ARRAY_BUFFER, sizeof(pixels),
                         &pixels, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0); //position
            glVertexAttribPointer(0,2,GL_FLOAT ,GL_FALSE, 5*sizeof(float), (void*)0);
            glEnableVertexAttribArray(1); //color
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
        } glBindBuffer(GL_ARRAY_BUFFER, 0);
    } glBindVertexArray(0);

    // GLuint quad_VAO, quad_VBO;
    // glGenVertexArrays(1, &quad_VAO);
    // glGenBuffers(1, &quad_VBO);
    // glBindVertexArray(quad_VAO); {
    //     glBindBuffer(GL_ARRAY_BUFFER, quad_VBO); {
    //         glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    //         glEnableVertexAttribArray(0);
    //         glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 5*sizeof(float), (void*)0);
    //         glEnableVertexAttribArray(1);
    //         glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    //     } glBindBuffer(GL_ARRAY_BUFFER, 0);
    // } glBindVertexArray(0);
//=============================================================================
//ping pong framebuffers
    // const GLuint fbo_count = 1;
    // const GLuint tex_per_fbo = 1;


    // GLuint fbo[fbo_count];
    // GLuint tex[fbo_count * tex_per_fbo];
    // glGenFramebuffers(fbo_count, fbo);
    // glGenTextures(fbo_count * tex_per_fbo, tex);

    // for(GLuint i=0; i<fbo_count; i++){
    //     glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
    //     for(GLuint j=0; j<tex_per_fbo; j++){
    //     glBindTexture(GL_TEXTURE_2D, tex[i * tex_per_fbo + j]);
    //         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_x, screen_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //         glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j, GL_TEXTURE_2D, tex[i * tex_per_fbo + j], 0);

    //         // glBindRenderbuffer(GL_RENDERBUFFER, depth_buf[i]);
    //         // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_x/downscale, screen_y/downscale);
    //         // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf[i]);
    //     }
    // }


//=============================================================================
//3d camera


    float current_time=0, last_time=0, delta_time=0, time_passed=0;

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    while(!glfwWindowShouldClose(window)){
        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;
        time_passed = glfwGetTime();
        cam_angle += delta_time * input_axis.x;
        cam_dist -= delta_time * input_axis.y;
        cam_dist = glm::clamp(cam_dist, 0.0f, 100000.0f);

        glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//=============================================================================
        auto draw_stuff = [&]() {

            glm::vec3 color(0.5f,0.7f,0.9f);

            glClear(GL_COLOR_BUFFER_BIT);

            glViewport(0,0,screen_x,screen_y);

            pass.use();
            glm::vec2 resolution = glm::vec2((float)screen_x, (float)screen_y);
            pass.set_uniform("resolution", GL_FLOAT_VEC2, glm::value_ptr(resolution));
            glBindBuffer(GL_ARRAY_BUFFER, pixels_VBO);
            glBindVertexArray(pixels_VAO);
            glDrawArrays(GL_POINTS, 0, pixels_count);
        };

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDrawBuffer(GL_COLOR_ATTACHMENT0);
        draw_stuff();

//=============================================================================
/*
 * wadwadwadl
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        pass_shader.use();
        for(GLuint i=0; i<fbo_count; i++){
            int dy = screen_y/fbo_count;
            for(GLuint j=0; j<tex_per_fbo; j++){
                int dx = screen_x/tex_per_fbo;
                glViewport(i*dx,i*dy,dx,dy);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex[i * tex_per_fbo + j] );

                glBindVertexArray(quad_VAO); ;
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
*/
//=============================================================================

        glfwSwapBuffers(window);
        glfwPollEvents(); 
        // glfwSetWindowShouldClose(window, GLFW_TRUE);

    }// while(!glfwWindowShouldClose(window))
    glDeleteVertexArrays(1, &pixels_VAO);
    glDeleteBuffers(1, &pixels_VBO);
    glfwTerminate();
    return 0;
}//main


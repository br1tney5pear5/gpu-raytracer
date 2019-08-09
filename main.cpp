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

    return 0;
}//main


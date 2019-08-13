#include <chrono>
#include <regex>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <map>
#include <tgmath.h>
#include <filesystem>

#define GLFW_RAW_MOUSE_MOTION
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Logger.h"
#include "Logger.h"
#include "ShaderBuilder.h"
//#include "Ray.h"
//#include "Sphere.h"
//#include "Hitable.h"
//#include "Camera.h"

#include <stdint.h>
#include <stdlib.h>

#include <string.h>

namespace fs = std::filesystem;
// glm::vec2 input_axis;
// float cam_angle = 0, cam_dist = 0;

// constexpr size_t width=300, height=300;

static void key_callback(GLFWwindow* win, int key, int /*scancode*/, int action, int /*mods*/){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win, GLFW_TRUE);
}


int main(){
    ShaderBuilder builder;
    std::vector<std::string> include_directories;
    builder.add_include_dir("/shared/projects/raytracer/shaders/");
    builder.add_module("main.frag");
    builder.add_module("quaternion.glsl");
    builder.add_module("constants.glsl");
    builder.add_module("rays_util.glsl");
    builder.add_module("random.glsl");

    std::error_code ec;

    auto source = builder.build("mainfrag",ec);

    CON();
    auto sorted_modules = builder.get_sorted_modules_list();
    for(auto m : sorted_modules) std::cout << m.name << "\n";

    CON();
    std::cout << source;

    


    return 0;
}//main



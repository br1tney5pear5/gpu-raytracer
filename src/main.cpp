#include <chrono>
#include <regex>
#include <tuple>
#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <vector>
#include <initializer_list>
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



int main(){
    ShaderBuilder builder;
    std::vector<std::string> include_directories;
    builder.add_include_dir("/shared/projects/raytracer/shaders/");
    builder.import_modules_from_file("glslmodules");
    builder.build("mainfrag");

    std::error_code ec;

    using namespace std::chrono_literals;
    // LOG(sizeof(std::filesystem::file_time_type));


    // while(true) {
    //     std::this_thread::sleep_for(1.0s);
    //     if(true || ftime != last_ftime) {
    //         builder.clear_modules();
    //         builder.import_modules_from_file("glslmodules");

    //         std::ofstream outfile("../shaders/output.frag");
    //         outfile << builder.build("mainfrag", ec);
    //         outfile.close();

    //         modules.close();
    //         last_ftime = ftime;
    //     }
    // }
    return 0;
}//main



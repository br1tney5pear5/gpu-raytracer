// #define GLFW_RAW_MOUSE_MOTION
// #define GLEW_STATIC
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>

//GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>



#include <chrono>
#include <thread>

#include "Shader.h"
#include "Logger.h"
#include "Logger.h"

#include "ShaderBuilder.h"


int main(){
    ShaderBuilder builder;
    builder.add_include_dir("/shared/projects/raytracer/shaders/");

    using namespace std::chrono_literals;

    std::string source;

    std::error_code ec;
    builder.import_modules_from_file("glslmodules", ec);

    LOG(ec.message());

    builder.build("mainfrag", ec);


    // while(true) {
    //     std::cout << "Checking for changes\n";

    //     std::this_thread::sleep_for(1.0s);

        auto flag = builder.hot_rebuild("mainfrag", source);

    //     if(flag) {
    //         std::cout << source;
    //     }
    // }
    return 0;
}//main



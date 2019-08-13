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
float quad_vertices[] = {
                         -1.0f,-1.0f, 0.0f,  0.0f, 0.0f,
                         1.0f,-1.0f, 0.0f,  1.0f, 0.0f,
                         -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
                         1.0f, 1.0f, 0.0f,  1.0f, 1.0f
};


GLFWwindow* window;

int main(){
    //=============================================================================
    //init
    //if( !glfwInit() ){ LOG( "!glfwInit\n"); return -1; }
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR ,3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR ,2);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    //window = glfwCreateWindow(width, height, "floating", NULL, NULL);
    //if(!window){ LOG("!window\n"); glfwTerminate(); return -1; }
    //glfwMakeContextCurrent(window);
    //glewExperimental = GL_TRUE;
    //glewInit();
    //glfwSwapInterval(1);
    //glfwSetKeyCallback(window, key_callback);

    //glEnable(GL_PROGRAM_POINT_SIZE);
 
    // Shader shader("../shaders/pass0.vert", "../shaders/pass0.frag");

    std::string shader_path = "../shaders/pass0.frag";
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
        LOG("reading shader file failed.");
    }
    // auto shader_code_cstr = shader_code.c_str();

    // for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    //     std::smatch match = *i;
    //     std::string match_str = match.str();
    //     if (match_str.size() > N) {
    //         std::cout << "  " << match_str << '\n';
    //     }
    // }
    // (^__[a-z_]+)(?:\s+)(.*$)
    LOG("REGEX TEST");

    auto line_no_at = [](const std::string& str, size_t pos) -> size_t {
                          size_t line_no = 1;
                          for(size_t i=0; i<pos; ++i)
                              if(str[i] == '\n') line_no++;
                          return line_no;
                      };
    // std::regex preprocess_directive_regex("(^__[a-z_]+)(?:\\s+)(.*$)");
    std::regex preprocess_directive_regex("((?:__)[a-z_]+)(?:\\s+)(.*)",
                                          std::regex_constants::ECMAScript
                                          // std::regex_constants::multiline
                                          );
    // std::regex quotes_regex();

    //       line no        delete       include
    std::map<int, std::pair<std::string, std::string>> edit_map;


    std::vector<std::string> include_directories;
    include_directories.push_back("/shared/projects/raytracer/");
    include_directories.push_back("/shared/projects/raytracer/src/");
    include_directories.push_back("/shared/projects/raytracer/shaders/");

    auto begin = std::sregex_iterator(shader_code.cbegin(),
                                     shader_code.cend(),
                                     preprocess_directive_regex);
    auto end = std::sregex_iterator();

    for(auto i = begin;  i != end; ++i) {
        std::smatch match = *i;
        const std::string& keyword = match[1];
        if(match.size() == 3 && keyword == "__include") {
            std::string include_filename = match[2];

            if(*(include_filename.begin()) == '"' &&
               *(include_filename.end()-1) == '"')
            {
                include_filename.erase(0,1);
                include_filename.erase(include_filename.size()-1,1);

                bool include_file_opening_status = false;
                std::string include_file_contents;
                std::ifstream include_file;
                for(auto include_dir : include_directories) {
                    fs::path include_filepath(include_dir + include_filename);

                    std::error_code ec;
                    if(fs::exists(include_filepath, ec) && !ec &&
                       fs::is_regular_file(include_filepath, ec) && !ec)
                    {

                        auto include_file_size = fs::file_size(include_filepath);

                        include_file.open(include_filepath);
                        if(include_file.is_open() && include_file.good() ) {
                            include_file_contents
                                .reserve(include_file_size);

                            include_file_contents
                                .assign((std::istreambuf_iterator<char>(include_file)),
                                        std::istreambuf_iterator<char>());
                            include_file_opening_status = true;
                            break;
                        } //|
                    } //    |
                } //        |
                // <--------*

                if(include_file_opening_status) {
                    edit_map.insert(make_pair(match.position(0),
                                              make_pair(match[0],
                                                        include_file_contents)));
                } else{
                    ERROR("while parsing");
                    __CON(shader_path, ":", line_no_at(shader_code, match.position(0)));
                    __CON(match[0]);
                    __CON();
                    __CON("file ", match[2], " not found" );
                }
            } else {
                ERROR("while parsing");
                __CON(shader_path, ":", line_no_at(shader_code, match.position(0)));
                __CON(match[0]);
                __CON();
                __CON("syntax error");
            }
        }
        // else if
        // else if
        // else if
        // ...
        else {
            ERROR("while parsing");
            __CON(shader_path, ":", line_no_at(shader_code, match.position(0)));
            __CON(match[0]);
            __CON();
            __CON("unrecognised keyword ", keyword);
        }
    }

    size_t deleted_chars = 0;
    size_t inserted_chars = 0;

    for(auto const& edit : edit_map) {
        auto const& deletion = edit.second.first; 
        auto const& insertion = edit.second.second; 

        deleted_chars  += deletion.size();
        inserted_chars += insertion.size();
    }

    if(inserted_chars > deleted_chars) shader_code.reserve(shader_code.size() +
                                                            inserted_chars -
                                                            deleted_chars);
    deleted_chars = 0;
    inserted_chars = 0;

    for(auto const& edit : edit_map) {
        auto pos = edit.first +
            inserted_chars -
            deleted_chars;
        auto const& deletion = edit.second.first; 
        auto const& insertion = edit.second.second; 

        shader_code.erase(pos, deletion.size());
        shader_code.insert(pos, insertion);

        deleted_chars  += deletion.size();
        inserted_chars += insertion.size();
    }

    // std::cout << shader_code << "\n";

    return 0;
}//main



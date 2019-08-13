#include "ShaderBuilder.h"
#include <regex>
#include <tuple>
#include <fstream>
#include <filesystem>
#include <map>

#include "Logger.h"


namespace fs = std::filesystem;

static std::error_code make_error_code(ShaderBuilderErrc e) {
    return {static_cast<int>(e), theShaderBuilderErrCategory};
}

std::string ShaderBuilder::read_file(std::string filename, std::error_code& ec){

    bool file_opening_status = false;

    std::string file_contents;
    std::ifstream file;

    for(auto include_dir : include_dirs) {
        fs::path filepath(include_dir + filename);

        std::error_code ec;
        if(fs::exists(filepath, ec) && !ec &&
            fs::is_regular_file(filepath, ec) && !ec)
        {
            auto file_size = fs::file_size(filepath);

            file.open(filepath);
            if(file.is_open() && file.good() ) {
                file_contents
                    .reserve(file_size);

                file_contents
                    .assign((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
                file_opening_status = true;
                break;
            } //|
        } //    |
    } //        |
    // <--------*

    if(!file_opening_status) {
        ec = ShaderBuilderErrc::file_read_error;
        ERROR("failed to open ", filename);
    }
    return file_contents;
}

void ShaderBuilder::add_module(std::string filename) {
    std::error_code ec;
    auto module = parse(filename, ec);
    if(!ec) modules.push_back(module);
}
std::optional<ShaderModule> ShaderBuilder::get_module(std::string module_name) {
    for(auto& m : modules) {
        if(m.name == module_name) {
            return std::optional<ShaderModule>(m);
        }
    }
    return std::optional<ShaderModule>();
}

void ShaderBuilder::add_include_dir(std::string dir){
    add_include_dir(dir, last_ec);
}

void ShaderBuilder::add_include_dir(std::string dir, std::error_code& ec){
    fs::path dirpath(dir);
    if(fs::exists(dirpath, ec) && !ec &&
       fs::is_directory(dirpath, ec) && !ec) {
        include_dirs.push_back(dir);
    }
}

std::string ShaderBuilder::build(std::string init_module_name, std::error_code& ec){
    return "ok";
}
ShaderType ShaderBuilder::detect_type(std::string filename){
    ShaderType detected_type = ShaderType::NONE;
    //NOTE: not perfect but certainly good enough
    auto match_ext = [](std::string str, std::string ext) -> bool {
                         [[maybe_unused]]std::smatch match;
                         auto regex = std::regex(".+\\." + ext ,
                                                 std::regex_constants::ECMAScript);
                         return std::regex_match(str, match, regex);
                     };

         if(match_ext(filename, "vert"))
        detected_type = ShaderType::VERT;
    else if(match_ext(filename, "frag"))
        detected_type = ShaderType::FRAG;
    else if(match_ext(filename, "geom"))
        detected_type = ShaderType::GEOM;
    else if(match_ext(filename, "comp"))
        detected_type = ShaderType::COMP;
    else if(match_ext(filename, "tess_ctrl"))
        detected_type = ShaderType::TESS_CTRL;
    else if(match_ext(filename, "tess_eval"))
        detected_type = ShaderType::TESS_EVAL;

    return detected_type;
}

ShaderModule ShaderBuilder::parse(const std::string filename) {
    return parse(filename, last_ec);
}
ShaderModule ShaderBuilder::parse(const std::string filename, std::error_code& ec) {

    std::string source = read_file(filename, ec);
    std::string output;
    ShaderModule module;
    module.type = detect_type(filename);

    if(ec) return module;

    auto line_no_at = [](const std::string& str, size_t pos) -> size_t {
                            size_t line_no = 1;
                            for(size_t i=0; i<pos; ++i)
                                if(str[i] == '\n') line_no++;
                            return line_no;
                        };

    auto parse_err = [&](size_t pos, std::string source_fragment, std::string msg){
                            ERROR("while parsing");
                            __CON(filename, ":", line_no_at(source, pos));
                            __CON(source_fragment);
                            __CON();
                            __CON(msg);
                        };



    std::regex preprocess_directive_regex("((?:__)[a-z_]+)(?:\\s+)(.*)",
                                        std::regex_constants::ECMAScript
                                        // std::regex_constants::multiline
                                        );

    //       line no        delete       include
    std::map<int, std::pair<std::string, std::string>> edit_map;

    auto edit_at = [&](size_t pos,
                        const std::string& deletion,
                        const std::string& insertion)
                    {
                        edit_map.insert(make_pair(pos,
                                                    make_pair(deletion, insertion))); 
                    };

    auto begin = std::sregex_iterator(source.cbegin(),
                                        source.cend(),
                                        preprocess_directive_regex);
    auto end = std::sregex_iterator();

    size_t module_keyword_hits = 0; //NOTE: there ought to be only one module per file (for now)
    size_t type_keyword_hits = 0; //NOTE: there ought to be only one type per file (for now)

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

                std::error_code ec;
                std::string include_file_contents = read_file(include_filename, ec);

                if(!ec) {
                    edit_at(match.position(0), match[0], include_file_contents);
                } else{
                    parse_err(match.position(0), match[0],
                                std::string("file " + match[2].str() + " not found") );
                }
            } else {
                parse_err(match.position(0), match[0],
                            "syntax error");
            }
        } else if(match.size() == 3 && keyword == "__module") {
            std::string module_name = match[2];

            module_keyword_hits++;
            if(module_keyword_hits > 1) 
                parse_err(match.position(0), match[0],
                            "more than one module definition");


            if(*(module_name.begin()) == '"' &&
                *(module_name.end()-1) == '"')
            {
                module_name.erase(0,1);
                module_name.erase(module_name.size()-1,1);

                module.name = module_name;

                edit_at(match.position(0), match[0], "");
            }
        } else if(match.size() == 3 && keyword == "__uses") {
            std::string used_module_name = match[2];

            if(*(used_module_name.begin()) == '"' &&
               *(used_module_name.end()-1) == '"')
            {
                used_module_name.erase(0,1);
                used_module_name.erase(used_module_name.size()-1,1);

                module.used_modules.push_back(used_module_name);

                edit_at(match.position(0), match[0], "");
            }
        } else if(match.size() == 3 && keyword == "__type") {
            std::string module_type = match[2];

            type_keyword_hits++;
            if(module_keyword_hits > 1) 
                parse_err(match.position(0), match[0],
                            "more than one type definition");

            if(*(module_type.begin()) == '"' &&
               *(module_type.end()-1) == '"')
            {

                module_type.erase(0,1);
                module_type.erase(module_type.size()-1,1);

                     if(module_type == "NONE") module.type = ShaderType::NONE;
                else if(module_type == "FRAG") module.type = ShaderType::FRAG;
                else if(module_type == "VERT") module.type = ShaderType::VERT;
                else if(module_type == "GEOM") module.type = ShaderType::GEOM;
                else if(module_type == "COMP") module.type = ShaderType::COMP;
                else if(module_type == "TESS_CTRL") module.type = ShaderType::TESS_CTRL;
                else if(module_type == "TESS_EVAL") module.type = ShaderType::TESS_EVAL;

                edit_at(match.position(0), match[0], "");
            }
        }

        // else if
        // else if
        // else if
        // ...
        else {
            parse_err(match.position(0), match[0],
                        std::string("unrecognised keyword " + keyword));
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

    if(inserted_chars > deleted_chars) output.reserve(source.size() +
                                                        inserted_chars -
                                                        deleted_chars);
    output.assign(source);

    deleted_chars = 0;
    inserted_chars = 0;

    for(auto const& edit : edit_map) {
        auto pos = edit.first +
            inserted_chars -
            deleted_chars;
        auto const& deletion = edit.second.first; 
        auto const& insertion = edit.second.second; 

        output.erase(pos, deletion.size());
        output.insert(pos, insertion);

        deleted_chars  += deletion.size();
        inserted_chars += insertion.size();
    }
    module.source = output;
    return module;
}


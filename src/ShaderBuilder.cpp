#include "ShaderBuilder.h"
#include <regex>
#include <tuple>
#include <fstream>
#include <map>

#include "Logger.h"


namespace fs = std::filesystem;

static std::error_code make_error_code(ShaderBuilderErrc e) {
    return {static_cast<int>(e), theShaderBuilderErrCategory};
}

/// Removes all modules in builder
void ShaderBuilder::clear_modules() {
    modules.clear();
}


/// Checks if builder has a given module
///
/// @param module_name Name of module to look for
/// @return True if module is present, false otherways
bool ShaderBuilder::has_module(std::string module_name) {
    bool ret = false;
    for(auto& m : modules)
        if(m.name == module_name) ret = true;
    return ret;
}

/// Sets a shader header included at the top of every built file
/// @param header header contents
void ShaderBuilder::set_header(const std::string header) {
    this->header = header;
}

/// Adds module to the builder
/// @param filename Name of the file with module
void ShaderBuilder::add_module(std::string filename) {
    add_module(filename, last_ec);
}

/// Adds module to the builder
/// @param filename Name of the file with module
/// @param ec Error code
void ShaderBuilder::add_module(std::string filename, std::error_code& ec) {
    auto module = parse(filename, ec);
#ifdef SHADER_BUILDER_HOT_REBUILD_SUPPORT
    module.path = get_file_path(filename, ec);
    module.last_write_time = fs::last_write_time(module.path);
#endif
    if(!ec && !has_module(module.name)) modules.push_back(module);
}


/// Adds one or more modules to the builder
/// @param modules Brace-enclosed initializer list of names of files with modules to add
void ShaderBuilder::add_modules(std::initializer_list<std::string> modules){
    add_modules(modules, last_ec);
}


/// Adds one or more modules to the builder
/// @param modules Brace-enclosed initializer list of names of files with modules to add
/// @param ec Error code
/// @todo Should it perhaps break on return on first error?
/// @todo Should there be bool for it like add_modules_fail_fast ?
void ShaderBuilder::add_modules(std::initializer_list<std::string> modules,
                                std::error_code& ec){
    for(auto& module : modules) add_module(module, ec); 
}


/// Imports modules listed in file
/// @param modules_list_filename Name of the file with list of modules to import
void ShaderBuilder::import_modules_from_file(std::string modules_list_filename) {
    import_modules_from_file(modules_list_filename, last_ec);
}

/// Imports modules listed in file
/// @param modules_list_filename Name of the file with list of modules to import
/// @param ec Error code
void ShaderBuilder::import_modules_from_file(std::string modules_list_filename,
                                          std::error_code& ec) {
    std::stringstream ss(read_file(modules_list_filename, ec));

    for(std::string line; std::getline(ss,line);) {
        add_module(line, ec);
    }
}


/// Returns given module by value
/// @param module_name
/// @return module specified by module_name or nothing if no such module is present
std::optional<ShaderModule> ShaderBuilder::get_module(std::string module_name) {
    for(auto& m : modules) {
        if(m.name == module_name) {
            return std::optional<ShaderModule>(m);
        }
    }
    return std::optional<ShaderModule>();
}

/// Returns given module by referece
/// @param module_name
/// @return module specified by module_name or nothing if no such module is present
std::optional<std::reference_wrapper<ShaderModule>>
ShaderBuilder::get_module_mut(std::string module_name){
    for(auto& m : modules) {
        if(m.name == module_name) {
            return std::optional<std::reference_wrapper<ShaderModule>>(m);
        }
    }
    return std::optional<std::reference_wrapper<ShaderModule>>();
}



/// Returns list of all builder modules
/// @return vector of modules
const std::vector<ShaderModule>& ShaderBuilder::get_modules_list() {
    return modules;
}

/// Returns list of topologicaly sorted modules
///
/// Returns reference to vector which is buffer for topological sort function.
/// After build this vector of modules in the same order as they went to the output file.
/// Should not be used for anything but debugging.
/// @return vector of sorted modules
const std::vector<ShaderModule>& ShaderBuilder::get_sorted_modules_list() {
    return sorted_modules;
}

/// Appends directory to search-for-modules directories
/// @param dir new include path
/// @todo take std::filesystem::path instead of string
void ShaderBuilder::add_include_dir(std::string dir){
    add_include_dir(dir, last_ec);
}

/// Appends directory to search-for-modules directories
/// @param dir new include path
/// @todo take std::filesystem::path instead of string
/// @param ec Error code
void ShaderBuilder::add_include_dir(std::string dir, std::error_code& ec){
    fs::path dirpath(dir);
    if(fs::exists(dirpath, ec) && !ec &&
       fs::is_directory(dirpath, ec) && !ec) {
        include_dirs.push_back(dir);
    }
}



std::string ShaderBuilder::build(std::string init_module_name){
    return build(init_module_name, last_ec);
}

std::string ShaderBuilder::build(std::string init_module_name, std::error_code& ec){
    std::string ret = "";
    auto opt_init_module = get_module_mut(init_module_name);
    if(opt_init_module.has_value())
        topo_sort_modules(opt_init_module.value(),ec);
    else {
        ERROR("init build module ", init_module_name, " not found");
        return ret;
    }

    size_t final_size = header.size();
    for(auto m : modules) final_size += m.source.size();

    ret.reserve(final_size);

    ret += header;
    for(auto m : sorted_modules) ret += m.source;

    return ret;
}



//TODO: Optimize that!
/// Performs module topological sort so modules are included in correct order
/// Fails if finds circular dependency or missing dependency
///
/// @param ec error_code - unused
/// @return true unless it fails
///
bool ShaderBuilder::topo_sort_modules(ShaderModule& root_module,
                                      [[maybe_unused]]std::error_code& ec) {
   for(auto& m : modules){
        m.temp_mark = false;
        m.perm_mark = false;
    }

    sorted_modules.clear();
    sorted_modules.reserve(modules.size());

    if(! topo_sort_recursive_visit(root_module) ){
        ERROR("failed to resolve dependencies");
        return false;
    }
    return true;
}

bool ShaderBuilder::topo_sort_recursive_visit(ShaderModule& module){
    if(module.perm_mark == true) return true;
    if(module.temp_mark == true) return false;

    module.temp_mark = true;

    for(auto& dep_name : module.used_modules) {
        auto opt_dep = get_module_mut(dep_name); // NOTE: opt_ prefix marks optional
        if(opt_dep.has_value()) {
            bool is_acyclic = topo_sort_recursive_visit(opt_dep.value());
            if(!is_acyclic) {
                ERROR("circular dependency ", dep_name);
                __CON("used by ", module.name);
                return false;
            };
        } else {
            ERROR("missing module ", dep_name);
            __CON("used by ", module.name);
            return false;
        }
    }

    module.temp_mark = false;
    module.perm_mark = true;

    sorted_modules.push_back(module);
    return true;
}



/// Detects shader type depending on filename extension
///
/// For filename with a given extension it will return
/// "*.vert" - VERT
/// "*.frag" - FRAG
/// "*.geom" - GEOM
/// "*.comp" - COMP
/// "*.tess_ctrl" - TESS_CTRL
/// "*.tess_eval" - TESS_EVAL
///
/// @param filename name of the file
/// @return return detected type or type NONE
///
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



//TODO: multipass
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
                            __CON(msg);
                        };



    std::regex preprocess_directive_regex("(?:\\n)?((?:__)[a-z_]+)(?:\\s+)(\".*?\")(?:\\s+)?",
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
            if(type_keyword_hits > 1) 
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

    if(module.name.empty()){
        ERROR("unnamed module in ", filename);
    }


    size_t deleted_chars = 0;
    size_t inserted_chars = 0;

    for(auto const& edit : edit_map) {
        auto const& deletion = edit.second.first; 
        auto const& insertion = edit.second.second; 

        deleted_chars  += deletion.size();
        inserted_chars += insertion.size();
    }

    std::string header =
        "\n//=============================== " + module.name + "\n";

    if(inserted_chars > deleted_chars) output.reserve(header.size() +
                                                      source.size() +
                                                      inserted_chars -
                                                      deleted_chars);
    output.assign(header + source);

    deleted_chars = 0;
    inserted_chars = 0;

    for(auto const& edit : edit_map) {
        auto pos =
            edit.first +
            header.size() +
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

#ifdef SHADER_BUILDER_HOT_REBUILD_SUPPORT
fs::path ShaderBuilder::get_file_path(std::string filename, std::error_code& ec){

    bool filepath_status = false;

    fs::path filepath;

    for(auto include_dir : include_dirs) {
        filepath = fs::path(include_dir + filename);

        if(fs::exists(filepath, ec) && !ec &&
           fs::is_regular_file(filepath, ec) && !ec){
            filepath_status = true; 
            break;
        }
    }
    if(!filepath_status) {
        //TODO: hadnle error
    }

    return filepath;
}

bool ShaderBuilder::check_for_dirty_modules() {
    bool any_dirty = false;
    for(auto& module : modules){
        auto newtime = fs::last_write_time(module.path);
        if(module.last_write_time != newtime){
            module.last_write_time = newtime;
            module.dirty = true;
            any_dirty = true;
        }
    }
    return any_dirty;
}
//NOTE: Would rebuild even if edited module is not used by root module
//      or any of its dependencies
///
bool ShaderBuilder::hot_rebuild(const std::string root_module, std::string& output) {
    bool needs_rebuild = false;
    if(check_for_dirty_modules()){
        for(auto& module : modules){
            if(module.dirty) {
                needs_rebuild = true;
            }
        }
        if(needs_rebuild) {
            output = build(root_module);
            return true;
        }
    }
    return false;
}
#endif


#pragma once
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <system_error>
#include <filesystem>

#define SHADER_BUILDER_HOT_REBUILD_SUPPORT
enum class ShaderType {NONE, VERT, FRAG, GEOM, COMP, TESS_CTRL, TESS_EVAL};

struct ShaderModule { //TODO: you could hash module names
    std::string name;
    std::vector<std::string> used_modules;
    std::string source;
    ShaderType type = ShaderType::NONE;

    //NOTE: used for hot reload
#ifdef SHADER_BUILDER_HOT_REBUILD_SUPPORT
    std::filesystem::path path;
    std::filesystem::file_time_type last_write_time;
    bool dirty = false;
#endif

    //NOTE: used for topological sorting
    bool temp_mark;
    bool perm_mark;
};

enum class ShaderBuilderErrc {
                              file_read_error
};

namespace std {
    template <>
    struct is_error_code_enum<ShaderBuilderErrc> : true_type {};
}
namespace {
    struct ShaderBuilderErrCategory : std::error_category {
        const char* name() const noexcept override;
        std::string message(int ev) const override;
    };

    const char* ShaderBuilderErrCategory::name() const noexcept {
    return "flights";
    }

    std::string ShaderBuilderErrCategory::message(int ev) const {
    switch (static_cast<ShaderBuilderErrc>(ev)) {
        case ShaderBuilderErrc::file_read_error:
            return "failed to read file";
        default:
            return "(unrecognized error)";
        }
    }

    const ShaderBuilderErrCategory theShaderBuilderErrCategory {};
}
/// Stores and assembles shader modules
///
/// Rather hefty class handling parsing and storing modules and their interdependencies.
/// Meant to have only one instance, that builds one or many shaders sharing dependencies.
class ShaderBuilder {
public:
    void clear_modules();

    bool has_module(std::string module_name);

    void set_header(std::string header);

    void add_module(std::string filename);
    void add_module(std::string filename, std::error_code& ec);

    void add_modules(std::initializer_list<std::string> modules);
    void add_modules(std::initializer_list<std::string> modules, std::error_code& ec);

    void import_modules_from_file(std::string modules_list_filename);
    void import_modules_from_file(std::string modules_list_filename, std::error_code& ec);

    std::optional<ShaderModule> get_module(std::string module_name);
    std::optional<std::reference_wrapper<ShaderModule>>
      get_module_mut(std::string module_name);

    const std::vector<ShaderModule>& get_modules_list();
    const std::vector<ShaderModule>& get_sorted_modules_list();

    void add_include_dir(std::string dir);
    void add_include_dir(std::string dir, std::error_code& ec);

    std::string build(std::string init_module_name);
    std::string build(std::string init_module_name, std::error_code& ec);


#ifdef SHADER_BUILDER_HOT_REBUILD_SUPPORT
    bool check_for_dirty_modules();

    bool hot_rebuild(const std::string root_module, std::string& output);

#endif

private:

    bool topo_sort_modules(ShaderModule& root_module,std::error_code& ec);
    bool topo_sort_recursive_visit(ShaderModule& module);

    ShaderType detect_type(std::string filename);

    ShaderModule parse(const std::string filename);
    ShaderModule parse(const std::string filename, std::error_code& ec);

    std::string read_file(std::string filename, std::error_code& ec);

#ifdef SHADER_BUILDER_HOT_REBUILD_SUPPORT
    std::filesystem::path get_file_path(std::string filename, std::error_code& ec);

#endif

    std::string header;
    std::vector<ShaderModule> modules;
    std::vector<ShaderModule> sorted_modules;
    std::vector<std::string> include_dirs; //change type to fs::path?
    std::error_code last_ec;
};


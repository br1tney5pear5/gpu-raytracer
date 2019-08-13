#pragma once
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <system_error>


enum class ShaderType {NONE, VERT, FRAG, GEOM, COMP, TESS_CTRL, TESS_EVAL};

struct ShaderModule { //TODO: you could hash module names
    std::string name;
    std::vector<std::string> used_modules;
    std::string source;
    ShaderType type = ShaderType::NONE;

    //NOTE: used for topological sorting
    bool temp_mark;
    bool perm_mark;

    friend std::ostream& operator<<(std::ostream& os, const ShaderModule& module) {
        os << "name: " << module.name << '\n';
        os << "type: " ;
        switch(module.type) {
            case ShaderType::NONE: os << "NONE"; break;
            case ShaderType::VERT: os << "VERT"; break;
            case ShaderType::FRAG: os << "FRAG"; break;
            case ShaderType::GEOM: os << "GEOM"; break;
            case ShaderType::COMP: os << "COMP"; break;
            case ShaderType::TESS_CTRL: os << "TESS_CTRL"; break;
            case ShaderType::TESS_EVAL: os << "TESS_EVAL"; break;
            default: os << "NONE"; break;
        } os << '\n';
        os << "uses: ";
        for(auto& s : module.used_modules) std:: cout << s << " ";
        os <<'\n';
        return os;
    }

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

class ShaderBuilder {
public:
    std::string read_file(std::string filename, std::error_code& ec);
    void add_module(std::string filename);
    std::optional<ShaderModule> get_module(std::string module_name);
    void add_include_dir(std::string dir);
    void add_include_dir(std::string dir, std::error_code& ec);

    std::string build(std::string init_module_name, std::error_code& ec);
private:
    ShaderType detect_type(std::string filename);
    ShaderModule parse(const std::string filename);
    ShaderModule parse(const std::string filename, std::error_code& ec);

    std::vector<ShaderModule> modules;
    std::vector<std::string> include_dirs; //change type to fs::path?
    std::error_code last_ec;
};


#pragma once
#include <string>
#include <vector>
#include <system_error>


enum class ShaderType {NONE, VERT, FRAG, GEOM, COMP, TESS_CTRL, TESS_EVAL};

struct ShaderModule { //TODO: you could hash module names
    std::string name;
    std::vector<std::string> used_modules;
    std::string source;
    ShaderType type = ShaderType::NONE;
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
    ShaderModule parse(const std::string filename);
    ShaderModule parse(const std::string filename, std::error_code& ec);
    void add_include_dir(std::string dir);
    void add_include_dir(std::string dir, std::error_code& ec);
private:
    std::vector<ShaderModule> modules;
    std::vector<std::string> include_dirs; //change type to fs::path?
    std::error_code last_ec;
};


#include <array>
#include <glad/glad.h>

#include <algorithm>
#include <array>
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>

#include "runtime_exception.h"
#include "shader.h"

Shader::Shader() : program(-1) {}

Shader::Shader(unsigned int program) : program(program) {}

Shader::Shader(const Shader &shader) : program(shader.program) {}

int Shader::uniform(const char *name) {
    auto uniform = glGetUniformLocation(program, name);
    if (uniform == -1) {
        throw RuntimeException::error("Failed to find uniform", name);
    }
    return uniform;
}

Shader Shader::load(std::vector<std::string> files,
                    std::function<std::vector<unsigned char>(const std::string &)> loader) {

    auto shader_types = {ShaderBuilder::Vertex, ShaderBuilder::Fragment, ShaderBuilder::Geometry};
    ShaderBuilder builder;

    auto try_compile = [&loader, &builder](auto source) {
        if (source) {
            ShaderBuilder::ShaderType type = source.value();
            if (std::vector<unsigned char> source = loader(ShaderBuilder::filename(type));
                !source.empty()) {
                builder.compile(type, source);
            }
        }
    };

    auto get_shader_type =
        [shader_types](std::string filename) -> std::optional<ShaderBuilder::ShaderType> {
        for (auto type : shader_types) {
            if (ShaderBuilder::filename(type) == filename) {
                return std::optional(type);
            }
        }
        return std::nullopt;
    };

    auto shader_files = files | std::views::transform(get_shader_type);

    std::ranges::for_each(shader_files, try_compile);
    auto program = builder.build();
    return Shader(program);
}

Shader Shader::default_resource() { throw RuntimeException::error("No default shader"); }

void Shader::load(int uniform, float val) { glUniform1f(uniform, val); }

void Shader::load(int uniform, unsigned int val) { glUniform1i(uniform, val); }

void Shader::load(int uniform, glm::mat4 mat) {
    glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::load(int uniform, glm::vec3 vec) { glUniform3f(uniform, vec.x, vec.y, vec.z); }

Shader::~Shader() {}

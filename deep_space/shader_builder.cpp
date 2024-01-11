
#include "shader_builder.h"
#include "runtime_exception.h"
#include "shader.h"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <glad/glad.h>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

std::string check_result(std::function<void(int *)>, std::function<void(char *, unsigned int)>);
const char *read_file(const char *filename);

ShaderBuilder::ShaderBuilder() { program = glCreateProgram(); }

const char *ShaderBuilder::filename(ShaderType type) {
    switch (type) {
    case Vertex:
        return "vertex.glsl";
    case Fragment:
        return "fragment.glsl";
    case Geometry:
        return "geometry.glsl";
    }
}

int ShaderBuilder::to_gl(ShaderType type) {
    switch (type) {
    case Vertex:
        return GL_VERTEX_SHADER;
    case Fragment:
        return GL_FRAGMENT_SHADER;
    case Geometry:
        return GL_GEOMETRY_SHADER;
    }
}

ShaderBuilder ShaderBuilder::compile(ShaderType type, const std::vector<unsigned char> &source) {
    const char *src = (char *)source.data();

    auto shader = glCreateShader(to_gl(type));
    if (shader == 0) {
        throw ShaderException("failed to create shader");
    }

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    auto is_success = [shader](int *success) { glGetShaderiv(shader, GL_COMPILE_STATUS, success); };
    auto get_log = [shader](char *log, unsigned int log_size) {
        glGetShaderInfoLog(shader, log_size, nullptr, log);
    };

    auto result = check_result(is_success, get_log);

    if (result.empty()) {
        glAttachShader(program, shader);
        return *this;
    } else {
        throw RuntimeException::error("Failed to compile shader", filename, "reason:\n", result);
    }
}

unsigned int ShaderBuilder::build() {
    glLinkProgram(program);

    auto is_success = [this](int *success) { glGetProgramiv(program, GL_LINK_STATUS, success); };
    auto get_log = [this](char *log, unsigned int log_size) {
        glGetProgramInfoLog(program, log_size, nullptr, log);
    };

    auto result = check_result(is_success, get_log);

    if (result.empty()) {
        return program;
    } else {
        throw RuntimeException::error("Failed to link shader", result);
    }
}

std::string check_result(std::function<void(int *)> is_success,
                         std::function<void(char *, unsigned int)> get_logs) {
    int success;
    char log[512];
    is_success(&success);
    if (!success) {
        get_logs(log, 512);
        return std::string(log);
    }
    return "";
}

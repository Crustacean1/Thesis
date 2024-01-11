#include "program.h"
#include "../opengl/graphical_device_gl.h"

#include <algorithm>
#include <filesystem>
#include <ranges>

namespace fs = std::filesystem;
namespace vs = std::ranges::views;

std::tuple<const char *, const char *, const char *> read_shader(const char *name);

Program::Program(GL::GraphicalDevice &device, const char *name)
    : m_device(device), sources(read_shader(name)),
      handle(device.create(ProgramDefinition{name, std::get<0>(sources), std::get<1>(sources),
                                             std::get<2>(sources)})) {}

Program::~Program() {}

std::pair<unsigned int, char *> read_file(const char *filename) {
    auto file = fopen(filename, "r");
    if (file == nullptr) {
        char error[512];
        sprintf(error, "File '%s' not found", filename);
        throw std::runtime_error(error);
    }

    fseek(file, 0, SEEK_END);
    auto file_length = ftell(file);
    rewind(file);

    char *buffer = new char[file_length + 1];
    fread(buffer, 1, file_length, file);
    buffer[file_length] = 0;

    return std::pair(file_length + 1, buffer);
}

std::tuple<const char *, const char *, const char *> read_shader(const char *name) {
    auto root = fs::current_path() / "shaders" / name;
    auto shader_types = {"vertex.glsl", "geometry.glsl", "fragment.glsl"};
    auto try_read_file = [root](fs::path file) -> char * {
        auto path = root / file;
        if (fs::exists(path)) {
            return std::get<1>(read_file(path.c_str()));
        } else {
            return nullptr;
        }
    };

    auto shader_contents = shader_types | vs::transform(try_read_file);
    if (std::find_if(shader_contents.begin(), shader_contents.end(),
                     [](const char *c) { return c != nullptr; }) == shader_contents.end()) {
        char error_msg[512];
        sprintf(error_msg, "Shader program '%s' not found", name);
        throw std::runtime_error(error_msg);
    }
    auto it = shader_contents.begin();

    auto v = *it++;
    auto f = *it++;
    auto g = *it++;
    return std::tuple(v, f, g);
}

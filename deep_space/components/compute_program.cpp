#include "./compute_program.h"
#include "../opengl//graphical_device_gl.h"
#include "./program.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

const char *read_compute_shader(const char *name);

ComputeProgram::ComputeProgram(GL::GraphicalDevice &device, const char *name)
    : m_device(device),
      handle(device.create(ComputeProgramDefinition{name, read_compute_shader(name)})) {}

const char *read_compute_shader(const char *name) {
    char buffer[128];

    auto root = fs::current_path() / "shaders" / name / "compute.glsl";

    return read_file(root.c_str()).second;
}

void ComputeProgram::dispatch(int x, int y, int z) { m_device.dispatch(handle, x, y, z); }

#include "shader_buffer.h"
#include "../opengl/graphical_device_gl.h"

#include <stdexcept>

ShaderBuffer::ShaderBuffer(GL::GraphicalDevice &device, UsagePattern pattern, unsigned int size, void * data)
    : m_device(device), handle(device.create(ShaderBufferDefinition{data, size, pattern})) {}

void ShaderBuffer::load(void *ptr, unsigned int size) { m_device.load(handle, ptr, size); }

void ShaderBuffer::map(void **ptr) { m_device.map_read(handle, ptr); }

void ShaderBuffer::unmap(){m_device.unmap(handle);}

void ShaderBuffer::bind(unsigned int location) { m_device.bind(handle, location); }

ShaderBuffer::~ShaderBuffer() {}

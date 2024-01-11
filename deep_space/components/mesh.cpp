#include "./mesh.h"
#include "../opengl/graphical_device_gl.h"

#include <numeric>
#include <ranges>

Handles::VertexBuffer create_vertex_buffer(GL::GraphicalDevice &device, const void *data,
                                           size_t size) {
    return device.create(VertexBufferDefinition{data, size, UsagePattern::Static});
}

Handles::IndexBuffer create_index_buffer(GL::GraphicalDevice &device, const void *data,
                                         size_t size) {
    return device.create(IndexBufferDefinition{data, size, UsagePattern::Static});
}

Handles::Mesh create_mesh(GL::GraphicalDevice &device, const Handles::VertexBuffer &vertex_buffer,
                          const Handles::IndexBuffer &index_buffer, const VertexLayout &layout) {
    return device.create(MeshDefinition{layout, vertex_buffer, index_buffer});
}

void load(GL::GraphicalDevice &device, Handles::VertexBuffer buffer,const  void *data,
          unsigned int size) {
    device.load(buffer, data, size);
}

void load(GL::GraphicalDevice &device, Handles::IndexBuffer buffer,const  void *data, unsigned int size) {
    device.load(buffer, data, size);
}

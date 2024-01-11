#ifndef SHADER_BUFFER
#define SHADER_BUFFER

#include "../graphical_device.h"

namespace GL {
class GraphicalDevice;
}

class ShaderBuffer {
    GL::GraphicalDevice &m_device;

  public:
    const Handles::ShaderBuffer handle;
    ShaderBuffer(GL::GraphicalDevice &device, UsagePattern pattern, unsigned int size, void *data = nullptr);

    ShaderBuffer(const ShaderBuffer &) = delete;
    ShaderBuffer &operator=(const ShaderBuffer &) = delete;

    void load(void *ptr, unsigned int size);
    void map(void **ptr);
    void unmap();

    void bind(unsigned int location);

    ~ShaderBuffer();
};

#endif /*SHADER_BUFFER*/

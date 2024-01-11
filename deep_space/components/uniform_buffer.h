#ifndef UNIFORM_BUFFER
#define UNIFORM_BUFFER

#include "../graphical_context.h"
#include "../opengl/graphical_device_gl.h"

template <typename T> class UniformBuffer {
  public:
    T data;

    UniformBuffer(GL::GraphicalDevice &context)
        : context(context), handle(context.create(UniformBufferDefinition{})) {}
    UniformBuffer(GL::GraphicalDevice &context, T t)
        : data(t), context(context), handle(context.create(UniformBufferDefinition{})) {}

    UniformBuffer(const UniformBuffer &) = delete;
    UniformBuffer &operator=(const UniformBuffer &) = delete;

    void bind(unsigned int position) { context.bind(handle, position); }

    void load() { context.load(handle, &data, sizeof(T)); }

    ~UniformBuffer() {}

  private:
    Handles::UniformBuffer handle;
    GL::GraphicalDevice &context;
};

#endif /*UNIFORM_BUFFER*/

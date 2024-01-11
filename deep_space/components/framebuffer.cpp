#include <glad/glad.h>

#include <iostream>
#include <vector>

#include "../opengl/graphical_device_gl.h"
#include "../runtime_exception.h"
#include "./framebuffer.h"

std::shared_ptr<Framebuffer> Framebuffer::m_default_fb = nullptr;

Framebuffer::Framebuffer(GL::GraphicalDevice &context, bool default_fb)
    : context(context), framebuffer_handle(default_fb ? context.create(FramebufferDefinition{})
                                                      : context.default_fb()) {}

void Framebuffer::bind(Handles::Texture2D texture) {
    context.attach(framebuffer_handle, AttachmentDefinition2D{texture, BufferType::Color});
}

void Framebuffer::bind(Handles::Texture2DFloat texture) {
    context.attach(framebuffer_handle, AttachmentDefinition2DFloat{texture, BufferType::Color});
}

void Framebuffer::bind(Handles::CubemapTexture texture) {
    context.attach(framebuffer_handle, AttachmentDefinitionCube{texture, BufferType::Color});
}

std::shared_ptr<Framebuffer> Framebuffer::default_fb(GL::GraphicalDevice &device) {
    if (m_default_fb == nullptr) {
        m_default_fb = std::make_shared<Framebuffer>(device, false);
    }
    return m_default_fb;
}

Framebuffer::~Framebuffer() {}

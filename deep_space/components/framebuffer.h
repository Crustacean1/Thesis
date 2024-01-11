#ifndef FRAMEBUFFER
#define FRAMEBUFFER

#include "../graphical_device.h"
#include "./texture.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace GL {
class GraphicalDevice;
}

class Framebuffer {
    static std::shared_ptr<Framebuffer> m_default_fb;

    GL::GraphicalDevice &context;

  public:
    const Handles::Framebuffer framebuffer_handle;
    static std::shared_ptr<Framebuffer> default_fb(GL::GraphicalDevice &);

    Framebuffer(GL::GraphicalDevice &context, bool default_fb = true);

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;

    void bind(Handles::Texture2D texture);
    void bind(Handles::Texture2DFloat texture);
    void bind(Handles::CubemapTexture texture);
    void generate_depth_renderbuffer(unsigned int width, unsigned int height);

    ~Framebuffer();
};

#endif /*FRAMEBUFFER*/

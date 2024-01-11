#ifndef TEXTURE
#define TEXTURE

#include "../graphical_context.h"
#include <array>

namespace GL {
class GraphicalDevice;
}

class Texture2D {
    GL::GraphicalDevice &m_device;
    unsigned int m_width;
    unsigned int m_height;

  public:
    typedef const unsigned char *TextureSource;
    Texture2D(GL::GraphicalDevice &context, unsigned int width, unsigned int height,
              TextureSource data = nullptr);

    const Handles::Texture2D handle;

    Texture2D(const Texture2D &) = delete;
    Texture2D &operator=(const Texture2D &) = delete;

    unsigned int width();
    unsigned int height();

    void bind(unsigned int slot = 0);

    void load(unsigned int width, unsigned int height, TextureSource data);
};

class Texture2DFloat {
    GL::GraphicalDevice &m_device;
    unsigned int m_width;
    unsigned int m_height;

  public:
    const Handles::Texture2DFloat handle;

    Texture2DFloat(GL::GraphicalDevice &context, unsigned int width, unsigned int height);

    Texture2DFloat(const Texture2DFloat &) = delete;
    Texture2DFloat &operator=(const Texture2DFloat &) = delete;
};

class CubemapTexture {
    GL::GraphicalDevice &m_device;
    unsigned int m_width;
    unsigned int m_height;

  public:
    Handles::CubemapTexture handle;

    typedef std::array<const unsigned char *, 6> TextureSource;

    CubemapTexture(GL::GraphicalDevice &context, unsigned int width, unsigned int height,
                   const TextureSource &source = {
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr,
                   });

    unsigned int width();
    unsigned int height();

    void bind();

    void load(unsigned int width, unsigned int height, TextureSource source);
};

class Texture3D {

    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_depth;
};

#endif /*TEXTURE*/

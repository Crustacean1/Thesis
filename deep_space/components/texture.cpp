#include "./texture.h"
#include "../opengl/graphical_device_gl.h"

Texture2D::Texture2D(GL::GraphicalDevice &context, unsigned int width, unsigned int height,
                     TextureSource data)
    : m_device(context),
      handle(context.create(TextureDefinition2D{
          .format = TextureFormat::Rgb, .data = (void *)data, .width = width, .height = height})) {}

Texture2DFloat::Texture2DFloat(GL::GraphicalDevice &context, unsigned int width,
                               unsigned int height)
    : m_device(context), handle(context.create(TextureDefinition2DFloat{
                             .width = width,
                             .height = height,
                             .data = (void *)nullptr,
                         })) {}

unsigned int Texture2D::width() { return m_width; }
unsigned int Texture2D::height() { return m_height; }

CubemapTexture::CubemapTexture(GL::GraphicalDevice &device, unsigned int width, unsigned int height,
                               const TextureSource &data)
    : m_device(device), handle(device.create(CubeTextureDefinition{.format = TextureFormat::Rgb,
                                                                   .data = (void *)data[0],
                                                                   .width = width,
                                                                   .height = height})) {}
void CubemapTexture::bind() {}

unsigned int CubemapTexture::width() { return m_width; }
unsigned int CubemapTexture::height() { return m_height; }

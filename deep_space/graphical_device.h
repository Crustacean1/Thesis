#ifndef GRAPHICAL_DEVICE
#define GRAPHICAL_DEVICE

#include <cstddef>
#include <vector>

enum RenderState {
    Blending = 3,
    DepthTest = 4,
    DepthWrite = 8,
    FrontCull = 16,
    BackCull = 32,
};

enum AlphaBlending { None = 0, Addative = 1, Dst = 2 };

enum TextureFormat { Grayscale, StencilDepth, Rgb, Rgba };
enum TextureType { Tex2D, Tex3D, Cubemap };

enum BufferType { Color, Depth, DepthStencil };

enum Geometry {
    Points,
    Lines,
    Triangles,
    Quads,
};

enum UsagePattern { Static, Dynamic, Read, Write };

template <typename T> struct BoundingRect {
    T left;
    T top;
    T width;
    T height;
};

namespace Handles {
struct Handle {
    size_t id = 0;
};

struct VertexBuffer : Handle {};
struct IndexBuffer : Handle {};
struct UniformBuffer : Handle {};
struct ShaderBuffer : Handle {};
struct Mesh : Handle {};
struct Texture2D : Handle {};
struct Texture2DFloat : Handle {};
struct Texture3D : Handle {};
struct CubemapTexture : Handle {};
struct Program : Handle {};
struct ComputeProgram : Handle {};
struct Framebuffer : Handle {};
} // namespace Handles

struct VertexLayout {
    struct Attribute {
        size_t location;
        size_t size;
        size_t type;
        size_t count;
    };

    const std::vector<Attribute> attributes;
    const size_t stride;
};

struct BufferDefinition {
    const void *data;
    size_t size;
    UsagePattern pattern;
};

struct VertexBufferDefinition : public BufferDefinition {};

struct IndexBufferDefinition : public BufferDefinition {};

struct UniformBufferDefinition : public BufferDefinition {};

struct ShaderBufferDefinition : public BufferDefinition {};

struct MeshDefinition {
    VertexLayout layout;
    Handles::VertexBuffer vertices;
    Handles::IndexBuffer indices;
};

struct InstancedMeshDefinition {
    VertexLayout layout;
    Handles::VertexBuffer vertices;
    Handles::VertexBuffer instances;
    Handles::IndexBuffer indices;
};

struct TextureDefinition2DFloat {
    unsigned int width;
    unsigned int height;
    void *data;
};

struct TextureDefinition2D {
    TextureFormat format;
    void *data;
    unsigned int width;
    unsigned int height;
};

struct TextureDefinition3D {
    TextureFormat format;
    void *data;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
};

struct CubeTextureDefinition {
    TextureFormat format;
    void *data;
    unsigned int width;
    unsigned int height;
};

struct FramebufferDefinition {};

struct ProgramDefinition {
    const char *name;
    const char *vertex;
    const char *geometry;
    const char *fragment;
};

struct ComputeProgramDefinition {
    const char *name;
    const char *source;
};

namespace Commands {

template <typename T, unsigned int N> struct Command {
    static constexpr unsigned int COMMAND_TYPE = N;
    const unsigned int type = N;
    size_t pass = 0;
};

struct TextureBinding {
    union {
        Handles::Texture2D tex2d;
        Handles::CubemapTexture cube;
    } texture;
    enum { Texture2D, Cubemap } binding_type;
};

struct UniformBinding {
    Handles::UniformBuffer buffer;
    const void *data;
    unsigned int size;
};

struct Draw : Command<Draw, 1> {
    unsigned int state;
    unsigned int point_size = 1;

    unsigned int textures = 0;
    TextureBinding texture_binding[4];
    unsigned int uniforms = 0;
    UniformBinding uniform_bindings[4];

    Geometry geometry = Geometry::Triangles;
    Handles::Framebuffer fb;
    Handles::Program program;
    Handles::Mesh vao;
    size_t offset;
    size_t size;
};

struct DrawInstanced : Command<DrawInstanced, 2> {
    unsigned int state;
    Geometry geometry;
    Handles::Framebuffer fb;
    Handles::Program program;
    Handles::Mesh vao;
    size_t offset;
    size_t size;
    size_t instances;
};

struct Clear : Command<Clear, 3> {
    Handles::Framebuffer fb;
};

struct SetViewport : Command<SetViewport, 4> {
    BoundingRect<float> rect;
};

} // namespace Commands

struct AttachmentDefinition2DFloat {
    Handles::Texture2DFloat texture;
    BufferType target;
};

struct AttachmentDefinition2D {
    Handles::Texture2D texture;
    BufferType target;
};

struct AttachmentDefinitionCube {
    Handles::CubemapTexture texture;
    BufferType target;
};

#endif /*GRAPHICAL_DEVICE*/

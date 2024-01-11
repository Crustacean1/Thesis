#ifndef MY_GL_RENDERER
#define MY_GL_RENDERER

#include "../graphical_context.h"
#include "../graphical_device.h"
#include "generic_container.h"

namespace GL {
class GraphicalDevice {
    unsigned int m_current_program;
    unsigned int m_current_vao;
    unsigned int m_current_fb;
    unsigned int m_current_point_size;

    std::pair<unsigned int, unsigned int> m_current_textures[16];
    unsigned int m_current_uniforms[16];

    unsigned int m_current_state;
    unsigned int m_custom_uniform_buffer;

    static constexpr size_t UNIFORM_BUFFER_COUNT = 4;
    unsigned int m_current_uniform_buffers[UNIFORM_BUFFER_COUNT];

    unsigned int geometry(Geometry);
    unsigned int pattern(UsagePattern);
    unsigned int attribute_type();
    unsigned int tex_format(TextureFormat);

    unsigned int create_buffer();

    struct Program {
        unsigned int id;
    };

    struct Mesh {
        unsigned int vao;
    };

    struct VertexBuffer {
        unsigned int id;
    };

    struct IndexBuffer {
        unsigned int id;
    };

    struct ShaderBuffer {
        unsigned int id;
    };

    struct Framebuffer {
        unsigned int id;
        unsigned int width;
        unsigned int height;
    };

    struct Texture2D {
        unsigned int texture;
        unsigned int width;
        unsigned int height;
    };

    struct CubemapTexture {
        unsigned int texture;
        unsigned int width;
        unsigned int height;
    };

    struct UniformBuffer {
        unsigned int buffer;
        unsigned int size;
    };

    GenericContainer<Program, Mesh, VertexBuffer, IndexBuffer, Framebuffer, Texture2D,
                     CubemapTexture, UniformBuffer, ShaderBuffer>
        container;

    static void error_callback(unsigned int source, unsigned int type, unsigned int id,
                               unsigned int severity, int length, const char *message,
                               const void *userParam);

    int compile(unsigned int target, const char *);

    bool get_compile_logs(unsigned int program, char *logs);
    bool get_linking_logs(unsigned int program, char *logs);

    unsigned int create_program(const char *name,
                                const std::vector<std::pair<const char *, unsigned int>> &targets);

  public:
    GraphicalDevice();
    GraphicalDevice(const GraphicalDevice &) = delete;
    GraphicalDevice &operator=(const GraphicalDevice &) = delete;

    void process(Commands::Draw *);
    void process(Commands::DrawInstanced *);
    void process(Commands::Clear *);
    void process(Commands::SetViewport *);

    void start_frame();
    void end_frame();

    FramebufferContext create_context();

    Handles::VertexBuffer create(VertexBufferDefinition definition);
    Handles::IndexBuffer create(IndexBufferDefinition definition);
    Handles::ShaderBuffer create(ShaderBufferDefinition definition);
    Handles::Mesh create(MeshDefinition definition);
    Handles::Program create(ProgramDefinition definition);
    Handles::ComputeProgram create(ComputeProgramDefinition definition);
    Handles::UniformBuffer create(UniformBufferDefinition definition);
    Handles::Framebuffer create(FramebufferDefinition definition);
    Handles::Texture2D create(TextureDefinition2D definition);
    Handles::Texture2DFloat create(TextureDefinition2DFloat definition);
    Handles::Texture3D create(TextureDefinition3D definition);
    Handles::CubemapTexture create(CubeTextureDefinition definition);

    void release(Handles::VertexBuffer);
    void release(Handles::IndexBuffer);
    void release(Handles::Mesh);
    void release(Handles::Texture2D);
    void release(Handles::Texture3D);
    void release(Handles::CubemapTexture);
    void release(Handles::Framebuffer);
    void release(Handles::Program);
    void release(Handles::UniformBuffer);
    void release(Handles::Texture2DFloat);
    void release(Handles::ShaderBuffer);

    void load(Handles::UniformBuffer, const void *data, size_t size,
              UsagePattern usage = UsagePattern::Static);
    void load(Handles::VertexBuffer, const void *data, size_t size,
              UsagePattern usage = UsagePattern::Static);
    void load(Handles::IndexBuffer, const void *data, size_t size,
              UsagePattern usage = UsagePattern::Static);
    void load(Handles::ShaderBuffer, const void *data, size_t size, UsagePattern usage = UsagePattern::Dynamic);
    void load(Handles::Texture2D, unsigned int face, const void *data, size_t width, size_t height);

    void bind(Handles::Texture2D texture, unsigned int slot);
    void bind(Handles::UniformBuffer, unsigned int slot);
    void bind(Handles::ShaderBuffer, unsigned int slot);

    void attach(Handles::Framebuffer fb, AttachmentDefinition2D definition);
    void attach(Handles::Framebuffer fb, AttachmentDefinition2DFloat definition);
    void attach(Handles::Framebuffer fb, AttachmentDefinitionCube definition);

    void map_read(Handles::ShaderBuffer buffer, void **memory);
    void unmap(Handles::ShaderBuffer buffer);

    void dispatch(Handles::ComputeProgram, int x, int y, int z);

    void wait_for_compute();

    void debug(void *);

    Handles::Framebuffer default_fb();
};
} // namespace GL

#endif /*MY_GL_RENDERER*/

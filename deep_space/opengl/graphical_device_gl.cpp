#include "./graphical_device_gl.h"
#include "../graphical_context.h"

#include <glad/glad.h>
#include <numeric>
#include <stdexcept>

namespace GL {

void gl_error_callback(unsigned int source, unsigned int type, unsigned int id,
                       unsigned int severity, int length, const char *message,
                       const void *userParam) {
    std::cout << "OpenGL: " << message << std::endl;
}

GraphicalDevice::GraphicalDevice() {

    m_current_fb = 999999999;
    m_current_state = 0;
    m_current_program = 999999999;
    m_current_vao = 999999999;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_error_callback, nullptr);
    glLineWidth(2.);

    glClearColor(0.1, 0.1, 0.1, 1);

    for (int i = 0; i < UNIFORM_BUFFER_COUNT; i++) {
        m_current_uniform_buffers[i] = create_buffer();
        glBindBufferBase(GL_UNIFORM_BUFFER, i + 2, m_current_uniform_buffers[i]);
    }

    container.allocate<Framebuffer>(0, 1200, 800);
}

void GraphicalDevice::start_frame() { glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); }

void GraphicalDevice::end_frame() {
    m_current_fb = 0;
    auto fb = container.get<Framebuffer>(0);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
    glViewport(0, 0, fb.width, fb.height);
}

void GraphicalDevice::process(Commands::Draw *command) {

    if (command->fb.id ^ m_current_fb) {
        m_current_fb = command->fb.id;
        auto fb = container.get<Framebuffer>(command->fb.id);
        glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
        glViewport(0, 0, fb.width, fb.height);
    }

    if (command->program.id != m_current_program) {
        m_current_program = command->program.id;
        glUseProgram(container.get<Program>(command->program.id).id);
    }

    if (command->point_size != m_current_point_size) {
        m_current_point_size = command->point_size;
        glPointSize(command->point_size);
    }

    command->vao.id ^ m_current_vao &&
        (m_current_vao = command->vao.id,
         glBindVertexArray(container.get<Mesh>(command->vao.id).vao), 0);

    for (int i = 0; i < command->textures; i++) {
        unsigned int tex_type = command->texture_binding[i].binding_type;

        auto [tex_id, tex_target] =
            tex_type == Commands::TextureBinding::Texture2D
                ? std::pair(container.get<Texture2D>(command->texture_binding[i].texture.tex2d.id)
                                .texture,
                            GL_TEXTURE_2D)
                : std::pair(
                      container.get<CubemapTexture>(command->texture_binding[i].texture.cube.id)
                          .texture,
                      GL_TEXTURE_CUBE_MAP);

        if (m_current_textures[i].first != tex_type || m_current_textures[i].second != tex_id) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(tex_target, tex_id);
            m_current_textures[i] = std::pair(tex_type, tex_id);
        }
    }

    for (int i = 0; i < command->uniforms; i++) {
        auto binding = command->uniform_bindings[i];
        glNamedBufferData(m_current_uniform_buffers[i], binding.size, binding.data, GL_STREAM_DRAW);
    }

    unsigned int state_change = command->state ^ m_current_state;
    if (state_change & RenderState::DepthTest) {
        if (command->state & RenderState::DepthTest) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    if (state_change & RenderState::DepthWrite) {
        if (command->state & RenderState::DepthWrite) {
            glDepthMask(GL_TRUE);
        } else {
            glDepthMask(GL_FALSE);
        }
    }

    if (state_change & RenderState::Blending) {
        switch (command->state & RenderState::Blending) {
        case 0:
            glDisable(GL_BLEND);
            break;
        case 1:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case 2:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case 3:
            // glEnable(GL_BLEND);
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            // break;
        default:
            break;
        }
    }

    m_current_state = command->state;

    glDrawElements(geometry(command->geometry), command->size, GL_UNSIGNED_INT,
                   (const void *)command->offset);
}

void GraphicalDevice::process(Commands::DrawInstanced *command) {
    if (command->vao.id != m_current_vao) {
        glBindVertexArray(1);
    }
}

void GraphicalDevice::process(Commands::SetViewport *command) {}

void GraphicalDevice::process(Commands::Clear *clear) {
    if (m_current_fb != clear->fb.id) {
        m_current_fb = clear->fb.id;
        auto fb = container.get<Framebuffer>(clear->fb.id);
        glViewport(0, 0, fb.width, fb.height);
        glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
    }
    glClearColor(0.2, 0.2, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Handles::VertexBuffer GraphicalDevice::create(VertexBufferDefinition definition) {
    unsigned int vao ;
    glCreateBuffers(1, &vao);
    glNamedBufferStorage(vao, definition.size, definition.data, GL_DYNAMIC_STORAGE_BIT);
    return Handles::VertexBuffer{container.allocate<VertexBuffer>(vao)};
}

Handles::IndexBuffer GraphicalDevice::create(IndexBufferDefinition definition) {
    unsigned int vao;
    glCreateBuffers(1, &vao);
    glNamedBufferStorage(vao, definition.size, definition.data, GL_DYNAMIC_STORAGE_BIT);
    return Handles::IndexBuffer{container.allocate<IndexBuffer>(vao)};
}

Handles::ShaderBuffer GraphicalDevice::create(ShaderBufferDefinition definition) {
    unsigned int buffer;
    glCreateBuffers(1, &buffer);
    std::cout << "Creating shader buffer with pattern: " << definition.pattern << std::endl;
    glNamedBufferStorage(buffer, definition.size, definition.data, pattern(definition.pattern));
    return Handles::ShaderBuffer{container.allocate<ShaderBuffer>(buffer)};
}

Handles::Mesh GraphicalDevice::create(MeshDefinition definition) {
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, container.get<VertexBuffer>(definition.indices.id).id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, container.get<IndexBuffer>(definition.indices.id).id);

    size_t stride, offset = 0;

    stride =
        std::accumulate(definition.layout.attributes.begin(), definition.layout.attributes.end(), 0,
                        [](int a, VertexLayout::Attribute b) { return a + b.size; });

    for (auto attribute : definition.layout.attributes) {
        glVertexAttribPointer(attribute.location, attribute.count, GL_FLOAT, GL_FALSE, stride,
                              (void *)offset);
        glEnableVertexAttribArray(attribute.location);
        offset += attribute.size;
    }
    glBindVertexArray(0);

    return Handles::Mesh{container.allocate<Mesh>(vao)};
}

unsigned int
GraphicalDevice::create_program(const char *name,
                                const std::vector<std::pair<const char *, unsigned int>> &targets) {
    char errors[2048];
    char log[512];

    unsigned int program = glCreateProgram();

    auto build_shader = [&](const char *source, unsigned int target) {
        if (source) {
            unsigned int shader = compile(target, source);
            bool success =
                shader && get_compile_logs(shader, log) && (glAttachShader(program, shader), 1);
            if (!success) {
                sprintf(errors, "Failed to compile %s in shader '%s', code: %d, reason:\n%s", "N/A",
                        name, target, log);
                throw std::runtime_error(errors);
            }
            glDeleteShader(shader);
        }
    };

    for (auto [source, target] : targets) {
        build_shader(source, target);
    }

    glLinkProgram(program);

    if (!get_linking_logs(program, log)) {
        sprintf(errors, "Failed to link shader '%s', reason:\n'%s'", name, log);
        throw std::runtime_error(errors);
    }

    return program;
}

Handles::ComputeProgram GraphicalDevice::create(ComputeProgramDefinition definition) {
    std::vector<std::pair<const char *, unsigned int>> programs = {
        std::pair(definition.source, GL_COMPUTE_SHADER)};

    return Handles::ComputeProgram{
        container.allocate<Program>(create_program(definition.name, programs))};
}

Handles::Program GraphicalDevice::create(ProgramDefinition definition) {
    std::vector<std::pair<const char *, unsigned int>> programs;
    if (definition.vertex) {
        programs.push_back(std::pair(definition.vertex, GL_VERTEX_SHADER));
    }
    if (definition.geometry) {
        programs.push_back(std::pair(definition.geometry, GL_GEOMETRY_SHADER));
    }
    if (definition.fragment) {
        programs.push_back(std::pair(definition.fragment, GL_FRAGMENT_SHADER));
    }

    return Handles::Program{container.allocate<Program>(create_program(definition.name, programs))};
}

Handles::Framebuffer GraphicalDevice::create(FramebufferDefinition definition) {
    unsigned int fb;
    glCreateFramebuffers(1, &fb);
    return Handles::Framebuffer{container.allocate<Framebuffer>(fb, 0, 0)};
}

Handles::Texture2D GraphicalDevice::create(TextureDefinition2D definition) {
    unsigned int texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureStorage2D(texture, 1, GL_RGBA8, definition.width, definition.height);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return Handles::Texture2D{
        container.allocate<Texture2D>(texture, definition.width, definition.height)};
}

Handles::Texture2DFloat GraphicalDevice::create(TextureDefinition2DFloat definition) {
    unsigned int texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureStorage2D(texture, 1, GL_RGBA16F, definition.width, definition.height);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return Handles::Texture2DFloat{
        container.allocate<Texture2D>(texture, definition.width, definition.height)};
}

void GraphicalDevice::load(Handles::Texture2D handle, unsigned int face, const void *data,
                           size_t width, size_t height) {
    glTextureStorage2D(container.get<Texture2D>(handle.id).texture, 1, GL_RGBA8, width, height);
}

Handles::CubemapTexture GraphicalDevice::create(CubeTextureDefinition definition) {
    unsigned int id;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
    glTextureStorage2D(id, 1, GL_RGBA8, definition.width, definition.height);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return Handles::CubemapTexture{
        container.allocate<CubemapTexture>(id, definition.width, definition.height)};
}

void GraphicalDevice::attach(Handles::Framebuffer fb_handle, AttachmentDefinition2D definition) {
    auto tex = container.get<Texture2D>(definition.texture.id);
    auto &fb = container.get<Framebuffer>(fb_handle.id);
    switch (definition.target) {
    case Color:
        fb.width = tex.width;
        fb.height = tex.height;
        glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, tex.texture, 0);
        unsigned int rbo;
        // glCreateRenderbuffers(1, &rbo);
        // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
        // glNamedFramebufferRenderbuffer(fb.id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        break;
    case DepthStencil:
        glNamedFramebufferTexture(fb.id, GL_DEPTH_STENCIL, tex.texture, 0);
        break;
    case Depth:
        break;
    }
    if (glCheckNamedFramebufferStatus(container.get<Framebuffer>(fb_handle.id).id,
                                      GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Failed to complete framebuffer");
    }
}

void GraphicalDevice::attach(Handles::Framebuffer fb_handle,
                             AttachmentDefinition2DFloat definition) {
    auto tex = container.get<Texture2D>(definition.texture.id);
    auto &fb = container.get<Framebuffer>(fb_handle.id);
    unsigned int buffers[1] = {GL_COLOR_ATTACHMENT0};

    switch (definition.target) {
    case Color:
        fb.width = tex.width;
        fb.height = tex.height;
        glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, tex.texture, 0);
        glNamedFramebufferDrawBuffers(fb.id, 1, buffers);

        // std::cout << "Attaching rb: " << fb.width << " " << fb.height << std::endl;
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb.width, fb.height);
        glNamedFramebufferRenderbuffer(fb.id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // glBindFramebuffer(GL_FRAMEBUFFER, fb.id);
        // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        break;
    case DepthStencil:
        glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, tex.texture, 0);
        break;
    case Depth:
        break;
    }
    if (auto error = glCheckNamedFramebufferStatus(container.get<Framebuffer>(fb_handle.id).id,
                                                   GL_FRAMEBUFFER);
        error != GL_FRAMEBUFFER_COMPLETE) {
        char error_msg[256];
        sprintf(error_msg, "Failed to complete float texture framebuffer: %d %d %d %d %d", error,
                GL_FRAMEBUFFER_UNDEFINED, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
                GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, GL_FRAMEBUFFER_COMPLETE);
        throw std::runtime_error(error_msg);
    }
}

void GraphicalDevice::attach(Handles::Framebuffer fb_handle, AttachmentDefinitionCube definition) {
    auto tex = container.get<CubemapTexture>(definition.texture.id);
    auto &fb = container.get<Framebuffer>(fb_handle.id);
    unsigned int buffers[1] = {GL_COLOR_ATTACHMENT0};

    switch (definition.target) {
    case Color:
        fb.width = tex.width;
        fb.height = tex.height;
        glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, tex.texture, 0);
        glNamedFramebufferDrawBuffers(fb.id, 1, buffers);
        break;
    case DepthStencil:
        glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, tex.texture, 0);
        break;
    case Depth:
        break;
    }
    if (auto error = glCheckNamedFramebufferStatus(container.get<Framebuffer>(fb_handle.id).id,
                                                   GL_FRAMEBUFFER);
        error != GL_FRAMEBUFFER_COMPLETE) {
        char error_msg[256];
        sprintf(error_msg, "Failed to complete cube framebuffer: %d %d %d %d %d", error,
                GL_FRAMEBUFFER_UNDEFINED, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
                GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER, GL_FRAMEBUFFER_COMPLETE);
        throw std::runtime_error(error_msg);
    }
}

unsigned int GraphicalDevice::create_buffer() {
    unsigned int buffer;
    glCreateBuffers(1, &buffer);
    return buffer;
}

Handles::UniformBuffer GraphicalDevice::create(UniformBufferDefinition definition) {
    auto buffer = create_buffer();
    return Handles::UniformBuffer{container.allocate<UniformBuffer>(buffer)};
}

int GraphicalDevice::compile(unsigned int target, const char *source) {
    unsigned int shader = glCreateShader(target);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

void GraphicalDevice::load(Handles::UniformBuffer handle, const void *data, size_t size,
                           UsagePattern usage) {
    unsigned int a = container.get<UniformBuffer>(handle.id).buffer;
    glNamedBufferData(a, size, data, pattern(usage));
}

void GraphicalDevice::load(Handles::VertexBuffer handle, const void *data, size_t size,
                           UsagePattern usage) {
	std::cout<<"Loading vertices..."<<std::endl;
    unsigned int buffer = container.get<VertexBuffer>(handle.id).id;
    glNamedBufferData(buffer, size, data, pattern(usage));
}

void GraphicalDevice::load(Handles::IndexBuffer handle, const void *data, size_t size,
                           UsagePattern usage) {
    unsigned int buffer = container.get<IndexBuffer>(handle.id).id;
    glNamedBufferData(buffer, size, data, pattern(usage));
}

void GraphicalDevice::load(Handles::ShaderBuffer handle, const void *data, size_t size,
                           UsagePattern usage) {
    unsigned int a = container.get<ShaderBuffer>(handle.id).id;
    glNamedBufferSubData(a, 0, size, data);
}

void GraphicalDevice::bind(Handles::UniformBuffer buffer, unsigned int slot) {
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, container.get<UniformBuffer>(buffer.id).buffer);
}

void GraphicalDevice::bind(Handles::ShaderBuffer buffer, unsigned int slot) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, container.get<ShaderBuffer>(buffer.id).id);
}

void GraphicalDevice::map_read(Handles::ShaderBuffer buffer, void **memory) {
    *memory = glMapNamedBuffer(container.get<ShaderBuffer>(buffer.id).id, GL_READ_ONLY);
}

void GraphicalDevice::unmap(Handles::ShaderBuffer buffer) {
    glUnmapNamedBuffer(container.get<ShaderBuffer>(buffer.id).id);
}

void GraphicalDevice::dispatch(Handles::ComputeProgram program, int x, int y, int z) {
    if (m_current_program != program.id) {
        glUseProgram(container.get<Program>(program.id).id);
        m_current_program = program.id;
    }

    glDispatchCompute(x, y, z);
}

void GraphicalDevice::wait_for_compute() { glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); }

void GraphicalDevice::debug(void *ptr) {
    unsigned int handle = container.get<CubemapTexture>(*(unsigned int *)ptr).texture;

    unsigned char *tex_data = new unsigned char[1024 * 1024 * 4];
    unsigned char *tex_source = tex_data;
    for (int i = 0; i < 1024 * 1024; i++) {
        *tex_data++ = 0;
        *tex_data++ = 0;
        *tex_data++ = 255;
        *tex_data++ = 255;
    }
    glTextureSubImage3D(handle, 0, 0, 0, 0, 1024, 1024, 1, GL_RGBA, GL_UNSIGNED_BYTE, tex_source);
}

Handles::Framebuffer GraphicalDevice::default_fb() { return Handles::Framebuffer{0}; }

unsigned int GraphicalDevice::tex_format(TextureFormat f) {
    switch (f) {
    case Grayscale:
        return GL_RED;
    case StencilDepth:
        return GL_DEPTH_STENCIL;
    case Rgb:
        return GL_RGB;
    case Rgba:
        return GL_RGBA;
        break;
    }
}

unsigned int GraphicalDevice::geometry(Geometry g) {
    switch (g) {
    case Points:
        return GL_POINTS;
    case Lines:
        return GL_LINES;
    case Triangles:
        return GL_TRIANGLES;
    case Quads:
    default:
        return GL_QUADS;
    }
}

unsigned int GraphicalDevice::pattern(UsagePattern p) {
    switch (p) {
    case Static:
        return GL_STATIC_DRAW;
    case Dynamic:
        return GL_DYNAMIC_DRAW;
    case Read:
        return GL_MAP_READ_BIT;
    case Write:
        return GL_DYNAMIC_STORAGE_BIT;
    default:
        return 0;
    }
}

bool GraphicalDevice::get_compile_logs(unsigned int shader, char *logs) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) {
        return true;
    } else {
        glGetShaderInfoLog(shader, 512, NULL, logs);
        return false;
    }
}

bool GraphicalDevice::get_linking_logs(unsigned int program, char *logs) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) {
        return true;
    } else {
        glGetProgramInfoLog(program, 512, NULL, logs);
        return false;
    }
}

} // namespace GL

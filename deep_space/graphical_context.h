#ifndef GRAPHICAL_CONTEXT
#define GRAPHICAL_CONTEXT

#include "command_queue.h"

#include <glm/fwd.hpp>
#include <memory>

class ProgramContext;

class Program;
class Framebuffer;
class Texture2D;

class GraphicalContext {
    BaseCommandQueue &m_queue;

  public:
    GraphicalContext(BaseCommandQueue &queue);

    // Commands
    void draw(Commands::Draw);
    void draw_instanced(Commands::DrawInstanced);
    void clear(Commands::Clear);
    void set_viewport(Commands::SetViewport);
};

class FramebufferContext {
    std::shared_ptr<Framebuffer> m_fb = nullptr;
    GraphicalContext &m_context;

  public:
    FramebufferContext(GraphicalContext &context, std::shared_ptr<Framebuffer> fb);

    void set_framebuffer(std::shared_ptr<Framebuffer> fb);

    // Commands
    void draw(Commands::Draw);
    void draw_instanced(Commands::DrawInstanced);
    void clear(Commands::Clear);
    void set_viewport(Commands::SetViewport);
};

class ProgramContext {
    AlphaBlending m_blending = AlphaBlending::None;
    bool m_depth_test = true;
    bool m_depth_write = true;
    float m_point_size = 1;

    FramebufferContext &m_context;

  protected:
    std::shared_ptr<Program> m_program;

  public:
    ProgramContext(FramebufferContext &context, std::shared_ptr<Program> program);

    void draw(Commands::Draw);

    void set_blending(AlphaBlending blending);
    void set_depth_write(bool);
    void set_point_size(float);
    void set_depth_test(bool);

    void draw_instanced(Commands::DrawInstanced);
};

#endif /*GRAPHICAL_CONTEXT*/

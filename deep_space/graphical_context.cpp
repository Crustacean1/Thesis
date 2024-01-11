#include "./graphical_context.h"
#include "./graphical_device.h"
#include "components/framebuffer.h"
#include "components/program.h"

GraphicalContext::GraphicalContext(BaseCommandQueue &queue) : m_queue(queue) {}

void GraphicalContext::draw(Commands::Draw cmd) { m_queue.post(cmd); }
void GraphicalContext::draw_instanced(Commands::DrawInstanced cmd) { m_queue.post(cmd); }
void GraphicalContext::clear(Commands::Clear cmd) { m_queue.post(cmd); }
void GraphicalContext::set_viewport(Commands::SetViewport cmd) { m_queue.post(cmd); }

FramebufferContext::FramebufferContext(GraphicalContext &context, std::shared_ptr<Framebuffer> fb)
    : m_context(context), m_fb(fb) {}

void FramebufferContext::draw(Commands::Draw cmd) {
    cmd.fb = m_fb->framebuffer_handle;
    m_context.draw(cmd);
}

void FramebufferContext::draw_instanced(Commands::DrawInstanced cmd) {
    m_context.draw_instanced(cmd);
}
void FramebufferContext::set_viewport(Commands::SetViewport cmd) { m_context.set_viewport(cmd); }
void FramebufferContext::clear(Commands::Clear cmd) {
    cmd.fb = m_fb->framebuffer_handle;
    m_context.clear(cmd);
}
void FramebufferContext::set_framebuffer(std::shared_ptr<Framebuffer> fb) { m_fb = fb; }

ProgramContext::ProgramContext(FramebufferContext &context, std::shared_ptr<Program> program)
    : m_context(context), m_program(program) {}

void ProgramContext::draw(Commands::Draw command) {
    unsigned int state = (m_depth_test ? RenderState::DepthTest : 0) |
                         (m_depth_write ? RenderState::DepthWrite : 0) | (m_blending);
    int mask = ~((RenderState::DepthTest) | (RenderState::DepthWrite) | (RenderState::Blending));
    command.state = (command.state & mask) | state;
    command.program = m_program->handle;
    command.point_size = m_point_size;

    m_context.draw(command);
}

void ProgramContext::set_depth_test(bool test) { m_depth_test = test; }
void ProgramContext::set_depth_write(bool write) { m_depth_write = write; }
void ProgramContext::set_point_size(float size) { m_point_size = size; }
void ProgramContext::set_blending(AlphaBlending blending) { m_blending = blending; }

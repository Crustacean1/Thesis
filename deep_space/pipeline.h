#ifndef PIPELINE
#define PIPELINE

#include "components/framebuffer.h"
#include "graphical_context.h"
#include "node/node.h"
#include "opengl/graphical_device_gl.h"

#include <functional>
#include <ranges>
#include <vector>

class Camera;

template <typename Q, typename... Components> class RendererWrapper {
    Q m_renderer;

    template <typename... Args> void render_groups(std::tuple<Args...> tuple) {
        ((std::for_each(
             std::get<Args>(tuple).begin(), std::get<Args>(tuple).end(),
             [this](const std::pair<Node *, Components *> &c) -> void { m_renderer.render(c); })),
         ...);
    }

  public:
    RendererWrapper(Q renderer) : m_renderer(renderer) {}

    template <typename T> void render(T &component_manager) {
        auto groups = std::make_tuple(component_manager.template components<Components>()...);
        render_groups(groups);
    }
};

class IRenderingPipeline {
  public:
    virtual void render(Node *node, Camera *camera) = 0;
};

void init_camera(FramebufferContext &context, Node *node, Camera *camera);

template <typename T, typename... Params> struct RendererBuilder {
    typedef RendererWrapper<T, Params...> Output;

    const char *program_source;
    std::function<T(ProgramContext context)> create_fn;

    Output build(GL::GraphicalDevice &device, FramebufferContext &context) {
        auto program_context =
            ProgramContext(context, std::make_shared<Program>(device, program_source));
        return Output(create_fn(program_context));
    }
};

template <typename Q, typename... Renderers> class RenderingPipeline : public IRenderingPipeline {
    FramebufferContext m_context;
    Q &m_component_manager;
    std::tuple<typename Renderers::Output...> m_renderers;
    std::shared_ptr<Framebuffer> m_target;

  public:
    RenderingPipeline(GL::GraphicalDevice &device, FramebufferContext context, Q &component_manager,
                      std::tuple<Renderers...> renderers)
        : m_context(context), m_component_manager(component_manager),
          m_renderers(std::get<Renderers>(renderers).build(device, m_context)...) {}

    void render(Node *node, Camera *camera) override {
        init_camera(m_context, node, camera);
        (std::get<typename Renderers::Output>(m_renderers).render(m_component_manager), ...);
    }
};

template <typename... Renderers> class ParitalRenderingPipelineBuilder {
    std::tuple<Renderers...> m_renderers;

  public:
    ParitalRenderingPipelineBuilder(std::tuple<Renderers...> renderers) : m_renderers(renderers) {}

    ParitalRenderingPipelineBuilder() {}

    template <typename T, typename... TParams>
    ParitalRenderingPipelineBuilder<RendererBuilder<T, TParams...>, Renderers...>
    add(std::function<T(ProgramContext)> t, const char *program) {
        std::tuple<RendererBuilder<T, TParams...>, Renderers...> builder_tuple = std::tuple(
            RendererBuilder<T, TParams...>{program, t}, std::get<Renderers>(m_renderers)...);

        return ParitalRenderingPipelineBuilder<RendererBuilder<T, TParams...>, Renderers...>(
            builder_tuple);
    }

    template <typename T, typename... TParams>
    ParitalRenderingPipelineBuilder<RendererBuilder<T, TParams...>, Renderers...>
    add(const char *program) {
        std::tuple<RendererBuilder<T, TParams...>, Renderers...> builder_tuple = std::tuple(
            RendererBuilder<T, TParams...>{program, [](auto program) { return T(program); }},
            std::get<Renderers>(m_renderers)...);

        return ParitalRenderingPipelineBuilder<RendererBuilder<T, TParams...>, Renderers...>(
            builder_tuple);
    }

    template <typename Q>
    std::shared_ptr<RenderingPipeline<Q, Renderers...>>
    build(GL::GraphicalDevice &device, GraphicalContext &context, Q &manager) {
        auto fb_context = FramebufferContext(context, Framebuffer::default_fb(device));

        return std::make_shared<RenderingPipeline<Q, Renderers...>>(device, fb_context, manager,
                                                                    m_renderers);
    }
};

#endif /*PIPELINE*/

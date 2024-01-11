#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include "camera.h"
#include "component_manager.h"
#include "graphical_context.h"
#include "node/node_context.h"
#include "runtime_exception.h"

#include <ranges>

namespace v = std::ranges::views;

template <typename... Components>
class ComponentManager : public Components::Manager..., public BaseComponentManager {
  public:
    template <typename T> void attach_component(NodeManager &node_manager, NodeHandle it, T &&t) {
        ((typename T::Manager *)this)->attach(node_manager, it, std::move(t));
    }
    template <typename T> T &get_component(NodeHandle it) { return T::Manager::get(it); }
    template <typename T> std::vector<std::pair<NodeHandle, T>> &components() {
        return T::Manager::visible_components();
    }
    void update() { (Components::Manager::process_commands(), ...); }
};

struct NodeBuilder {

    NodeManager &node_manager;
    BaseComponentManager *component_manager;
    NodeHandle node;

  public:
    template <typename Q> NodeBuilder &component(Q &&component) {
        dynamic_cast<typename Q::Manager *>(component_manager)
            ->attach(node_manager, node, std::move(component));
        return *this;
    }
    operator NodeHandle() { return node; }
};

class ISceneManager {
  public:
    virtual NodeBuilder node(Transform transform = Transform(), NodeHandle *parent = nullptr) = 0;
    virtual Node &node(NodeHandle it) = 0;
};

template <typename... Components> class SceneManager : public ISceneManager {
    std::vector<std::pair<Node *, Camera *>> cameras;
    NodeManager node_manager;
    ComponentManager<Components...> component_manager;

  public:
    template <typename T> void attach(NodeHandle it, T &&component) {
        component_manager.attach_component(node_manager, it, std::move(component));
        ((typename T::Manager *)&component_manager)->on_attach(it);
    }

    template <typename T> T &component(NodeHandle node) {
        return component_manager.template get_component<T>(node);
    }

    template <typename T> auto components() {
        return component_manager.template components<T>() |
               v::transform([this](auto &pair) -> std::pair<Node *, T *> {
                   return std::pair(node_manager.get(pair.first), &pair.second);
               });
    }

    NodeBuilder node(Transform transform = Transform(), NodeHandle *parent = nullptr) override {
        auto it = parent == nullptr ? node_manager.create(transform)
                                    : node_manager.create(transform, parent->i);
        return NodeBuilder{node_manager, &component_manager, NodeHandle{it, node_manager}};
    }

    Node &node(NodeHandle it) override { return *node_manager.get(it); }

    template <typename T> typename T::Manager &get_manager() {
        return (typename T::Manager &)component_manager;
    }

    void update() {
        node_manager.update();
        component_manager.update();
    }

    void render(GL::GraphicalDevice &device, BaseCommandQueue &queue) {
        auto cameras_it = components<Camera>() |
                          std::ranges::views::filter([](const std::pair<Node *, Camera *> &pair) {
                              return pair.second->pipeline != nullptr;
                          });

        cameras.clear();
        cameras.insert(cameras.begin(), cameras_it.begin(), cameras_it.end());

        std::sort(
            cameras.begin(), cameras.end(),
            [](const std::pair<Node *, Camera *> &a, const std::pair<Node *, Camera *> &b) -> bool {
                return a.second->layer > b.second->layer;
            });

        device.start_frame();

        for (auto &[node, camera] : cameras) {
            if (camera->clear) {
                queue.post(Commands::Clear{.fb = camera->framebuffer()->framebuffer_handle});
            }
            camera->pipeline->render(node, camera);

            queue.submit(device);
            queue.clear();
        }

        device.end_frame();
    }

    void remove(NodeHandle it) {}
};

template <typename... Components> class SceneManagerBuilder {
  public:
    template <typename T> SceneManagerBuilder<T, Components...> register_component() {
        return SceneManagerBuilder<T, Components...>();
    }
    SceneManager<Components...> build() { return SceneManager<Components...>(); }
};

#endif /*SCENE_MANAGER*/

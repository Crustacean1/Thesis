#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "node/node.h"
#include "node/node_context.h"
#include "runtime_exception.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <ranges>
#include <vector>

class CommandProcessor {
    std::vector<std::function<void()>> command_buffer;

  public:
    void add_command(std::function<void()> command) { command_buffer.push_back(command); }
    void process_commands() {
        while (!command_buffer.empty()) {
            command_buffer.back()();
            command_buffer.pop_back();
        }
    }
};

class BaseComponentManager {
  public:
    virtual void id() {}
};

template <typename T> class GenericComponentManager : protected CommandProcessor {
  protected:
    std::vector<std::pair<NodeHandle, T>> components;

  public:
    void on_attach(NodeHandle &it);

    T &attach(NodeManager &node_manager, NodeHandle it, T &&component) {
        components.emplace_back(std::make_pair(it, std::forward<T>(component)));
        node_manager.register_on_delete(it, [this, it]() {
            add_command([this, it]() {
                // TODO: Remove component
            });
        });
        return components.back().second;
    }

    T &get(NodeHandle it) {
        auto component =
            std::find_if(components.begin(), components.end(),
                         [it](auto &component_node) { return component_node.first == it; });
        if (component != components.end()) {
            return component->second;
        }
        throw RuntimeException::error("No component attached");
    }

    void detach(NodeManager &node_manager, NodeHandle node) {
        node_manager.clear_callbacks(typeid(T).hash_code(), node);
    }

    std::vector<std::pair<NodeHandle, T>> &visible_components() { return components; }
};

#endif /*COMPONENT_MANAGER*/

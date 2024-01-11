#include "./node_context.h"
#include <iostream>

NodeManager::NodeManager() {}
Transform NodeManager::global(unsigned int node) {
    auto transform = nodes[node].transform;
    while (nodes[node].parent() != -1) {
        node = nodes[node].parent();
        transform = nodes[node].transform * transform;
    }
    return transform;
}

Transform NodeManager::global(Node *node) {
    Transform transform = node->transform;
    while (node->parent() != -1) {
        node = &nodes[node->parent()];
        transform = node->transform * transform;
    }
    return transform;
}

unsigned int NodeManager::create(Transform transform, unsigned int parent) {
    nodes.push_back(Node{*this, transform, parent});
    return nodes.size() - 1;
}

void NodeManager::default_camera(unsigned int id) { default_camera_id = id; }

unsigned int NodeManager::default_camera() { return default_camera_id; }

Transform NodeManager::local(unsigned int node) { return nodes[node].transform; }

void NodeManager::register_on_delete(NodeHandle it, std::function<void()> fn) {
    get(it)->delete_callbacks.push_back(std::pair(0, fn));
}
void NodeManager::register_on_movement(NodeHandle it, std::function<void(Transform)> fn) {
    get(it)->movement_callbacks.push_back(std::pair(0, fn));
}

Node *NodeManager::get(NodeHandle it) { return nodes.data() + it.i; }

void NodeManager::update() {
    for (auto &node : nodes) {
        node.process_callbacks();
        node.refresh();
    }
}

#include "./node.h"
#include "./node_context.h"
#include <iostream>

void Node::move(glm::vec3 direction) {
    transform.move(direction);
    moved = true;
}

void Node::turn(glm::quat axis) {
    transform.turn(axis);
    moved = true;
}

void Node::orientation(glm::quat axis) {
    transform.orientation(axis);
    moved = true;
}

void Node::refresh() { global_transform = m_manager.global(this); }

void Node::process_callbacks() {
    if (moved == true) {
        for (auto [id, callback] : movement_callbacks) {
            callback(transform);
        }
        moved = false;
    }
}

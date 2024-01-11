#ifndef NODE
#define NODE

#include "../transform.h"
#include <functional>
#include <vector>

class NodeManager;

class Node {
    friend NodeManager;

    std::vector<std::pair<unsigned int, std::function<void()>>> delete_callbacks;
    std::vector<std::pair<unsigned int, std::function<void(Transform)>>> movement_callbacks;

    bool moved = false;

    unsigned int parent_id = -1;

    void process_callbacks();

    NodeManager &m_manager;

  public:
    Node(NodeManager &manager, Transform transform = Transform{}, unsigned int parent = -1)
        : m_manager(manager), transform(transform), parent_id(parent) {}
    Transform global_transform;
    Transform transform;

    void move(glm::vec3 direction);
    void turn(glm::quat axis);
    void orientation(glm::quat axis);

    void reset_movement();

	void refresh();

    unsigned int parent() { return parent_id; }
};

#endif /*NODE*/

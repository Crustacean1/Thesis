#ifndef NODE_CONTEXT
#define NODE_CONTEXT

#include <vector>

#include "./node.h"

class NodeManager;

struct NodeHandle {
    unsigned int i;
    NodeManager &manager;
    bool operator==(const NodeHandle &it) { return i == it.i; }
};

class NodeManager {
    std::vector<Node> nodes;

    unsigned int create(Node node);

    unsigned int default_camera_id;

  public:
    NodeManager();

    void register_on_delete(NodeHandle it, std::function<void()> fn);
    void register_on_movement(NodeHandle it, std::function<void(Transform)> fn);

    void clear_callbacks(unsigned int listener_type, NodeHandle node);

    Node *get(NodeHandle it);

    unsigned int create(Transform transform = Transform{}, unsigned int parent = -1);

    Node &node(unsigned int id) { return nodes[id]; }

    void default_camera(unsigned int id);
    unsigned int default_camera();

    Transform global(unsigned int node);
    Transform global(Node *node);

    Transform local(unsigned int node);

    void update();
};

#endif /*NODE_CONTEXT*/

#include "./camera_manager.h"
#include "../camera.h"

void CameraManager::attach(NodeManager &node_manager, NodeHandle it, Camera &&component) {
    GenericComponentManager<Camera>::attach(node_manager, it, std::move(component));

    //node_manager.register_on_movement(it, [this, it](auto transform) {
        //add_command([this, transform, it]() { get(it).update_uniform(transform); });
    //});
}

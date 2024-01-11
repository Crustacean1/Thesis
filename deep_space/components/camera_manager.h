#ifndef CAMERA_MANAGER
#define CAMERA_MANAGER

#include "../component_manager.h"

class Camera;

class CameraManager : public GenericComponentManager<Camera> {
  public:
    void attach(NodeManager &node_manager, NodeHandle it, Camera &&component);
};

#endif /*CAMERA_MANAGER*/

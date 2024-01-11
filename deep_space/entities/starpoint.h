#ifndef STARTPOINT
#define STARTPOINT

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../vertices.h"
#include <memory>

class StarpointManager;

struct StarPoint {
    typedef StarpointManager Manager;
    bool enabled = false;
};

class StarpointManager : public GenericComponentManager<StarPoint> {
  public:
    void process_commands() {
        for (auto &component : components) {
            component.second.enabled = false;
        }
    }
};

#endif /*STARTPOINT*/

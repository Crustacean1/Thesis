#ifndef SPINNER
#define SPINNER

#include "../component_manager.h"

#include <chrono>

class SpinnerManager;

struct Spinner {
    typedef SpinnerManager Manager;

    glm::vec3 axis;
    float speed = 1;
};

class SpinnerManager : public GenericComponentManager<Spinner> {
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

  public:
    SpinnerManager() {
        t1 = std::chrono::high_resolution_clock::now();
        t2 = t1;
    }

    void process_commands() {
		t2 = t1;
        t1 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> delta = t1 - t2;
        for (auto &[node, spinner] : components) {
            node.manager.node(node.i).turn(glm::angleAxis(spinner.speed * 0.0001f * spinner.speed * (float)delta.count(), spinner.axis));
        }
    }
};

#endif /*SPINNER*/

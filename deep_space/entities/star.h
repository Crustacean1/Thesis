#ifndef STAR_ENTITY
#define STAR_ENTITY

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../components/uniform_buffer.h"
#include "../vertices.h"
#include "../entities/noise.h"

#include <chrono>
#include <memory>

class StarManager;

struct Star {
    typedef StarManager Manager;

    struct StarMaterial {
        glm::vec4 colors[4] = {glm::vec4(1,0,0,0),glm::vec4(1,0.1,0,0), glm::vec4(1,0.4,0,0), glm::vec4(1,1,0,0)};
        SimplexNoise noise;
        float radius = 1;
        float time = 1;
        float speed = 0.5;
    };

    std::shared_ptr<Mesh<Position, Normal, Tex>> mesh;
    StarMaterial material;
};

class StarManager : public GenericComponentManager<Star> {
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

  public:
    StarManager() {
        t1 = std::chrono::high_resolution_clock::now();
        t2 = t1;
    }
    void process_commands() {
        t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> delta = t1 - t2;
        for (auto &star : components) {
            star.second.material.time = delta.count() * 0.001 * star.second.material.speed;
        }
    }
};

#endif /*STAR_ENTITY*/

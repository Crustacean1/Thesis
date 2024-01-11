#ifndef SPACEBOX
#define SPACEBOX

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../components/uniform_buffer.h"
#include "../vertices.h"

struct SpaceBox {
    typedef GenericComponentManager<SpaceBox> Manager;

    struct SimplexNoise {
        glm::vec4 cloud_color = glm::vec4(10, 10, 8, 0);
        int iterations = 16;
        int octaves = 10;
        float density = 1;
        float scatt = 0.5;
        float scale = 3.5;
        float fractal = 1.8;
        float base = 0;
        float brightness = 1;
        float radius = 1;
    };

    struct Stars {
        glm::vec4 positions[16] = {
            glm::vec4(0.75, 0, 0, 0),
        };
        glm::vec4 colors[16] = {glm::vec4(1, 1, 1, 0)};
        int star_count = 1;
    };

    const int fragments;
    std::shared_ptr<Mesh<Position, Color4>> stars_mesh;
    std::shared_ptr<Mesh<Position, Normal, Tex>> mesh;
    SimplexNoise noise;
    Stars stars;
    int current_fragment = 0;
    bool stars_rendered;
};

#endif /*SPACEBOX*/

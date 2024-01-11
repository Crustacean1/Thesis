#ifndef PLANET_ENTITY
#define PLANET_ENTITY

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../components/uniform_buffer.h"
#include "../vertices.h"
#include <memory>

struct Planet {
    struct Light {
        glm::vec3 position;
        float __0;
        glm::vec3 color;
        float __1;
    };

    struct Surface {
        glm::vec4 colors[4] = {glm::vec4(0, 0.4, 1, 1), glm::vec4(0, 0.4, 1, 1), glm::vec4(0, 1, 0, 1),
                               glm::vec4(0.3, 0.7, 0.2, 1)};

        glm::vec4 scattering = glm::vec4(0.25, 0.65, 1.5, 0);
        Light light = {glm::normalize(-glm::vec3(2, 5, 0)), 0, glm::vec3(1, 1, 1), 0};
        float density_falloff = 1;
        float planet_radius = 0.8;
        int octaves = 10;
        float intensity = 5;
        float offset = 1;
        float scale = 1;
        float increment = 2;
        float surface_increment = 2;
        float surface_base = 0.5;
        float surface_factor = 0.3;
    };

    typedef GenericComponentManager<Planet> Manager;
    std::shared_ptr<Mesh<Position, Normal, Tex>> box;
    Surface uniform;
};

#endif /*PLANET_ENTITY*/

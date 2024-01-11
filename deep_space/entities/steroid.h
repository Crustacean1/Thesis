#ifndef STEROID_ENTITY
#define STEROID_ENTITY

#include "../components/mesh.h"
#include "../component_manager.h"
#include "noise.h"

#include <glm/ext/vector_float4.hpp>
#include <memory>

struct AsteroidVolume {
    typedef GenericComponentManager<AsteroidVolume> Manager;

    std::shared_ptr<MeshPNT> mesh;
    struct Blob {
        glm::vec4 position;
        float intensity = 1;
        float radius = 1;
        float _padding[2];
    };
    struct Material {
        Blob balls[128];
        unsigned int ball_count = 0;
        float threshold = 0.5;
        SimplexNoise noise;
        int iterations = 32;

    } material;
};

#endif /*STEROID_ENTITY*/

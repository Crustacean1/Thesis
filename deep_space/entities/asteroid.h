#ifndef ASTEROID_ENTITY
#define ASTEROID_ENTITY

#include "../components/mesh.h"
#include "./steroid.h"
#include "../component_manager.h"

#include <memory>

struct Asteroid {
    typedef GenericComponentManager<Asteroid> Manager;

    std::shared_ptr<MeshPN> mesh;
    AsteroidVolume::Material material;

    struct Texture {
        int octaves = 5;
        float scale = 1;
        float increment = 2;
        float offset = 0.5;
        float intensity = 0.2;
        float normal_strength = 0.3;
    } texture;
    bool wiremesh = false;
	bool normal = false;
};

#endif /*ASTEROID_ENTITY*/

#ifndef NOISE_MESH
#define NOISE_MESH

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../components/uniform_buffer.h"
#include "../vertices.h"

struct NoiseMesh {
    struct SimplexNoise {
        glm::vec4 directions[27];
        int hash[512];
        int octaves;
        float brightness;
        float baseline;
        float scale;
        float displacement;
        float increment;
    };

    typedef GenericComponentManager<NoiseMesh> Manager;
    float scale;
    std::shared_ptr<MeshPNT> mesh;
    SimplexNoise noise;
};

#endif /*NOISE_MESH*/

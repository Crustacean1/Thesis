#ifndef SPACE_BACKGROUND_ENTITY
#define SPACE_BACKGROUND_ENTITY

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../vertices.h"

#include <memory>

struct SpaceBackgroundEntity {
    typedef GenericComponentManager<SpaceBackgroundEntity> Manager;

    std::shared_ptr<MeshPNT> cube_mesh;
    std::shared_ptr<CubemapTexture> texture;
};

#endif /*SPACE_BACKGROUND_ENTITY*/

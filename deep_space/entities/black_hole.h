#ifndef BLACK_HOLE_ENTITY
#define BLACK_HOLE_ENTITY

#include "../component_manager.h"
#include "../components/mesh.h"
#include "../components/texture.h"
#include "../vertices.h"

#include <memory>

struct BlackHole {
    typedef GenericComponentManager<BlackHole> Manager;
	std::shared_ptr<MeshPNT> cube_mesh;
	std::shared_ptr<CubemapTexture> surrounding;

	struct BlackHoleProperties{
		float radius = 11.0;
		float step = 0.08;
		float strength = 350;
		int iterations = 8192;
	} black_hole_properties;
};

#endif /*BLACK_HOLE_ENTITY*/

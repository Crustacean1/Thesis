#ifndef HDR_SOURCE
#define HDR_SOURCE

#include "../components/mesh.h"
#include "component_manager.h"
#include "components/texture.h"

#include <memory>

struct HdrDisplay {
    typedef GenericComponentManager<HdrDisplay> Manager;
    std::shared_ptr<MeshPNT> plane;
    std::shared_ptr<Texture2DFloat> texture;
};

#endif /*HDR_SOURCE*/

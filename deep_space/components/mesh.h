#ifndef MESH
#define MESH

#include <vector>

#include "../graphical_device.h"
#include "../vertices.h"

namespace GL {
class GraphicalDevice;
}

Handles::VertexBuffer create_vertex_buffer(GL::GraphicalDevice &device, const void *data,
                                           size_t size);
Handles::IndexBuffer create_index_buffer(GL::GraphicalDevice &device, const void *data,
                                         size_t size);
Handles::Mesh create_mesh(GL::GraphicalDevice &device, const Handles::VertexBuffer &vertex_buffer,
                          const Handles::IndexBuffer &index_buffer, const VertexLayout &layout);

void load(GL::GraphicalDevice &device, Handles::VertexBuffer, const void *data, unsigned int);
void load(GL::GraphicalDevice &device, Handles::IndexBuffer, const void *data, unsigned int);

template <typename... Attrs> class Mesh {
    GL::GraphicalDevice &m_device;

    size_t m_size = 0;
    size_t m_offset = 0;
    Geometry m_geometry = Geometry::Triangles;

    static VertexLayout layout();

  public:
    typedef std::vector<Vertex<Attrs...>> VertexData;
    typedef std::vector<unsigned int> IndexData;

    const Handles::VertexBuffer vertices;
    const Handles::IndexBuffer indices;
    const Handles::Mesh mesh;

    Mesh(GL::GraphicalDevice &device, Geometry geometry)
        : Mesh(device, VertexData(), IndexData(), geometry) {}

    Mesh(GL::GraphicalDevice &device,
         const std::pair<std::vector<Vertex<Attrs...>>, std::vector<unsigned int>> &data,
         Geometry geometry = Geometry::Triangles)
        : Mesh(device, data.first, data.second, geometry) {}

    Mesh(GL::GraphicalDevice &device, const VertexData &d_vertices, const IndexData &d_indices,
         Geometry geometry = Geometry::Triangles)
        : m_device(device), m_geometry(geometry),
          vertices(create_vertex_buffer(m_device, d_vertices.data(),
                                        d_vertices.size() * sizeof(Vertex<Attrs...>))),
          indices(create_index_buffer(m_device, d_indices.data(),
                                      d_indices.size() * sizeof(unsigned int))),
          mesh(create_mesh(m_device, vertices, indices, layout())), m_size(d_indices.size()) {}

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    void load(const VertexData &vertices);
    void load(const IndexData &indices);

    unsigned int size() { return m_size; }
    unsigned int offset() { return m_offset; }
    Geometry geometry() { return m_geometry; }

    ~Mesh() {}
};

template <typename... Attrs> VertexLayout Mesh<Attrs...>::layout() {
    size_t location = 0;
    auto attributes = {VertexLayout::Attribute{location++, sizeof(Attrs), 0, Attrs::COUNT}...};
    auto stride = (sizeof(Attrs) + ...);
    return VertexLayout{attributes, stride};
}

template <typename... Attrs> void Mesh<Attrs...>::load(const VertexData &vertex_data) {
    ::load(m_device, vertices, (void *)vertex_data.data(),
           vertex_data.size() * sizeof(Vertex<Attrs...>));
}

template <typename... Attrs> void Mesh<Attrs...>::load(const IndexData &index_data) {
    ::load(m_device, indices, (void *)index_data.data(), index_data.size() * sizeof(unsigned int));
    m_size = index_data.size();
}

typedef Mesh<Position, Normal, Tex> MeshPNT;
typedef Mesh<Position, Normal> MeshPN;

#endif /*MESH*/

#ifndef CONTEXT
#define CONTEXT

#include "./camera.h"
#include "./components/mesh.h"
#include "./components/texture.h"
#include "./transform.h"
#include "./vertices.h"
#include "component_manager.h"
#include "noises/simplex_3d.h"
#include <memory>

#include <map>
#include <vector>

template <typename T> struct Component {
    unsigned int id;
    unsigned int node;
    T component;
};

struct PointSwarm {
    Mesh<Position, Normal, Tex> &mesh;
    float threshold = 0.5;
    Texture3D texture;
};

struct DirectionalLight {
    // Mesh<Position> mesh;
    glm::vec3 ambient;
    glm::vec3 diffuse;
};

struct Rock {
    Mesh<Position, Normal> mesh;
    Texture3D image;
};

struct Model {
    typedef GenericComponentManager<Model> Manager;
    std::shared_ptr<Mesh<Position, Normal, Tex>> mesh;
    std::shared_ptr<Texture2D> tex;
};

struct Grid {
    Mesh<Position> mesh;
};

struct Sprite {
    Mesh<Position, Normal, Tex> &mesh;
    Texture2D sprite;
};

struct Sprite3D {
    Mesh<Position, Normal, Tex> &mesh;
    SimplexDefinition noise_definition;
    // Texture3D sprite;
};

struct Skycube {
    Mesh<Position, Normal, Tex> &mesh;
};

template <typename T> struct BaseContext {
    typedef std::pair<unsigned int, T> Entity;
    std::vector<Entity> entities;

  public:
    void base_add(unsigned int node, T &&t) { entities.push_back(std::pair(node, std::move(t))); }

    std::vector<Entity> &base_components() { return entities; }

    T &base_get(unsigned int node) {
        auto it = std::find_if(entities.begin(), entities.end(),
                               [node](auto entity) { return entity.first == node; });
        if (it == entities.end()) {
            throw RuntimeException::error("Invalid component access", node);
        }
        return it->second;
    }
};

template <typename... Comp> struct Context : public BaseContext<Comp>... {
  public:
    template <typename T> void add(unsigned int node, T &&t) {
        BaseContext<T>::base_add(node, std::move(t));
    }

    template <typename T> std::vector<std::pair<unsigned int, T>> &components() {
        return BaseContext<T>::base_components();
    }

    template <typename T> T &get(unsigned int node) { return BaseContext<T>::base_get(node); }
};

typedef Context<int> RenderContext;

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>>
cube(float size = 1.0);
std::pair<std::vector<Vertex<Position>>, std::vector<unsigned int>> grid(float range,
                                                                         float distance);

std::pair<std::vector<Vertex<Position, Normal>>, std::vector<unsigned int>>
cubic_point_swarm(unsigned int resolution, float size);

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>>
quad(float x = 1.0, float y = 1.0);

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>>
sphere_pnt(float size = 1.0, unsigned int resolution = 10);

std::pair<std::vector<Vertex<Position>>, std::vector<unsigned int>>
sphere_p(float size = 1.0, unsigned int resolution = 10);

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>> plane(float x,
                                                                                       float z);
class Simplex3DGenerator;

void asteroid_deformation(std::function<float(glm::vec3)> generator,
                          std::vector<Vertex<Position, Normal, Tex>> &vertices,
                          unsigned int resolution, float strength);

#endif /*CONTEXT*/

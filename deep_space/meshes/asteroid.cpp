#include "../components/mesh.h"
#include "../noises/simplex_3d.h"
#include "../vertices.h"

#include <functional>
#include <vector>

glm::vec3 slerp(glm::vec3 a, glm::vec3 b, float t) {
    auto angle = acos(glm::dot(a, b));
    return (a * sinf(angle * (1 - t)) + b * sinf(angle * t)) / (sinf(angle));
}

glm::vec3 new_normal(glm::vec3 top, glm::vec3 bottom, glm::vec3 left, glm::vec3 right) {
    // return glm::normalize(slerp(slerp(top, bottom, 0.5), slerp(left, right, 0.5), 0.5));
    return glm::normalize(cross(left, top) + cross(top, right) + cross(right, bottom) +
                          cross(bottom, left));
}

void deform(std::function<float(glm::vec3)> &generator,
            std::vector<Vertex<Position, Normal, Tex>> &vertices, float strength) {
    for (auto &vertex : vertices) {
        auto pos = glm::vec3(vertex.x, vertex.y, vertex.z);
        auto norm = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
        pos += norm * generator(pos) * strength;
        vertex.x = pos.x;
        vertex.y = pos.y;
        vertex.z = pos.z;
    }
}

void restore_normals(std::vector<Vertex<Position, Normal, Tex>> &vertices,
                     unsigned int resolution) {
    auto vertex = [&, resolution](int x, int y) -> unsigned int {
        x = (x + resolution) % resolution;
        return y * resolution + x;
    };

    auto to_vec = [&vertices, vertex](int x, int y) -> glm::vec3 {
        auto i = vertex(x, y);
        return glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
    };

    for (int x = 0; x < resolution; x++) {
        for (int y = 1; y < resolution - 1; y++) {
            auto i = vertex(x, y);
            auto center = to_vec(x, y);
            auto v = new_normal(normalize(to_vec(x, y + 1) - center), //
                                normalize(to_vec(x, y - 1) - center), //
                                normalize(to_vec(x - 1, y) - center), //
                                normalize(to_vec(x + 1, y) - center));

            vertices[i].nx = v.x;
            vertices[i].ny = v.y;
            vertices[i].nz = v.z;
        }
    }
}

void asteroid_deformation(std::function<float(glm::vec3)> generator,
                          std::vector<Vertex<Position, Normal, Tex>> &vertices,
                          unsigned int resolution, float strength) {
    deform(generator, vertices, strength);
    restore_normals(vertices, resolution);
}

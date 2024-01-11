#include "../context.h"

#include <ranges>

std::pair<std::vector<Vertex<Position, Normal>>, std::vector<unsigned int>>
cubic_point_swarm(unsigned int resolution, float size) {

    size_t v_size = resolution * resolution * resolution;
    std::vector<Vertex<Position, Normal>> vertices;
    std::vector<unsigned int> indices;

    float scale = size / (float)resolution;

    for (int x = 0, i = 0; x < resolution; x++) {
        for (int y = 0; y < resolution; y++) {
            for (int z = 0; z < resolution; z++, i++) {
                vertices.push_back(
                    Vertex<Position, Normal>(glm::vec3(x, y, z) * scale, glm::vec3(0.0)));
                indices.push_back(i);
            }
        }
    }

    return std::pair(vertices, indices);
}

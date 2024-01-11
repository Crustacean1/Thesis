#define GLM_FORCE_SWIZZLE

#include "../vertices.h"
#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <ranges>
#include <vector>

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>> cube(float size) {
    std::vector<Vertex<Position, Normal, Tex>> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 quad[] = {glm::vec3(-size, -size, size), glm::vec3(-size, size, size),
                        glm::vec3(size, size, size), glm::vec3(size, -size, size)};

    glm::vec3 directions[] = {glm::vec3(0, 1, 0),  glm::vec3(1, 0, 0),  glm::vec3(0, 0, 1),
                              glm::vec3(-1, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, -1, 0),
                              glm::vec3(-1, 0, 0)};

    std::vector<unsigned int> local_indices = {0, 3, 2, 2, 1, 0};

    for (int i = 0; i < 6; i++) {
        auto z = directions[i];
        auto y = directions[i + 1];
        auto x = glm::cross(y, z);
        glm::mat3 transform(x, y, z);

        for (auto vertex : quad) {
            vertices.push_back(Vertex<Position, Normal, Tex>(
                transform * vertex, transform * glm::vec3(0, 0, 1), vertex.xy() * 0.5f + 0.5f));
        }
        auto quad_indices =
            local_indices | std::views::transform([=](auto index) { return index + i * 4; });
        indices.insert(indices.end(), quad_indices.begin(), quad_indices.end());
    }

    return std::pair(vertices, indices);
}

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>>
cube(float size, unsigned int resolution) {
    std::vector<Vertex<Position, Normal, Tex>> vertices;
    std::vector<unsigned int> indices;

    glm::mat3 orientations[6] = {
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f))};

    unsigned int base_ind[] = {0, 1, resolution + 1, resolution + 1, resolution, 0};
    for (int i = 0; i < 6; i++) {
        for (int x = 0; x < resolution; x++) {
            for (int y = 0; y < resolution; y++) {
                vertices.push_back(Vertex<Position, Normal, Tex>(
                    orientations[i] * glm::vec3(-1.f + (((float)(2 * x)) / ((float)resolution - 1)),
                                                -1.f + (((float)(2 * y)) / ((float)resolution - 1)),
                                                1),
                    glm::vec3(0, 0, 0), glm::vec2(0, 0)));
            }
        }
        for (int x = 0; x < resolution - 1; x++) {
            for (int y = 0; y < resolution - 1; y++) {
                auto ind = base_ind | std::ranges::views::transform([=](int j) {
                               return j + i * resolution * resolution + y * resolution + x;
                           });
                indices.insert(indices.end(), ind.begin(), ind.end());
            }
        }
    }
    return std::pair(vertices, indices);
}

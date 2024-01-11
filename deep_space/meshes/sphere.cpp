#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <ranges>
#include <vector>

#include "../vertices.h"

static constexpr float PI = 3.1415926535;

typedef unsigned int uint;

std::vector<uint> sphere_indices(uint resolution) {
    uint index_size = resolution * (resolution - 1) * 6;
    auto indices = new uint[index_size];
    auto current_index = indices;

    for (int y = 0; y < resolution - 1; ++y) {
        for (int x = 0; x < resolution; ++x) {
            auto tl = resolution * (y + 1) + x;
            auto tr = resolution * (y + 1) + ((x + 1) % resolution);
            auto bl = resolution * y + x;
            auto br = resolution * y + ((x + 1) % resolution);

            *(current_index++) = bl;
            *(current_index++) = tl;
            *(current_index++) = tr;

            *(current_index++) = tr;
            *(current_index++) = br;
            *(current_index++) = bl;
        }
    }

    return std::vector<uint>(indices, indices + index_size);
}

glm::vec3 position(int x, int y, float radius, float x_inverse, float y_inverse) {
    float ax = ((float)x) * x_inverse * 2.0 * PI;
    float ay = ((float)y) * y_inverse * PI - 0.5 * PI;
    return glm::vec3(cos(ay) * cos(ax), sin(ay), cos(ay) * sin(ax)) * radius;
}

glm::vec3 normal(int x, int y, float x_inverse, float y_inverse) {
    float ax = ((float)x) * x_inverse * 2.0 * PI;
    float ay = ((float)y) * y_inverse * PI - 0.5 * PI;
    return glm::vec3(cos(ay) * cos(ax), sin(ay), cos(ay) * sin(ax));
}

glm::vec2 texture(int x, int y, float x_inverse, float y_inverse) {
    return glm::vec2(y * y_inverse, x * x_inverse);
}

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>>
sphere_pnt(float radius, unsigned int resolution) {
    typedef Vertex<Position, Normal, Tex> Vertex;

    uint vertex_size = resolution * resolution;

    auto vertices = new Vertex[vertex_size];
    auto current_vertex = vertices;

    float x_inverse = 1.0 / (float)(resolution);
    float y_inverse = 1.0 / (float)(resolution - 1);

    auto deform = glm::mat3(glm::vec3(2, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
    deform = glm::mat3(1);
    auto n_deform = glm::inverse(glm::transpose(deform));
    n_deform = glm::mat3(1);

    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            auto norm = normal(x, y, x_inverse, y_inverse);
            auto pos = deform * norm * radius;
            auto tex = texture(x, y, x_inverse, y_inverse);

            *current_vertex++ = Vertex(pos, n_deform * norm, tex);
        }
    }

    auto indices = sphere_indices(resolution);

    return std::pair(std::vector<Vertex>(vertices, vertices + vertex_size), indices);
}

std::pair<std::vector<Vertex<Position>>, std::vector<unsigned int>>
sphere_p(float radius, unsigned int resolution) {
    typedef Vertex<Position> Vertex;

    uint vertex_size = resolution * resolution;

    auto vertices = new Vertex[vertex_size];
    auto current_vertex = vertices;

    float x_inverse = 1.0 / (float)(resolution);
    float y_inverse = 1.0 / (float)(resolution - 1);

    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            auto norm = normal(x, y, x_inverse, y_inverse);
            auto pos = norm * radius;

            *current_vertex++ = Vertex(pos);
        }
    }

    auto indices = sphere_indices(resolution);

    return std::pair(std::vector<Vertex>(vertices, vertices + vertex_size), indices);
}

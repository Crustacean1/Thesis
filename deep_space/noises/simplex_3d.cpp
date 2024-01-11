#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <iostream>
#include <random>
#include <span>
#include <vector>

#include "../noise.h"
#include "./simplex_3d.h"

typedef glm::vec3 vec3;
typedef glm::mat3 mat3;

void sample3d(std::vector<unsigned char> &data, int width, int height, int depth,
              std::function<float(vec3)> noise) {
    unsigned int row = (((width + 3) >> 2) << 2);

    unsigned int size = row * height * depth * 3;

    float v_min = 10;
    float v_max = -10;

    for (unsigned int z = 0, i = 0; z < depth; ++z) {
        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < width; ++x) {
                i = x + (y + (z * height)) * row;
                auto v = noise(vec3(x, y, z));
                v_max = v_max > v ? v_max : v;
                v_min = v_min < v ? v_min : v;
                data[i * 3] += v;
                data[i * 3 + 1] += v;
                data[i * 3 + 2] += v;
            }
        }
    }
}

float max(float a, float b);

float apply(vec3 vertex, vec3 gradient) {
    return pow(max(0, 0.6 - glm::length2(vertex)), 4) * glm::dot(gradient, vertex);
}

void Simplex3DGenerator::generate_hash_table() {
    std::uniform_int_distribution<unsigned int> dist(0, hash_table_size);

    for (unsigned int i = 0; i < hash_table_size; ++i) {
        hash_table[i] = dist(engine);
    }
}

void Simplex3DGenerator::generate_dirs() {
    for (int i = 0; i < 3 * 3 * 3; ++i) {
        auto x = (i % 3) - 1;
        auto y = ((i / 3) % 3) - 1;
        auto z = ((i / 9) % 3) - 1;
        dirs[i] = glm::normalize(glm::vec3(x, y, z));
    }
    dirs[13] = dirs[26]; // vec(0,0,0) -> 13 %3 = 1, 4 %3 = 1
}

void Simplex3DGenerator::generate_lookup() {
    std::array<glm::vec3, 3> axes = {glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)};
    std::vector<unsigned int> coordinates = {2, 1, 0};

    auto save = [&axes, this](const auto &permutation) {
        auto order = perm(glm::vec3(permutation[0], permutation[1], permutation[2]));
        simplex_paths[order][2 - permutation[0]] = axes[0];
        simplex_paths[order][2 - permutation[1]] = axes[1];
        simplex_paths[order][2 - permutation[2]] = axes[2];
    };

    permute<unsigned int>(coordinates, save);
    for (int i = 0; i < 8; i++) {
        std::cout << i << std::endl;
        for (int j = 0; j < 3; j++) {
            std::cout << "\t" << simplex_paths[i][j].x << " " << simplex_paths[i][j].y << " "
                      << simplex_paths[i][j].z << std::endl;
        }
    }
}

unsigned int Simplex3DGenerator::perm(glm::vec3 vec) {
    unsigned int result = vec.x > vec.y;
    result <<= 1;
    result += vec.y > vec.z;
    result <<= 1;
    result += vec.x > vec.z;
    return result;
};

float Simplex3DGenerator::generate(const glm::vec3 point) {
    const vec3 simplex = skew_matrix * point;
    const vec3 hyper_cube = glm::floor(simplex);
    const vec3 delta = point - unskew_matrix * hyper_cube;

    auto edges = simplex_paths[perm(delta)];

    float result = 0;
    vec3 vertex = hyper_cube;

    // Bounding simplex traversal
    for (auto edge : edges) {
        result += apply(point - (unskew_matrix * vertex), gradient(vertex));
        vertex += edge;
    }

    result += apply(point - (unskew_matrix * vertex), gradient(vertex));

    return result;
}

glm::vec3 Simplex3DGenerator::gradient(glm::vec3 vertex) { return dirs[hash(vertex) % 26]; }

unsigned int Simplex3DGenerator::hash(glm::vec3 vertex) {
    auto x = ((int)vertex.x) & hash_mask;
    auto y = ((int)vertex.y) & hash_mask;
    auto z = ((int)vertex.z) & hash_mask;
    return hash_table[hash_table[hash_table[x] ^ y] ^ z];
}

Simplex3DGenerator::Simplex3DGenerator() {
    generate_dirs();
    generate_lookup();

    std::tie(skew_matrix, unskew_matrix) = simplex_matrix<3>();
}

float Simplex3DGenerator::operator()(glm::vec3 pos) {
    float s = 0.025075;
    return (generate(pos) + s) / (2.0 * s);
}

void Simplex3DGenerator::generate(std::vector<unsigned char> &tex, int width, int height, int depth,
                                  float level, float scale) {
    generate_hash_table();
    float min = 10, max = -10;
    sample3d(tex, width, height, depth, [&min, &max, level, scale, this](vec3 v) {
        float val = generate(v * scale);
        min = val < min ? val : min;
        max = val > max ? val : min;
        return (val + 0.025075) * (level / (2.0 * 0.025075));
    });
}

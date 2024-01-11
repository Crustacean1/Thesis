#include "./worley.h"
#include "../runtime_exception.h"
#include <glm/geometric.hpp>
#include <iostream>

WorleyGenerator::WorleyGenerator() {}

void WorleyGenerator::generate(std::vector<unsigned char> &data, float scale, unsigned int level,
                               unsigned int width, unsigned int height, unsigned int depth) {

    this->width = width;
    this->height = height;
    this->depth = depth;

    float factor = (level / sqrt(3));
    unsigned int row = (((width + 3) >> 2) << 2);
    seed_noise();

    for (int z = 0; z < depth; z++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned int i = (((z * height) + y) * row) + x;
                data[i * 3] += (point(glm::vec3(x, y, z) * scale) * level) / sqrt(3);
                data[i * 3 + 1] = data[i * 3];
                data[i * 3 + 2] = data[i * 3];
            }
        }
    }
}

unsigned int WorleyGenerator::point(int x, int y, int z) {
    return (((z * height) + y) * width + x);
}

void WorleyGenerator::seed_noise() {
    std::uniform_real_distribution<> dist(0, 1.);
    if (noise != nullptr) {
        delete[] noise;
    }
    noise = new glm::vec3[(width + 2) * (height + 2) * (depth + 2)];

    for (int x = 0; x < width + 2; x++) {
        for (int y = 0; y < height + 2; y++) {
            for (int z = 0; z < depth + 2; z++) {
                noise[point(x, y, z)] = glm::vec3(dist(engine), dist(engine), dist(engine));
            }
        }
    }
}

float WorleyGenerator::point(glm::vec3 pos) {
    auto grid = glm::floor(pos);
    int x = grid.x;
    int y = grid.y;
    int z = grid.z;

    float min = 999;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
               float distance = glm::length(
                    pos - (grid + glm::vec3(i, j, k) - noise[point(x + i, y + j, z + k)]));
                min = distance < min ? distance : min;
            }
        }
    }
    return min;
}

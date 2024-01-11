#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/mat2x2.hpp>
#include <glm/vec2.hpp>

#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "../noise.h"

typedef glm::mat2 mat2;

auto [skew_matrix, unskew_matrix] = simplex_matrix<2>();

int grad3[12][3] = {{1, 1, 0},  {-1, 1, 0},  {1, -1, 0}, {-1, -1, 0}, {1, 0, 1},  {-1, 0, 1},
                    {1, 0, -1}, {-1, 0, -1}, {0, 1, 1},  {0, -1, 1},  {0, 1, -1}, {0, -1, -1}};

void seed_hash_table(unsigned char *table, unsigned int size) {
    std::default_random_engine engine;
    std::uniform_int_distribution dist(0, 255);
    auto end = table + size;
    while (table != end) {
        *(table++) = dist(engine);
    }
}

constexpr unsigned int hash_table_size = 256;
unsigned char hash_table[hash_table_size];

void *_ = (seed_hash_table(hash_table, hash_table_size), nullptr);

typedef glm::vec2 vec2;

float simplex_2d_at_point(float x, float y);

float max(float a, float b) { return a > b ? a : b; }

std::vector<unsigned char> noise2d(unsigned int width, unsigned int height,
                                   std::function<float(vec2)> noise) {
    unsigned int row = (((width + 3) >> 2) << 2);
    unsigned int size = row * height * 3;
    unsigned char *data = new unsigned char[size];
    unsigned char *ptr = data;

    for (unsigned int y = 0, i = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            float f_value = noise(vec2(x, y));
            // Experimental values, need to find actual range but good enough for now
            unsigned char value = (f_value + 0.0145) * 4096.0;

            i = x + y * row;
            ptr[i * 3] = value;
            ptr[i * 3 + 1] = value;
            ptr[i * 3 + 2] = value;
        }
    }
    return std::vector(data, data + size);
}

vec2 hash_2d(vec2 pos) {
    auto x = ((int)pos.x) & 255;
    auto y = ((int)pos.y) & 255;
    auto vec = grad3[hash_table[(y + hash_table[x]) & 255] % 12];
    return vec2(vec[0], vec[1]);
}

float apply(vec2 vertex, vec2 gradient) {
    return pow(max(0, 0.5 - glm::length2(vertex)), 4) * glm::dot(gradient, vertex);
}

float simplex2d(vec2 point) {

    point *= 0.03;
    vec2 simplex0 = point * skew_matrix;
    vec2 simplex_origin = glm::floor(simplex0);
    vec2 origin = simplex_origin * unskew_matrix;

    vec2 delta = point - origin;

    vec2 s1 = delta.x > delta.y ? vec2(1, 0) : vec2(0, 1);

    vec2 vertex0 = delta;
    vec2 vertex1 = delta - unskew_matrix * s1;
    vec2 vertex2 = delta - unskew_matrix * vec2(1, 1);

    auto value = apply(vertex0, hash_2d(simplex_origin));
    value += apply(vertex1, hash_2d(simplex_origin + s1));
    value += apply(vertex2, hash_2d(simplex_origin + vec2(1, 1)));

    return value;
}

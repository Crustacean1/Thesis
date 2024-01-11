#include "../components/mesh.h"
#include "../vertices.h"

#include <array>
#include <ranges>

std::pair<std::vector<Vertex<Position>>, std::vector<unsigned int>> grid(float range,
                                                                         float spacing) {
    std::vector<unsigned int> indices;

    unsigned int count = 2 * (range / spacing);

    auto x_lines = std::views::iota(1) | std::views::take(count) |
                   std::views::transform([spacing, range](auto i) -> std::array<glm::vec3, 2> {
                       return {glm::vec3(-range, 0, spacing * i - range),
                               glm::vec3(range, 0, spacing * i - range)};
                   }) |
                   std::views::join |
                   std::views::transform([](auto p) { return Vertex<Position>(p); });

    auto z_lines = std::views::iota(1) | std::views::take(count) |
                   std::views::transform([spacing, range](auto i) -> std::array<glm::vec3, 2> {
                       return {glm::vec3(spacing * i - range, 0, -range),
                               glm::vec3(spacing * i - range, 0, range)};
                   }) |
                   std::views::join |
                   std::views::transform([](auto p) { return Vertex<Position>(p); });

    std::vector<Vertex<Position>> vertices;
    for (auto vertex : x_lines) {
        vertices.push_back(vertex);
    }
    for (auto vertex : z_lines) {
        vertices.push_back(vertex);
    }

    for (int i = 0; i < count * 2; ++i) {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
    }

    return std::pair(vertices, indices);
}

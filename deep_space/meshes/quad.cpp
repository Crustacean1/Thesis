#include <vector>

#include "../vertices.h"

typedef Vertex<Position, Normal, Tex> V;

std::pair<std::vector<V>, std::vector<unsigned int>> quad(float x, float y) {
    std::vector<V> vertices{
        V(glm::vec3(-x, -y, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)),
        V(glm::vec3(-x, y, 0), glm::vec3(0, 0, -1), glm::vec2(0, 1)),
        V(glm::vec3(x, y, 0), glm::vec3(0, 0, -1), glm::vec2(1, 1)),
        V(glm::vec3(x, -y, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0)),
    };

    std::vector<unsigned int> indices{0, 1, 2, 2, 3, 0};

    return std::pair(vertices, indices);
}

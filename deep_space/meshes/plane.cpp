#include "../vertices.h"

#include <vector>

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>> plane(float x,
                                                                                       float y) {
    typedef Vertex<Position, Normal, Tex> Vertex;

    std::vector<Vertex> vertices{
        Vertex{glm::vec3(-x,  -y,-1) * 0.5f, glm::vec3(0, 1, 0), glm::vec2(0, 0)},
        Vertex{glm::vec3(x,  -y,-1) * 0.5f, glm::vec3(0, 1, 0), glm::vec2(1, 0)},
        Vertex{glm::vec3(x,  y,-1) * 0.5f, glm::vec3(0, 1, 0), glm::vec2(1, 1)},
        Vertex{glm::vec3(-x,  y,-1) * 0.5f, glm::vec3(0, 1, 0), glm::vec2(0, 1)},
    };

    std::vector<unsigned int> indices{0, 1, 2, 0, 3, 2};

    return std::pair(vertices, indices);
}

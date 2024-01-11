#ifndef MARCHING_CUBE
#define MARCHING_CUBE

#include <functional>
#include <glm/fwd.hpp>
#include <utility>
#include <vector>

#include "../entities/steroid.h"
#include "../vertices.h"

struct VoxelizationParams {
    glm::vec3 center;
    glm::vec3 size;
    unsigned int resolution;
    float threshold;
};

class ComputeProgram;

void marching_cubes(std::vector<glm::vec3> &vertices, std::vector<unsigned int> &indices,
                    glm::vec4 position, unsigned int level, std::function<float(glm::vec3)> value,
                    std::function<glm::vec3(glm::vec3)> derivative);

void marching_cubes_gpu(GL::GraphicalDevice &device, std::shared_ptr<ComputeProgram> program,
                        std::vector<Vertex<Position, Normal>> &vertices,
                        std::vector<unsigned int> &indices, float size, unsigned int resolution,
                        AsteroidVolume::Material &asteroid);

#endif /*MARCHING_CUBE*/

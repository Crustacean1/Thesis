#ifndef WORLEY_GENERATOR
#define WORLEY_GENERATOR

#include <random>
#include <vector>

#include <glm/vec3.hpp>

class WorleyGenerator {
    std::default_random_engine engine;

    unsigned int width;
    unsigned int height;
    unsigned int depth;

    glm::vec3 *noise = nullptr;

    float point(glm::vec3);
    unsigned int point(int x, int y, int z);

    void seed_noise();

  public:
    WorleyGenerator();

    void generate(std::vector<unsigned char> &data, float scale, unsigned int level,
                  unsigned int width, unsigned int height, unsigned int depth);
};

#endif /*WORLEY_GENERATOR*/

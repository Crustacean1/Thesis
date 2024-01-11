#ifndef SIMPLEX_3D
#define SIMPLEX_3D

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <random>
#include <vector>

struct SimplexDefinition {
    glm::mat3 skew_matrix;
    glm::mat3 unskew_matrix;

	int hash_table[512];
    glm::vec3 dirs[26];
    glm::vec3 simplex_paths[24];
};

SimplexDefinition create_noise_definition();

class Simplex3DGenerator {
    std::default_random_engine engine;

    static constexpr unsigned int hash_table_size = 1024;
    static constexpr unsigned int hash_mask = hash_table_size - 1;

    glm::mat3 skew_matrix;
    glm::mat3 unskew_matrix;
    glm::vec3 dirs[26];
    unsigned char hash_table[hash_table_size];
    std::array<glm::vec3, 3> simplex_paths[8];

    void generate_dirs();
    void generate_lookup();
    void generate_hash_table();

    static unsigned int perm(glm::vec3);
    std::array<glm::vec3, 4> simplex_path(glm::vec3 pos);
    unsigned int hash(glm::vec3);
    glm::vec3 gradient(glm::vec3);

    float generate(glm::vec3);

  public:
    Simplex3DGenerator();
    float operator()(glm::vec3 pos);
    void generate(std::vector<unsigned char> &data, int width, int height, int depth, float level,
                  float scale);
};

#endif /*SIMPLEX_3D*/

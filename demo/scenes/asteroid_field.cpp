#include "components/compute_program.h"
#define GLM_FORCE_SWIZZLE

#include "context.h"
#include "entities/asteroid.h"
#include "noises/marching_cube.h"
#include "opengl/graphical_device_gl.h"
#include "scene_manager.h"
#include <memory>
#include <ranges>

namespace v = std::ranges::views;

void generate_asteroid_outline(AsteroidVolume::Material &material,
                               std::default_random_engine &engine) {
    material.ball_count = 0;
    glm::vec3 start = glm::vec3(0);
    float delta_radius = 0.3;
    std::uniform_real_distribution<float> dist(-delta_radius, delta_radius);

    for (int i = 0; i < 5; i++) {
        material.balls[i] =
            AsteroidVolume::Blob{.position = glm::vec4(start, 0), .intensity = 1, .radius = 1};
        glm::vec3 direction = glm::vec3(dist(engine), dist(engine), sqrt(1 - pow(delta_radius, 2)));
    }

    unsigned int rough_count = 5;
    float cone = 0.3;
    std::uniform_real_distribution<float> circumference(-3.1415, 3.1415);
    std::uniform_real_distribution<float> unit(-1, 1);

    glm::vec3 position(0);
    glm::vec3 old_dir = glm::vec3(0, 0, 1);
    glm::quat orientation = glm::angleAxis(0.f, glm::vec3(0, 1, 0));

    for (int i = 0; i < rough_count; i++) {
        glm::vec3 direction =
            orientation *
            glm::normalize(glm::vec3(0, 0, cone) + glm::vec3(unit(engine), unit(engine), 0.f));
        position += direction;
        orientation = glm::rotation(direction, old_dir);
        old_dir = direction * (0.001f + 0.002f * unit(engine));

        material.balls[material.ball_count++] = AsteroidVolume::Blob{glm::vec4(position, 0), 1, 1};
    }

    unsigned int fine_count = 15;
    std::uniform_int_distribution<int> sphere_dist(0, rough_count);
    std::uniform_real_distribution<float> weight(0.9, 2);
    std::uniform_int_distribution<int> sign(0, 1);

    for (int i = 0; i < fine_count; i++) {
        glm::vec3 dir = 1.5f * glm::normalize(glm::vec3(unit(engine), unit(engine), unit(engine)));
        float impact = weight(engine);
        material.balls[material.ball_count++] = AsteroidVolume::Blob{
            glm::vec4(material.balls[sphere_dist(engine)].position.xyz() + dir, 0),
            impact * (-0.6f), 2 * impact};
    }
}

void regenerate_asteroid(GL::GraphicalDevice &device, std::shared_ptr<ComputeProgram> program,
                         std::shared_ptr<MeshPN> mesh, AsteroidVolume::Material &material) {
    std::vector<Vertex<Position, Normal>> vertices;
    std::vector<unsigned int> indices;

    marching_cubes_gpu(device, program, vertices, indices, 5, 128, material);
    mesh->load(indices);
    mesh->load(vertices);
}

void generate_new_asteroid(GL::GraphicalDevice &device, std::default_random_engine &engine,
                           std::shared_ptr<ComputeProgram> program, std::shared_ptr<MeshPN> mesh,
                           AsteroidVolume::Material &mat) {
    generate_asteroid_outline(mat, engine);
    regenerate_asteroid(device, program, mesh, mat);
}

std::vector<NodeHandle> create_asteroid_field(GL::GraphicalDevice &device,
                                                std::shared_ptr<ComputeProgram> program,
                                                ISceneManager &manager) {
    std::default_random_engine engine;

    std::uniform_real_distribution<float> disp(-5, 5);
    std::vector<NodeHandle> nodes;

    AsteroidVolume::Material mat;
    mat.noise.scale = 0.5;
    mat.noise.fractal = 2;
    mat.noise.octaves = 5;
    mat.threshold = 0.5;
    mat.noise.brightness = 0.01;
    mat.noise.base_brightness = 0.01;
    generate_asteroid_outline(mat, engine);

    std::vector<Vertex<Position, Normal>> vertices;
    std::vector<unsigned int> indices;
    auto mesh = std::make_shared<MeshPN>(device, vertices, indices);

    generate_new_asteroid(device, engine, program, mesh, mat);
    auto steroid =
        manager.node(Transform(10, -10, 0)).component(Asteroid{.mesh = mesh, .material = mat});
    nodes.push_back(steroid);

    return nodes;
}

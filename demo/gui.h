#ifndef SPACE_GUI
#define SPACE_GUI

#include "components/compute_program.h"
#include "entities/asteroid.h"
#include "entities/black_hole.h"
#include "entities/planet.h"
#include "entities/spacebox.h"
#include "entities/star.h"
#include "entities/starpoint.h"
#include "entities/steroid.h"
#include "imgui.h"
#include "node/node_context.h"
#include "scene_manager.h"
#include <random>

extern std::default_random_engine asteroid_engine;

void generate_asteroid_outline(AsteroidVolume::Material &material,
                               std::default_random_engine &engine);

void generate_new_asteroid(GL::GraphicalDevice &device, std::default_random_engine &engine,
                           std::shared_ptr<ComputeProgram> program, std::shared_ptr<MeshPN> mesh,
                           AsteroidVolume::Material &material);

std::vector<NodeHandle> create_asteroid_field(GL::GraphicalDevice &device,
                                              std::shared_ptr<ComputeProgram> program,
                                              ISceneManager &manager);

template <typename M> static void planet_gui(M &manager, NodeHandle it) {
    Planet &planet = manager.template component<Planet>(it);
    char names[64];
    for (int i = 0; i < 4; i++) {
        sprintf(names, "Color %d", i + 1);
        ImGui::ColorEdit3(names, (float *)(planet.uniform.colors + i));
    }

    planet.uniform.light.position = glm::normalize(planet.uniform.light.position);
    ImGui::SliderFloat3("Light direction", (float *)&planet.uniform.light.position, -1, 1);
    ImGui::SliderFloat3("Atmosphere color", (float *)&planet.uniform.scattering, 0, 5);

    ImGui::SliderFloat("Falloff", &planet.uniform.density_falloff, 0, 100);
    ImGui::SliderFloat("Radius", &planet.uniform.planet_radius, 0.1, 1);
    ImGui::SliderInt("Octaves", &planet.uniform.octaves, 1, 20);
    ImGui::SliderFloat("Intensity", &planet.uniform.intensity, 0, 10);
    ImGui::SliderFloat("Offset", &planet.uniform.offset, 0, 10);
    ImGui::SliderFloat("Scale", &planet.uniform.scale, 0, 10);
    ImGui::SliderFloat("Fractal", &planet.uniform.increment, 0, 5);
    ImGui::SliderFloat("Continent Fractal", &planet.uniform.surface_increment, 0, 5);
    ImGui::SliderFloat("Surface base", &planet.uniform.surface_base, 0, 1);
    ImGui::SliderFloat("Surface factor", &planet.uniform.surface_factor, 0, 1);
}

template <typename M> void blackhole_gui(M &manager, NodeHandle it) {
    BlackHole &blackhole = manager.template component<BlackHole>(it);
    ImGui::SliderInt("Iterations", &blackhole.black_hole_properties.iterations, 0, 4096 * 4);
    ImGui::SliderFloat("Step", &blackhole.black_hole_properties.step, 0, 1);
    ImGui::SliderFloat("Radius", &blackhole.black_hole_properties.radius, 0, 25);
    ImGui::SliderFloat("Strength", &blackhole.black_hole_properties.strength, 0, 512);
}

template <typename M> void starpoint_gui(M &manager, NodeHandle star_node) {
    StarPoint &star_point = manager.template component<StarPoint>(star_node);
    if (ImGui::Button("Regenerate")) {
        star_point.enabled = true;
    }
}

template <typename M> void star_gui(M &manager, NodeHandle it) {
    Star::StarMaterial &material = manager.template component<Star>(it).material;

    char buffer[32];
    for (int i = 0; i < 4; i++) {
        sprintf(buffer, "Color %d", i);
        ImGui::ColorEdit3(buffer, (float *)(material.colors + i));
    }

    ImGui::SliderFloat("Scale", &material.noise.scale, 0, 10);
    ImGui::SliderFloat("Fractal", &material.noise.fractal, 0.1, 3);
    ImGui::SliderInt("Octaves", &material.noise.octaves, 0, 25);
    ImGui::SliderFloat("Base Brightness", &material.noise.base_brightness, 0, 3);
    ImGui::SliderFloat("Brightness", &material.noise.brightness, 0, 3);
    ImGui::SliderFloat("Radius", &material.radius, 0, 1);
    ImGui::SliderFloat("Speed", &material.speed, 0, 1);
}

template <typename M> void steroid_gui(M &manager, NodeHandle steroid_node) {
    AsteroidVolume &steroid = manager.template component<AsteroidVolume>(steroid_node);
    ImGui::SliderInt("Iterations", &steroid.material.iterations, 0, 256);
    ImGui::SliderFloat("Threshold", &steroid.material.threshold, 0, 1);
    ImGui::SliderFloat("Scale", &steroid.material.noise.scale, 0, 10);
    ImGui::SliderFloat("Fractal", &steroid.material.noise.fractal, 0.1, 3);
    ImGui::SliderInt("Octaves", &steroid.material.noise.octaves, 0, 25);
    ImGui::SliderFloat("Base Brightness", &steroid.material.noise.base_brightness, 0, 0.1);
    ImGui::SliderFloat("Brightness", &steroid.material.noise.brightness, 0, 0.1);
    if (ImGui::Button("Regenerate")) {

        generate_asteroid_outline(steroid.material, asteroid_engine);
    }
}

template <typename M>
void asteroid_gui(M &manager, GL::GraphicalDevice &device,
                  std::shared_ptr<ComputeProgram> program, NodeHandle asteroid_node) {
    Asteroid &asteroid = manager.template component<Asteroid>(asteroid_node);
    ImGui::SliderInt("Iterations", &asteroid.material.iterations, 0, 256);
    ImGui::SliderFloat("Threshold", &asteroid.material.threshold, 0, 1);
    ImGui::SliderFloat("Scale", &asteroid.material.noise.scale, 0, 10);
    ImGui::SliderFloat("Fractal", &asteroid.material.noise.fractal, 0.1, 3);
    ImGui::SliderInt("Octaves", &asteroid.material.noise.octaves, 0, 25);
    ImGui::SliderFloat("Base Brightness", &asteroid.material.noise.base_brightness, 0, 0.1);
    ImGui::SliderFloat("Brightness", &asteroid.material.noise.brightness, 0, 0.1);
    ImGui::Checkbox("Wiremesh", &asteroid.wiremesh);
    ImGui::Checkbox("Normals", &asteroid.normal);
    if (ImGui::Button("Regenerate")) {
        generate_new_asteroid(device, asteroid_engine, program, asteroid.mesh, asteroid.material);
    }
}

template <typename M> void spacebox_gui(M &manager, NodeHandle it) {
    SpaceBox &box = manager.template component<SpaceBox>(it);
    SpaceBox::SimplexNoise &noise = box.noise;
    SpaceBox::Stars &stars = box.stars;

    ImGui::SliderInt("Iterations", &noise.iterations, 0, 1024);
    ImGui::SliderInt("Octaves", &noise.octaves, 0, 32);
    ImGui::SliderFloat("Density", &noise.density, 0.5, 10);
    ImGui::SliderFloat("Scattering", &noise.scatt, 0, 1);
    ImGui::SliderFloat("Scale", &noise.scale, 0.01, 10);
    ImGui::SliderFloat("Fractal", &noise.fractal, 0.01, 4);
    ImGui::SliderFloat("Base", &noise.base, -1, 1);
    ImGui::SliderFloat("Brightness", &noise.brightness, 0, 3);
    ImGui::SliderFloat("Radius", &noise.radius, 0, 1.5);
    ImGui::SliderFloat3("Color", ((float *)&noise.cloud_color), 0, 25);

    ImGui::SliderInt("Stars", &stars.star_count, 0, 16);
    char labels[64];
    for (int i = 0; i < stars.star_count; i++) {
        sprintf(labels, "Star position %d", i + 1);
        ImGui::SliderFloat3(labels, (float *)(stars.positions + i), -5, 5);
        sprintf(labels, "Star color %d", i + 1);
        ImGui::ColorEdit4(labels, (float *)(stars.colors + i));
    }
    if (ImGui::Button("Refresh")) {
        box.current_fragment = 0;
        box.stars_rendered = false;
    }
}

#endif /*SPACE_GUI*/


#include <glad/glad.h>

#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ranges>

#include "camera.h"
#include "command_queue.h"
#include "components/compute_program.h"
#include "components/program.h"
#include "context.h"
#include "controllers/camera_controller.h"
#include "entities/asteroid.h"
#include "entities/black_hole.h"
#include "entities/hdr_source.h"
#include "entities/planet.h"
#include "entities/space_background.h"
#include "entities/spacebox.h"
#include "entities/spinner.h"
#include "entities/star.h"
#include "entities/starpoint.h"
#include "entities/steroid.h"
#include "graphical_device.h"
#include "gui.h"
#include "imgui.h"
#include "opengl/graphical_device_gl.h"
#include "pipeline.h"
#include "renderers/renderers.h"
#include "scene_manager.h"
#include "vertices.h"
#include "window.h"

std::default_random_engine asteroid_engine;

std::shared_ptr<Mesh<Position, Color4>> create_star_map(GL::GraphicalDevice &device) {
    std::vector<Vertex<Position, Color4>> vertices;
    std::vector<unsigned int> indices;
    std::uniform_real_distribution<float> dist(-1, 1);
    std::uniform_real_distribution<float> color(0, 0.5);
    std::uniform_real_distribution<float> size(0.3, 1);

    for (int i = 0; i < 50000; i++) {
        auto col = color(asteroid_engine);
        vertices.push_back(Vertex<Position, Color4>(
            glm::normalize(
                glm::vec3(dist(asteroid_engine), dist(asteroid_engine), dist(asteroid_engine))),
            glm::vec4(0.7 - col, 0.4, 0.75 + col, size(asteroid_engine))));
        indices.push_back(i);
    }
    return std::make_shared<Mesh<Position, Color4>>(device, vertices, indices, Geometry::Points);
}

std::pair<std::vector<Vertex<Position, Normal, Tex>>, std::vector<unsigned int>>
cube(float size, unsigned int resolution);

int main(int argc, char **argv) {
    auto window = Window::init(1200, 800, "DeepSpace");

    GL::GraphicalDevice device;
    BaseCommandQueue queue;
    GraphicalContext context(queue);

    // Creating resources
    auto hdr_plane = std::make_shared<MeshPNT>(device, plane(2, 2));
    auto mesh = std::make_shared<MeshPNT>(device, cube(1));
    auto black_hole_mesh = std::make_shared<MeshPNT>(device, cube(2));
    auto partitioned_mesh = std::make_shared<MeshPNT>(device, cube(1, 6));
    auto sphere = std::make_shared<MeshPNT>(device, sphere_pnt(1, 100));
    auto mesh5 = std::make_shared<MeshPNT>(device, cube(5));

    auto cubemap_texture = std::make_shared<CubemapTexture>(device, 2048, 2048);
    auto blackhole_texture = std::make_shared<CubemapTexture>(device, 512, 512);
    auto hdr_texture = std::make_shared<Texture2DFloat>(device, 1200, 800);
    auto marching_cubes_shader = std::make_shared<ComputeProgram>(device, "marching_cubes");

    // Creating components definition
    SceneManagerBuilder scene_builder;
    auto scene_manager = scene_builder.register_component<Model>()
                             .register_component<HdrDisplay>()
                             .register_component<Asteroid>()
                             .register_component<Camera>()
                             .register_component<Planet>()
                             .register_component<SpaceBox>()
                             .register_component<BlackHole>()
                             .register_component<SpaceBackgroundEntity>()
                             .register_component<Star>()
                             .register_component<StarPoint>()
                             .register_component<AsteroidVolume>()
                             .register_component<Spinner>()
                             .build();

    // Creating scene objects

    auto point_stars = create_star_map(device);
    auto asteroid_node = create_asteroid_field(device, marching_cubes_shader, scene_manager)[0];

    NodeHandle background = scene_manager.node(Transform(0, 0, 5))
                                .component(SpaceBox{
                                    5 * 5 * 6,
                                    point_stars,
                                    partitioned_mesh,
                                });
    NodeHandle planet_node = scene_manager.node(Transform(2, 3, 0))
                                 .component(Planet{mesh, Planet::Surface{
                                                             .density_falloff = 50,
                                                             .increment = 1.4,
                                                             .surface_increment = 1.9,
                                                         }});

    NodeHandle rotating_node = scene_manager.node(Transform(40, 0, 0))
                                   .component(Spinner{glm::vec3(1, 0, 0), 1.75});

    NodeHandle blackhole_node_1 = scene_manager.node(Transform(0, 5, 0), &rotating_node)
                                      .component(BlackHole{black_hole_mesh, cubemap_texture});
    NodeHandle blackhole_node_2 = scene_manager.node(Transform(0, -5, 0), &rotating_node)
                                      .component(BlackHole{black_hole_mesh, cubemap_texture});

    NodeHandle hdr_node =
        scene_manager.node(Transform(0, 0, 0)).component(HdrDisplay{hdr_plane, hdr_texture});

    scene_manager.node(Transform(0, 0, 0)).component(SpaceBackgroundEntity{mesh, cubemap_texture});
    NodeHandle star_node =
        scene_manager.node(Transform(0, -2, 0))
            .component(
                Star{.mesh = mesh,
                     .material = Star::StarMaterial{.noise = SimplexNoise{.octaves = 3,
                                                                          .scale = 2.5f,
                                                                          .fractal = 2,
                                                                          .base_brightness = 0.6},
                                                    .radius = 0.2f,
                                                    .speed = 0.15}});

    // Adding GUI controlls for each component

    window->register_gui("Nebula", [&]() { spacebox_gui(scene_manager, background); });
    window->register_gui("Planet", [&]() { planet_gui(scene_manager, planet_node); });

    window->register_gui("Star", [&]() { star_gui(scene_manager, star_node); });
    window->register_gui("Asteroid", [&]() {
        asteroid_gui(scene_manager, device, marching_cubes_shader, asteroid_node);
    });

    CameraController controller(scene_manager, scene_manager.get_manager<Camera>(), device,
                                hdr_texture, *window);

    auto normal_renderer = ParitalRenderingPipelineBuilder()
                               .add<SteroidRenderer, AsteroidVolume>("asteroid_helper")
                               .add<MeshRenderer, Model>("flat")
                               .add<NormalRenderer, Asteroid>("normal")
                               .add<AsteroidRenderer, Asteroid>("asteroid")
                               .add<WiremeshRenderer, Asteroid, AsteroidVolume>("wiremesh")
                               .add<StarRenderer, Star>("star")
                               .add<BlackholeRenderer, BlackHole>("blackhole")
                               .add<PlanetRenderer, Planet>("planet")
                               .add<BackgroundRenderer, SpaceBackgroundEntity>("cubemap")
                               .build(device, context, scene_manager);

    auto cubemap_source_renderer = ParitalRenderingPipelineBuilder()
                                       .add<StarpointRenderer, SpaceBox>("starpoint")
                                       .add<SpaceBoxRenderer, SpaceBox>("skycube")
                                       .build(device, context, scene_manager);

    auto bloom_renderer =
        ParitalRenderingPipelineBuilder().add<HdrRenderer, HdrDisplay>("hdr").build(device, context,
                                                                                    scene_manager);

    auto [camera_node, main_camera] = controller.get_camera_node();
    main_camera->pipeline = normal_renderer;

    auto back_camera =
        scene_manager.node(Transform(0, 0, 0))
            .component(Camera(device, PerspFrustrum{2048, 2048, 0.1, 10}, cubemap_texture));

    scene_manager.component<Camera>(back_camera).layer = 0;
    scene_manager.component<Camera>(back_camera).clear = false;
    scene_manager.component<Camera>(back_camera).pipeline = cubemap_source_renderer;

    auto hdr_camera =
        scene_manager.node(Transform(0, 0, 0)).component(Camera{device, OrthFrustrum{1200, 800}});

    scene_manager.component<Camera>(hdr_camera).pipeline = bloom_renderer;
    scene_manager.component<Camera>(hdr_camera).layer = 1;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(),
                                                   t2 = t1;

    window->run([&]() {
        std::chrono::duration<double, std::milli> delta = t1 - t2;

        scene_manager.render(device, queue);
        scene_manager.update();

        t2 = t1;
        t1 = std::chrono::high_resolution_clock::now();
    });
}

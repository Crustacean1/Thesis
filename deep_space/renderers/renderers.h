#ifndef RENDERERS
#define RENDERERS

#include "../graphical_device.h"

#include "../components/mesh.h"
#include "../entities/asteroid.h"
#include "../entities/black_hole.h"
#include "../entities/hdr_source.h"
#include "../entities/planet.h"
#include "../entities/space_background.h"
#include "../entities/spacebox.h"
#include "../entities/star.h"
#include "../entities/starpoint.h"
#include "../entities/steroid.h"
#include "../graphical_context.h"
#include "../node/node_context.h"
#include "context.h"
#include <glm/gtc/type_ptr.hpp>

class NormalRenderer {
    ProgramContext m_context;

    void render(std::shared_ptr<MeshPN> mesh, const float *transform) {
        Commands::Draw cmd{
            .uniforms = 1,
            .uniform_bindings = {{.data = transform, .size = sizeof(glm::mat4)}},
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }

    void render(std::shared_ptr<MeshPNT> mesh, const float *transform) {
        Commands::Draw cmd{
            .uniforms = 1,
            .uniform_bindings = {{.data = transform, .size = sizeof(glm::mat4)}},
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }

  public:
    NormalRenderer(ProgramContext context) : m_context(context) {
        context.set_depth_test(true);
        context.set_depth_write(false);
    }

    void render(const std::pair<Node *, Asteroid *> &pair) {
        if (pair.second->normal) {
            render(pair.second->mesh, glm::value_ptr(pair.first->transform.model()));
        }
    }
};

class WiremeshRenderer {
    ProgramContext m_context;

    void render(std::shared_ptr<MeshPN> mesh, const float *transform) {
        Commands::Draw cmd{
            .uniforms = 1,
            .uniform_bindings = {{.data = transform, .size = sizeof(glm::mat4)}},
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }

    void render(std::shared_ptr<MeshPNT> mesh, const float *transform) {
        Commands::Draw cmd{
            .uniforms = 1,
            .uniform_bindings = {{.data = transform, .size = sizeof(glm::mat4)}},
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }

  public:
    WiremeshRenderer(ProgramContext context) : m_context(context) {
        context.set_depth_test(true);
        context.set_depth_write(false);
    }

    void render(const std::pair<Node *, Asteroid *> &pair) {
        if (pair.second->wiremesh) {
            render(pair.second->mesh, glm::value_ptr(pair.first->transform.model()));
        }
    }
    void render(const std::pair<Node *, AsteroidVolume *> &pair) {
        render(pair.second->mesh, glm::value_ptr(pair.first->transform.model()));
    }
    void render(const std::pair<Node *, BlackHole *> &pair) {
        render(pair.second->cube_mesh, glm::value_ptr(pair.first->transform.model()));
    }
};

class SteroidRenderer {
    ProgramContext m_context;

  public:
    SteroidRenderer(ProgramContext context) : m_context(context) {
        m_context.set_depth_test(true);
        m_context.set_depth_write(true);
        m_context.set_blending(AlphaBlending::Dst);
    }

    void render(const std::pair<Node *, AsteroidVolume *> &pair) {
        auto [node, steroid] = pair;
        Commands::Draw draw{
            .uniforms = 2,
            .uniform_bindings = {{.data = glm::value_ptr(node->transform.model()),
                                  .size = sizeof(glm::mat4)},
                                 {.data = &steroid->material,
                                  .size = sizeof(AsteroidVolume::Material)}},
            .geometry = steroid->mesh->geometry(),
            .vao = steroid->mesh->mesh,
            .offset = steroid->mesh->offset(),
            .size = steroid->mesh->size(),
        };
        m_context.draw(draw);
    }
};

class StarRenderer {
    ProgramContext m_context;

  public:
    StarRenderer(ProgramContext context) : m_context(context) {
        m_context.set_depth_test(true);
        m_context.set_depth_write(false);
        m_context.set_blending(AlphaBlending::Dst);
    }

    void render(const std::pair<Node *, Star *> &pair) {
        auto [node, star] = pair;
        Commands::Draw draw{
            .uniforms = 2,
            .uniform_bindings = {{.data = glm::value_ptr(node->transform.model()),
                                  .size = sizeof(glm::mat4)},
                                 {.data = &star->material, .size = sizeof(Star::StarMaterial)}},
            .geometry = star->mesh->geometry(),
            .vao = star->mesh->mesh,
            .offset = star->mesh->offset(),
            .size = star->mesh->size(),
        };
        m_context.draw(draw);
    }
};

class HdrRenderer {
    ProgramContext m_context;

  public:
    HdrRenderer(ProgramContext context) : m_context(context) {}

    void render(const std::pair<Node *, HdrDisplay *> &pair) {
        auto [node, hdr] = pair;
        Commands::Draw cmd{
            .textures = 1,
            .texture_binding = {Commands::TextureBinding{.texture = {hdr->texture->handle},
                                                         .binding_type =
                                                             Commands::TextureBinding::Texture2D}},
            .geometry = hdr->plane->geometry(),
            .vao = hdr->plane->mesh,
            .offset = hdr->plane->offset(),
            .size = hdr->plane->size(),
        };
        m_context.draw(cmd);
    }
};

class BlackholeRenderer {
    ProgramContext m_context;

  public:
    BlackholeRenderer(ProgramContext context) : m_context(context) {
        m_context.set_depth_test(false);
        m_context.set_depth_write(false);
        m_context.set_blending(AlphaBlending::Dst);
    }

    void render(const std::pair<Node *, BlackHole *> &pair) {
        auto [node, black_hole] = pair;

        Commands::Draw cmd{
            .textures = 1,
            .texture_binding = {Commands::TextureBinding{
                .texture = {black_hole->surrounding->handle},
                .binding_type = Commands::TextureBinding::Cubemap}},
            .uniforms = 2,
            .uniform_bindings =
                {
                    {.data = glm::value_ptr(node->global_transform.model()),
                     .size = sizeof(glm::mat4)},
                    {.data = &black_hole->black_hole_properties,
                     .size = sizeof(BlackHole::BlackHoleProperties)},
                },
            .geometry = black_hole->cube_mesh->geometry(),
            .vao = black_hole->cube_mesh->mesh,
            .offset = black_hole->cube_mesh->offset(),
            .size = black_hole->cube_mesh->size(),
        };
        m_context.draw(cmd);
    }
};

class MeshRenderer {
    ProgramContext m_context;
    unsigned int m_u_model;

  public:
    MeshRenderer(ProgramContext context) : m_context(context) {
        context.set_depth_write(true);
        context.set_depth_test(true);
    }

    void render(const std::pair<Node *, Model *> &pair) {
        auto &node = *pair.first;
        auto &model = *pair.second;
        Commands::Draw cmd = {
            .uniforms = 1,
            .uniform_bindings = {{.data = glm::value_ptr(node.transform.model()),
                                  .size = sizeof(glm::mat4)}},
            .geometry = model.mesh->geometry(),
            .vao = model.mesh->mesh,
            .offset = model.mesh->offset(),
            .size = model.mesh->size(),
        };
        m_context.draw(cmd);
    }
};

class BackgroundRenderer {
    ProgramContext m_context;

  public:
    BackgroundRenderer(ProgramContext context) : m_context(context) {
        m_context.set_depth_test(true);
        m_context.set_depth_write(false);
    }

    void render(const std::pair<Node *, SpaceBackgroundEntity *> &pair) {
        auto [node, background] = pair;
        auto mesh = background->cube_mesh;

        Commands::Draw cmd = {
            .textures = 1,
            .texture_binding = {Commands::TextureBinding{.texture = {background->texture->handle},
                                                         .binding_type =
                                                             Commands::TextureBinding::Cubemap}},
            .uniforms = 1,
            .uniform_bindings = {{
                .data = glm::value_ptr(node->transform.model()),
                .size = sizeof(glm::mat4),
            }},
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }
};

class StarpointRenderer {
    ProgramContext m_context;

  public:
    StarpointRenderer(ProgramContext &context) : m_context(context) {
        m_context.set_depth_test(false);
        m_context.set_depth_write(false);
        m_context.set_blending(AlphaBlending::Addative);
    }

    void render(const std::pair<Node *, SpaceBox *> &pair) {
        auto [node, point] = pair;
        auto mesh = point->stars_mesh;
        if (point->fragments == point->current_fragment && point->stars_rendered == false) {
            Commands::Draw cmd{
                .geometry = mesh->geometry(),
                .vao = mesh->mesh,
                .offset = mesh->offset(),
                .size = mesh->size(),
            };
            m_context.draw(cmd);
			point->stars_rendered = true;
        }
    }
};

class SpaceBoxRenderer {
    ProgramContext m_context;

  public:
    SpaceBoxRenderer(ProgramContext context) : m_context(context) {}
    void render(const std::pair<Node *, SpaceBox *> &pair) {
        auto [node, model] = pair;
        if (model->current_fragment < model->fragments) {
            Commands::Draw cmd = {
                .uniforms = 2,
                .uniform_bindings = {{
                                         .data = &model->noise,
                                         .size = sizeof(SpaceBox::SimplexNoise),
                                     },
                                     {.data = &model->stars, .size = sizeof(SpaceBox::Stars)}},
                .geometry = model->mesh->geometry(),
                .vao = model->mesh->mesh,
                .offset = model->mesh->offset(),
                .size = model->mesh->size(),
            };

            cmd.offset = 6 * model->current_fragment++ * sizeof(unsigned int);
            cmd.size = 6;
            m_context.draw(cmd);
        }
    }
};

class PlanetRenderer {
    ProgramContext m_context;

  public:
    PlanetRenderer(ProgramContext context) : m_context(context) {
        m_context.set_depth_test(true);
        m_context.set_depth_write(false);
        m_context.set_blending(AlphaBlending::Dst);
    }

    void render(const std::pair<Node *, Planet *> &pair) {
        auto [node, planet] = pair;
        auto mesh = planet->box;
        Commands::Draw cmd{
            .uniforms = 2,
            .uniform_bindings = {{.data = glm::value_ptr(node->transform.model()),
                                  .size = sizeof(glm::mat4)},
                                 {.data = &planet->uniform, .size = sizeof(Planet::Surface)}},
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }
};

class AsteroidRenderer {
    ProgramContext m_context;

  public:
    AsteroidRenderer(ProgramContext context) : m_context(context) {
        m_context.set_depth_test(true);
        m_context.set_depth_write(true);
        m_context.set_blending(AlphaBlending::None);
    }

    void render(const std::pair<Node *, Asteroid *> &pair) {
        auto [node, asteroid] = pair;
        auto mesh = asteroid->mesh;
        Commands::Draw cmd{
            .uniforms = 1,
            .uniform_bindings =
                {
                    {.data = glm::value_ptr(node->transform.model()), .size = sizeof(glm::mat4)},
                },
            .geometry = mesh->geometry(),
            .vao = mesh->mesh,
            .offset = mesh->offset(),
            .size = mesh->size(),
        };
        m_context.draw(cmd);
    }
};

#endif /*RENDERERS*/

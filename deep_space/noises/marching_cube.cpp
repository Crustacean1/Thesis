#define GLM_FORCE_SWIZZLE
#include "./marching_cube.h"
#include "../components/compute_program.h"
#include "../components/shader_buffer.h"
#include "../opengl/graphical_device_gl.h"

#include <cassert>
#include <glm/gtx/norm.hpp>
#include <iostream>

extern int triangle_connection_table[256][16];
extern int edge_indexes[256];

unsigned int edge_vertices[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                                     {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

glm::vec3 cube_corners[8] = {glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
                             glm::vec3(1.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
                             glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 0.0, 1.0),
                             glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 1.0)};

constexpr unsigned int samples = 50;
constexpr unsigned int vertex_count = samples + 1;

unsigned int lookup[12];
unsigned int triangle[3];

void marching_cubes(std::vector<glm::vec3> &vertices, std::vector<unsigned int> &indices,
                    glm::vec4 position, unsigned int level, std::function<float(glm::vec3)> value,
                    std::function<glm::vec3(glm::vec3)> derivative) {
    float *values = new float[vertex_count * vertex_count * vertex_count];

    glm::vec3 origin = position.xyz() - glm::vec3(0.5 * vertex_count * position.w);
    glm::vec3 pos = origin;
    glm::vec3 dx = glm::vec3(1, 0, 0) * position.w;
    glm::vec3 dy = glm::vec3(0, 1, 0) * position.w;
    glm::vec3 dz = glm::vec3(0, 0, 1) * position.w;

    for (int x = 0, i = 0; x < vertex_count; x++, pos += dx) {
        pos.y = origin.y;
        for (int y = 0; y < vertex_count; y++, pos += dy) {
            pos.z = origin.z;
            for (int z = 0; z < vertex_count; z++, pos += dz, i++) {
                values[i] = value(pos);
            }
        }
    }

    float *v_directions[8] = {
        values,
        values + vertex_count * vertex_count,
        values + vertex_count * vertex_count + vertex_count,
        values + vertex_count,
        values + 1,
        values + vertex_count * vertex_count + 1,
        values + vertex_count * vertex_count + vertex_count + 1,
        values + vertex_count + 1,
    };

    unsigned int mask;

    origin = position.xyz() - glm::vec3(0.5 * samples * position.w);
    pos = origin;

    for (int x = 0, i = 0, j = 0; x < samples; x++, pos += dx) {
        pos.y = origin.y;
        for (int y = 0; y < samples; y++, pos += dy) {
            pos.z = origin.z;
            for (int z = 0; z < samples; z++, mask = 0, i++, pos += dz, j++) {
                mask = 0;
                for (int c = 0; c < 8; c++) {
                    mask <<= 1;
                    mask += (v_directions[7 - c][i] > 0);
                }
                if (mask != 0 && mask != 255) {
                    if (level != 1) {
                        marching_cubes(vertices, indices, glm::vec4(pos, position.w / samples),
                                       level - 1, value, derivative);
                    } else {
                        auto edge_config = edge_indexes[mask];
                        int k = 0;
                        while (edge_config) {
                            if (edge_config & 1) {
                                auto l_pos = pos + (cube_corners[edge_vertices[k][0]] +
                                                    cube_corners[edge_vertices[k][1]]) *
                                                       0.5f * position.w;
                                lookup[k] = vertices.size();
                                vertices.push_back(l_pos);
                            }
                            edge_config >>= 1;
                            k++;
                        }
                        for (int q = 0; triangle_connection_table[mask][q] != -1;) {
                            for (int a = 0; a < 3; a++, q++) {
                                indices.push_back(triangle[a] =
                                                      lookup[triangle_connection_table[mask][q]]);
                            }
                        }
                    }
                }
            }
            i++;
        }
        i += vertex_count;
    }
}

struct CVertex {
    glm::vec4 pos;
    glm::vec4 norm;
};

struct Cube {
    CVertex vertices[12];
    int indices[16];
};

struct MarchingCubesOutput {
    Cube cubes[64 * 64 * 64];
};

struct MarchingCubesDefinition {
    int edge_table[256];
    int triangle_connections[256][16];
    glm::vec4 pos;
    float step;
};

void marching_cubes_gpu(GL::GraphicalDevice &device, std::shared_ptr<ComputeProgram> program,
                        std::vector<Vertex<Position, Normal>> &vertices,
                        std::vector<unsigned int> &indices, float size, unsigned int resolution,
                        AsteroidVolume::Material &asteroid) {
    int log_res = 0;
    while (resolution /= 2) {
        log_res++;
    }
    resolution = (1 << log_res);

    MarchingCubesDefinition definition;

    definition.pos = glm::vec4(0);
    definition.step = 0.04f;
    memcpy(definition.edge_table, edge_indexes, sizeof(edge_indexes));
    memcpy(definition.triangle_connections, triangle_connection_table,
           sizeof(triangle_connection_table));

    ShaderBuffer definition_buffer(device, UsagePattern::Write, sizeof(definition), &definition);
    ShaderBuffer steroid_buffer(device, UsagePattern::Write, sizeof(asteroid), &asteroid);
    ShaderBuffer output_buffer(device, UsagePattern::Read, sizeof(MarchingCubesOutput));

    definition_buffer.load(&definition, sizeof(definition));

    definition_buffer.bind(1);
    output_buffer.bind(2);
    steroid_buffer.bind(3);

    MarchingCubesOutput *output;

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            for (int z = 0; z < 4; z++) {
                definition.pos = glm::vec4(-5) + glm::vec4(x, y, z, 0) * 64.f * definition.step;
                definition.step = 0.04f;
                definition_buffer.load(&definition, sizeof(definition));
                device.wait_for_compute();

                device.dispatch(program->handle, 64 / 8, 64 / 8, 64 / 8);
                device.wait_for_compute();
                output_buffer.map((void **)&output);

                for (int i = 0; i < 64 * 64 * 64; i++) {
                    for (int j = 0; output->cubes[i].indices[j] != -1; j++) {
                        int index = output->cubes[i].indices[j];
                        auto vertex = output->cubes[i].vertices[index];
                        indices.push_back(vertices.size());
                        vertices.push_back(Vertex<Position, Normal>(
                            (vertex.pos + definition.pos).xyz(), vertex.norm.xyz()));
                    }
                }
                output_buffer.unmap();
            }
        }
    }
}

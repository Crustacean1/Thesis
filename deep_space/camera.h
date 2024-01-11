#ifndef CAMERA
#define CAMERA

#include "components/camera_manager.h"
#include "components/framebuffer.h"
#include "components/uniform_buffer.h"
#include "graphical_context.h"
#include "pipeline.h"
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

namespace GL {
class GraphicalDevice;
}

struct OrthFrustrum {
    float width = 128;
    float height = 128;
    float near = 0.1;
    float far = 10;

    glm::mat4 projection();
};

class IRenderingPipeline;

struct PerspFrustrum {
    float width = 128;
    float height = 128;
    float near = 0.1;
    float far = 10;
    float fov = 1;

    glm::mat4 projection();
};

class Camera {
    static constexpr unsigned int CAMERA_UNIFORM_LOCATION = 1;

    struct SingleCameraUniform {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 pos;
    };

    struct OmniCameraUniform {
        glm::mat4 projection;
        glm::mat4 view[6];
        glm::vec3 pos;
    };

    const enum CameraType { Directional, OmniDirectional } m_camera_type;
    const enum Projection { Orthogonal, Perspective } m_camera_projection;

    union {
        OrthFrustrum orth;
        PerspFrustrum persp;
    } m_frustrum;

    std::shared_ptr<Framebuffer> m_fb;
    std::unique_ptr<UniformBuffer<SingleCameraUniform>> m_single_uniform = nullptr;
    std::unique_ptr<UniformBuffer<OmniCameraUniform>> m_omni_uniform = nullptr;

    void regenerate_projection(glm::mat4 *projection);
    void regenerate_view(const Transform &transform);
    void regenerate_omni_view(const Transform &transform);

    void bind(unsigned int slot = CAMERA_UNIFORM_LOCATION);

  public:
    typedef CameraManager Manager;
    template <typename T> using Ref = std::shared_ptr<T>;

    Camera(GL::GraphicalDevice &device, OrthFrustrum);
    Camera(GL::GraphicalDevice &device, OrthFrustrum, Ref<Texture2D>);

    Camera(GL::GraphicalDevice &device, PerspFrustrum);
    Camera(GL::GraphicalDevice &device, PerspFrustrum, Ref<Texture2DFloat>);
    Camera(GL::GraphicalDevice &device, PerspFrustrum, Ref<CubemapTexture>);

	bool clear = true;
    int layer = 0;
    std::shared_ptr<IRenderingPipeline> pipeline = nullptr;

    bool try_get_orth(OrthFrustrum **frustrum);
    bool try_get_persp(PerspFrustrum **frustrum);

    void fov(float fov);
    void scale(float scale);

    Ref<Framebuffer> framebuffer();

    void start_frame(const Transform &transform);

    BoundingRect<float> viewport();

    static bool sort(Camera *a, Camera *b);
};

#endif /*CAMERA*/

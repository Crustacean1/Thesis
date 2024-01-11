#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

glm::mat4 OrthFrustrum::projection() {
    glm::mat4 projection(1);

    projection[0][0] = 2.0 / width;
    projection[1][1] = 2.0 / height;
    projection[2][2] = 2 / (near - far);
    projection[3][2] = (far + near) / (near - far);

    return projection;
}

glm::mat4 PerspFrustrum::projection() {
    auto projection = glm::mat4(0.0);

    projection[0][0] = fov * height / width;
    projection[1][1] = fov;
    projection[2][2] = (far + near) / (near - far);
    projection[2][3] = -1.0;
    projection[3][2] = 2.0 * (far * near) / (near - far);

    return projection;
}

bool Camera::sort(Camera *a, Camera *b) { return a->layer > b->layer; }

void Camera::bind(unsigned int slot) { m_single_uniform->bind(slot); }

Camera::Camera(GL::GraphicalDevice &device, OrthFrustrum frustrum)
    : m_single_uniform(std::make_unique<UniformBuffer<SingleCameraUniform>>(device)),
      m_camera_type(CameraType::Directional), m_camera_projection(Orthogonal),
      m_frustrum{.orth = frustrum}, m_fb(Framebuffer::default_fb(device)) {}

Camera::Camera(GL::GraphicalDevice &device, OrthFrustrum frustrum, Ref<Texture2D> texture)
    : m_single_uniform(std::make_unique<UniformBuffer<SingleCameraUniform>>(device)),
      m_camera_type(CameraType::Directional), m_camera_projection(Orthogonal),
      m_frustrum{.orth = frustrum}, m_fb(std::make_shared<Framebuffer>(device)) {
    m_fb->bind(texture->handle);
}

Camera::Camera(GL::GraphicalDevice &device, PerspFrustrum frustrum)
    : m_single_uniform(std::make_unique<UniformBuffer<SingleCameraUniform>>(device)),
      m_camera_type(CameraType::Directional), m_camera_projection(Perspective),
      m_frustrum{.persp = frustrum}, m_fb(Framebuffer::default_fb(device)) {}

Camera::Camera(GL::GraphicalDevice &device, PerspFrustrum frustrum, Ref<Texture2DFloat> texture)
    : m_single_uniform(std::make_unique<UniformBuffer<SingleCameraUniform>>(device)),
      m_camera_type(CameraType::Directional), m_camera_projection(Perspective),
      m_frustrum{.persp = frustrum}, m_fb(std::make_shared<Framebuffer>(device)) {
    m_fb->bind(texture->handle);
}

Camera::Camera(GL::GraphicalDevice &device, PerspFrustrum frustrum, Ref<CubemapTexture> texture)
    : m_omni_uniform(std::make_unique<UniformBuffer<OmniCameraUniform>>(device)),
      m_camera_type(CameraType::OmniDirectional), m_camera_projection(Perspective),
      m_frustrum{.persp = frustrum}, m_fb(std::make_shared<Framebuffer>(device)) {
    m_fb->bind(texture->handle);
}

void Camera::fov(float fov) {}

void Camera::scale(float scale) {}

void Camera::regenerate_projection(glm::mat4 *projection) {
    if (m_camera_projection == Projection::Orthogonal) {
        *projection = m_frustrum.orth.projection();
    } else {
        *projection = m_frustrum.persp.projection();
    }
}

glm::quat VIEWS[6] = {glm::quatLookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                      glm::quatLookAt(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                      glm::quatLookAt(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                      glm::quatLookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                      glm::quatLookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                      glm::quatLookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

void Camera::regenerate_omni_view(const Transform &transform) {
    m_omni_uniform->data.pos = transform.position();
    for (int i = 0; i < 6; i++) {
        m_omni_uniform->data.view[i] =
            glm::toMat4(glm::conjugate(transform.orientation()) * VIEWS[i]),
        glm::translate(glm::mat4(1), -transform.position());
    }
}

void Camera::regenerate_view(const Transform &transform) {
    m_single_uniform->data.view = glm::toMat4(glm::conjugate(transform.orientation())) *
                                  glm::translate(glm::mat4(1), -transform.position());
    m_single_uniform->data.pos = transform.position();
}

void Camera::start_frame(const Transform &camera) {
    if (m_camera_type == CameraType::Directional) {
        regenerate_view(camera);
        regenerate_projection(&m_single_uniform->data.projection);

        m_single_uniform->load();
        m_single_uniform->bind(1);
    } else {
        regenerate_omni_view(camera);
        regenerate_projection(&m_omni_uniform->data.projection);

        m_omni_uniform->bind(1);
        m_omni_uniform->load();
    }
}

BoundingRect<float> Camera::viewport() {
    switch (m_camera_projection) {
    case Orthogonal:
        return BoundingRect<float>{0, 0, m_frustrum.orth.width, m_frustrum.orth.height};
    case Perspective:
        return BoundingRect<float>{0, 0, m_frustrum.persp.width, m_frustrum.persp.height};
    }
}

Camera::Ref<Framebuffer> Camera::framebuffer() { return m_fb; }

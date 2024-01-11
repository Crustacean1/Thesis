#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "context.h"
#include "imgui.h"

#include "./camera_controller.h"

#include "node/node_context.h"
#include "scene_manager.h"
#include "window.h"

CameraController::CameraController(ISceneManager &scene_manager, CameraManager &camera_manager,
                                   GL::GraphicalDevice &context,
                                   std::shared_ptr<Texture2DFloat> target, Window &window)
    : window(window), m_camera_manager(camera_manager), node_manager(scene_manager),
      camera_node(
          scene_manager.node(Transform(0, 2, 0))
              .component(Camera(context, PerspFrustrum{1200, 800, 0.1, 1000.0, 5}, target))) {

    window.subscribe_to_keypress(
        [this](int key, int scan, int action, int mode) { on_key_press(key, scan, action, mode); });

    window.subscribe_to_click(
        [this](int button, int action, int mods) { on_click(button, action, mods); });

    window.subscribe_to_movement([this](double x, double y) { on_mouse_move(x, y); });

    window.register_update([this](double time) { update(time); });

    window.register_gui("camera", [this]() { gui_handler(); });
}

void CameraController::on_press(int key) {
    unsigned int ukey = key;
    if (ukey < 256) {
        key_index[ukey] = key_count;
        active_keys[(key_count)++] = ukey;
    }
}

void CameraController::on_release(int key) {
    unsigned int ukey = key;
    if (ukey < 256) {
        key_count--;
        active_keys[key_index[key]] = active_keys[key_count];
        key_index[active_keys[key_count]] = key_index[key];
    }
}

void CameraController::on_mouse_move(double x, double y) {
    if (looking_enabled) {
        auto delta_x = (float)(x - prev_x);
        auto delta_y = (float)(y - prev_y);

        if (rigid_mode) {
            xangle += delta_x;
            yangle += delta_y;
            node_manager.node(camera_node)
                .orientation(glm::angleAxis((float)xangle * camera_speed, glm::vec3(0, -1, 0)) *
                             glm::angleAxis((float)yangle * camera_speed, glm::vec3(-1, 0, 0)));

            // node_manager.node(camera_node)
            //.turn(glm::angleAxis(delta_x * camera_speed, glm::vec3(0.0, -1.0, 0.0)));
            // node_manager.node(camera_node)
            //.turn(glm::angleAxis(delta_y * camera_speed, glm::vec3(-1.0, 0.0, 0.0)));

            // node_manager.node(camera_node).turn(movement);
        } else {
            auto movement = glm::angleAxis(delta_x * camera_speed, glm::vec3(0.0, -1.0, 0.0)) *
                            glm::angleAxis(delta_y * camera_speed, glm::vec3(-1.0, 0.0, 0.0));

            node_manager.node(camera_node).turn(movement);
        }
    }

    prev_x = x;
    prev_y = y;
}

void CameraController::update(double time) {

    const float speed = (time - prev_time) * 0.0000001 * movement_speed;

    for (auto i = 0; i < key_count; i++) {
        glm::vec3 dir(0);
        switch (active_keys[i]) {
        case GLFW_KEY_W:
            dir += glm::vec3(0, 0, -1.0);
            break;
        case GLFW_KEY_S:
            dir += glm::vec3(0, 0, 1.0);
            break;
        case GLFW_KEY_A:
            dir += glm::vec3(-1.0, 0, 0);
            break;
        case GLFW_KEY_D:
            dir += glm::vec3(1.0, 0, 0);
            break;
        default:
            continue;
        }
        node_manager.node(camera_node).move(glm::normalize(dir) * speed);
    }

    prev_time = time;
}

void CameraController::on_key_press(int key, int scane, int action, int mode) {
    if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
        window.close();
        return;
    }

    if (action == GLFW_PRESS) {
        on_press(key);
    } else if (action == GLFW_RELEASE) {
        on_release(key);
    }
}

void CameraController::on_click(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        looking_enabled = action == GLFW_PRESS;
        window.show_cursor(action != GLFW_PRESS);
    }
}

void CameraController::gui_handler() {
    ImGui::SliderFloat("Movement speed", &movement_speed, 0.0, 1);
    ImGui::SliderFloat("Camera speed", &camera_speed, 0.0, 0.01);
    ImGui::Checkbox("Static camera", &rigid_mode);
}

std::pair<Node *, Camera *> CameraController::get_camera_node() {
    return std::pair(&node_manager.node(camera_node), &m_camera_manager.get(camera_node));
}

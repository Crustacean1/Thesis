#ifndef CAMERA_CONTROLLER
#define CAMERA_CONTROLLER

#include "components/camera_manager.h"
#include "components/texture.h"
#include "node/node_context.h"
#include "opengl/graphical_device_gl.h"

namespace GL {
class GraphicalDevice;
}

class ISceneManager;
class Window;
class Camera;
class Gui;

class CameraController {
    Window &window;
    ISceneManager &node_manager;
    CameraManager &m_camera_manager;
    NodeHandle camera_node;

    float scale = 1.0;
    float fov = 5.0;
    bool rigid_mode = false;

    bool looking_enabled = false;
    char active_keys[255];
    char key_index[255];
    char key_count = 0;

    double prev_x;
    double prev_y;

    double xangle;
    double yangle;

    float movement_speed = 0.5;
    float camera_speed = 0.001f;

    double prev_time = 0;

    void on_press(int key);
    void on_release(int key);

  public:
    CameraController(ISceneManager &scene_manager, CameraManager &manager,
                     GL::GraphicalDevice &device, std::shared_ptr<Texture2DFloat>, Window &window);

    std::pair<Node *, Camera *> get_camera_node();
    void on_mouse_move(double x, double y);
    void on_key_press(int key, int scan, int action, int mode);
    void on_click(int button, int action, int mods);
    void update(double time);

    void gui_handler();
};

#endif /*CAMERA_CONTROLLER*/

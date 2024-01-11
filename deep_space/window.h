#ifndef WINDOW
#define WINDOW

#include <GLFW/glfw3.h>
#include <functional>
#include <optional>
#include <string>

class Window {
    static Window *window_ptr;
    static void key_callback(GLFWwindow *, int key, int scancode, int action, int mode);
    static void movement_callback(GLFWwindow *, double x, double y);
    static void click_callback(GLFWwindow *, int button, int action, int mode);

    std::vector<std::function<void(double, double)>> movement_sub;
    std::vector<std::function<void(int, int, int)>> click_sub;
    std::vector<std::function<void(int, int, int, int)>> key_sub;

    std::vector<std::function<void(double)>> update_functions;
    std::vector<std::pair<std::string, std::function<void()>>> gui_handlers;

    void gui();

  public:
    GLFWwindow *window;

    static Window *init(int width, int height, std::optional<const char *> title);
    ~Window();

    void run(std::function<void()> render);
    void close();
    void show_cursor(bool show);

    void subscribe_to_movement(std::function<void(double, double)> callback);
    void subscribe_to_click(std::function<void(int, int, int)> callback);
    void subscribe_to_keypress(std::function<void(int, int, int, int)> callback);
    void register_update(std::function<void(double)> update);
    void register_gui(std::string name, std::function<void()> gui_handler);
};

#endif /*WINDOW*/

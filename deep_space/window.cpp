#include <algorithm>
#include <functional>
#include <glad/glad.h>

#include "window.h"
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "./runtime_exception.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <chrono>

Window *Window::window_ptr = nullptr;

Window *Window::init(int width, int height, std::optional<const char *> title) {
    if (window_ptr != nullptr) {
        return window_ptr;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window_title = title.value_or("New window");
    auto window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);

    if (window == nullptr) {
        throw RuntimeException::error("Failed to create window");
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, Window::key_callback);
    glfwSetCursorPosCallback(window, Window::movement_callback);
    glfwSetMouseButtonCallback(window, Window::click_callback);

    auto result = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!result) {
        throw new std::runtime_error("Failed to load OpenGL functions");
    }

    window_ptr = new Window();
    window_ptr->window = window;

    // IM gui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::StyleColorsDark();

    return window_ptr;
}

Window::~Window() { glfwTerminate(); }

void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    for (auto subscriber : window_ptr->key_sub) {
        subscriber(key, scancode, action, mode);
    }
}

void Window::movement_callback(GLFWwindow *window, double x, double y) {
    for (auto subscriber : window_ptr->movement_sub) {
        subscriber(x, y);
    }
}

void Window::click_callback(GLFWwindow *window, int button, int action, int mods) {
    for (auto subscriber : window_ptr->click_sub) {
        subscriber(button, action, mods);
    }
}

void Window::subscribe_to_keypress(std::function<void(int, int, int, int)> callback) {
    key_sub.push_back(callback);
}

void Window::register_update(std::function<void(double)> update) {
    update_functions.push_back(update);
}

void Window::subscribe_to_movement(std::function<void(double, double)> callback) {
    movement_sub.push_back(callback);
}

void Window::subscribe_to_click(std::function<void(int, int, int)> callback) {
    click_sub.push_back(callback);
}

void Window::gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("Dock");

    ImGui::BeginTabBar("Tabs");
    for (auto [name, handler] : gui_handlers) {
        if (ImGui::BeginTabItem(name.c_str())) {
            handler();
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::run(std::function<void()> render) {
    std::chrono::high_resolution_clock clock;

    auto start = clock.now();

    while (!glfwWindowShouldClose(window)) {
        std::chrono::time_point now = clock.now();
        // Events
        glfwPollEvents();

        auto delta = clock.now() - start;
        for (auto update : update_functions) {
            update(delta.count());
        }

        // Rendering

        render();
        gui();

        glfwSwapBuffers(window);
    }
}

void Window::register_gui(std::string name, std::function<void()> gui_handler) {
    gui_handlers.push_back(std::pair(name, gui_handler));
}

void Window::close() { glfwSetWindowShouldClose(window, GLFW_TRUE); }

void Window::show_cursor(bool show) {
    glfwSetInputMode(window, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

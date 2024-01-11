#include <cmath>
#include <iostream>

#include "imgui.h"
#include "perf_monitor.h"
#include "window.h"

PerfMonitor::PerfMonitor(Window &window) : elapsed_since_update(0), rolling_fps(0) {

    window.register_gui("performance", std::bind(&PerfMonitor::gui, this));
}

void PerfMonitor::gui() { ImGui::Text("Current fps: %f", last_fps); }

void PerfMonitor::update(double delta) {
    elapsed_since_update += delta / 1000;
    if (delta != 0) {
        rolling_fps = rolling_fps * 0.9 + 0.1 * (1000.0 / delta);
    }
    if (elapsed_since_update > 1) {
        elapsed_since_update -= 1;
        last_fps = rolling_fps;
    }
}

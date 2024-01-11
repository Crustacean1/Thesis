#ifndef PERF_MONITOR
#define PERF_MONITOR

#include <functional>
class Window;

class PerfMonitor {
    double coefficient = 0.9;

    double elapsed_since_update = 0;
    double rolling_fps;

    double last_fps;

    void gui();
    char message[128];

  public:
    void update(double delta);
    PerfMonitor(Window &window);
};

#endif /*PERF_MONITOR*/

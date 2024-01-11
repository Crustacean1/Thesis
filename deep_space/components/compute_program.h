#ifndef COMPUTE_PROGRAM
#define COMPUTE_PROGRAM

#include "../graphical_device.h"

namespace GL {
class GraphicalDevice;
}

class ComputeProgram {
    GL::GraphicalDevice &m_device;

  public:
    const Handles::ComputeProgram handle;

    ComputeProgram(GL::GraphicalDevice &device, const char *name);

    ComputeProgram(const ComputeProgram &) = delete;
    ComputeProgram &operator=(const ComputeProgram &) = delete;

    void dispatch(int x, int y, int z);
};

#endif /*COMPUTE_PROGRAM*/

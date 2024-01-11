#ifndef PROGRAM
#define PROGRAM

#include "../graphical_device.h"

namespace GL {
class GraphicalDevice;
}

std::pair<unsigned int, char *> read_file(const char *filename);

class Program {
    const std::tuple<const char *, const char *, const char *> sources;
    GL::GraphicalDevice &m_device;

  public:
    const Handles::Program handle;

    Program(GL::GraphicalDevice &device, const char *name);

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    ~Program();
};

#endif /*PROGRAM*/

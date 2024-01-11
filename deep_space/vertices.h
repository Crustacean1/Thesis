#ifndef VERTICES
#define VERTICES

#include <glm/glm.hpp>

struct VertexComponent {
    unsigned int size;
    unsigned int count;
};

class Color3 {
  public:
    float r;
    float g;
    float b;
    static constexpr unsigned long COUNT = 3;
    static constexpr bool INSTANCED = false;
    typedef glm::vec3 Vec;
};

class Color4 {
  public:
    float r;
    float g;
    float b;
    float a;
    static constexpr unsigned long COUNT = 4;
    static constexpr bool INSTANCED = false;
    Color4(glm::vec4 c) : r(c.x), g(c.y), b(c.z), a(c.w) {}
    typedef glm::vec4 Vec;
};

class Tex {
  public:
    float u;
    float v;
    static constexpr unsigned long COUNT = 2;
    static constexpr bool INSTANCED = false;
    typedef glm::vec2 Vec;
    Tex(Vec vec) : u(vec.x), v(vec.y) {}
    Tex() {}
};

class Normal {
  public:
    float nx;
    float ny;
    float nz;
    static constexpr unsigned long COUNT = 3;
    static constexpr bool INSTANCED = false;
    typedef glm::vec3 Vec;
    glm::vec3 normal() const { return glm::vec3(nx, ny, nz); }
    Normal(Vec vec) : nx(vec.x), ny(vec.y), nz(vec.z) {}
    Normal() {}
};

class Position {
  public:
    float x;
    float y;
    float z;
    static constexpr unsigned long COUNT = 3;
    static constexpr bool INSTANCED = false;
    typedef glm::vec3 Vec;
    void position(glm::vec3 pos) {
        x = pos.x;
        y = pos.y;
        z = pos.z;
    }

    glm::vec3 position() const { return glm::vec3(x, y, z); }

    Position(Vec vec) : x(vec.x), y(vec.y), z(vec.z) {}
    Position() {}
};

template <typename... Attrs> class Vertex : public Attrs... {
  public:
    Vertex() {}
    Vertex(typename Attrs::Vec... vec) : Attrs(vec)... {}
    static constexpr unsigned long STRIDE = (sizeof(Attrs) + ...);
};

#endif /*VERTICES*/

#ifndef TRANSFORM
#define TRANSFORM

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

class Transform {
    glm::mat4 model_mat;
    glm::mat4 view_mat;

    glm::vec3 pos;
    glm::quat orient = glm::quat(1, 0, 0, 0);

    void refresh();

  public:
    Transform();
    Transform(glm::vec3 pos, glm::quat orient);
    Transform(float x, float y, float z);
    Transform(glm::vec3 position);
    const glm::mat4 &model() const;
    const glm::mat4 &view() const;

    void move(glm::vec3 pos);
    void turn(glm::quat dir);

    void position(glm::vec3 pos);
    void orientation(glm::quat orient);

    const glm::vec3 &position() const;
    const glm::quat &orientation() const;

    Transform operator/(const Transform &transform) const;
    Transform operator*(const Transform &transform) const;

    glm::vec3 global(glm::vec3 direction);
    glm::vec3 based_position(glm::vec3 position);
};

#endif /*TRANSFORM*/

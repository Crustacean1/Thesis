#define GLM_FORCE_SWIZZLE
#include "./transform.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

Transform::Transform(glm::vec3 pos, glm::quat orient) : pos(pos), orient(orient) { refresh(); }

Transform::Transform(glm::vec3 position) : orient(1., 0., 0., 0.), pos(position) { refresh(); }

Transform::Transform(float x, float y, float z) : Transform(glm::vec3(x, y, z)) {}

Transform::Transform() { refresh(); }

void Transform::refresh() {
    model_mat = glm::translate(glm::mat4(1), pos);
    model_mat = glm::toMat4(orient) * model_mat;
    // view_mat = glm::toMat4(glm::conjugate(orient)) * glm::translate(glm::mat4(1), -pos);
}

const glm::mat4 &Transform::model() const { return model_mat; }

const glm::vec3 &Transform::position() const { return pos; }

const glm::quat &Transform::orientation() const { return orient; }

void Transform::position(glm::vec3 new_pos) {
    pos = new_pos;
    refresh();
}

void Transform::orientation(glm::quat new_orient) {
    orient = new_orient;
    refresh();
}

void Transform::move(glm::vec3 dir) {
    // TODO: use quaternion
    auto world_dir = model_mat * glm::vec4(dir, 0.0);
    pos += glm::vec3(world_dir);
    refresh();
}

void Transform::turn(glm::quat dir) {
    orient = orient * dir;
    refresh();
}

// Change transformation to local frame of reference
Transform Transform::operator/(const Transform &transform) const {
    glm::vec3 new_pos = pos - transform.pos;
    glm::quat new_orient = glm::conjugate(transform.orient) * orient;
    return Transform{new_pos, new_orient};
}

// Change this transformation to local frame of reference of another transform
Transform Transform::operator*(const Transform &transform) const {
    glm::vec3 new_pos = pos + orient * transform.pos;
    glm::quat new_orient = orient * transform.orient;
    return Transform{new_pos, new_orient};
}

glm::vec3 Transform::global(glm::vec3 direction) { return glm::vec3(orient * direction); }

#pragma once
// moveable mesh
using namespace vivademos;
#include <demos/mesh.hpp>
#include <demos/transformable.hpp>
#include <external.hpp>

namespace vivademos {

struct TransformableMesh {
  Mesh shape;
  Transformable transform;
  vivaphysics::v3 m_color;
  //
  TransformableMesh(const Mesh &s, const Transformable &t)
      : shape(s), transform(t), m_color(1.0) {}

  TransformableMesh(const Mesh &s, const Transformable &t,
                    const vivaphysics::v3 &c)
      : shape(s), transform(t), m_color(c) {}

  void move(MOVE_DIRECTION md, real dtime, const onb &basis) {
    transform.trans.move(md, dtime, basis);
  }
  void set_position(const vivaphysics::v3 &p) {
    transform.trans.set_position(p);
  }
  void rotate_by_euler(ROTATE_DIRECTION direction, vivaphysics::real deltaTime,
                       vivaphysics::real speed) {
    transform.rot.rotate_by_euler(direction, deltaTime, speed);
  }
  glm::mat4 model() { return transform.model(); }
  void draw(Shader shdr) { shape.draw(shdr); }
  glm::vec3 color() const { return m_color.to_glm(); }
};

}; // namespace vivademos

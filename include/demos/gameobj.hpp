#pragma once
// moveable mesh
using namespace vivademos;
#include <demos/mesh.hpp>
#include <demos/transformable.hpp>
#include <external.hpp>
#include <vivaphysics/particle.hpp>

namespace vivademos {

struct TransformableMesh {
  Mesh shape;
  Transformable transform;
  vivaphysics::v3 m_color;
  //
  TransformableMesh() {}
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

struct ParticleParams {
  vivaphysics::real mass;
  vivaphysics::v3 velocity;
  vivaphysics::v3 acceleration;
  vivaphysics::real damping;
};

struct ParticleMesh {
  Mesh shape;
  vivaphysics::v3 m_color;
  vivaphysics::Particle particle;
  unsigned int start_time = 0;
  ParticleMesh(){}
  ParticleMesh(const Mesh &s) : shape(s), m_color(1.0) {}
  ParticleMesh(const Mesh &s, const vivaphysics::v3 &c)
      : shape(s), m_color(c) {}

  void set_particle_params(const ParticleParams &params) {
    particle.set_mass(params.mass);
    particle.set_velocity(params.velocity);
    particle.set_acceleration(params.acceleration);
    particle.set_damping(params.damping);
  }
  glm::mat4 model() const {
    vivaphysics::v3 pos;
    particle.get_position(pos);
    auto posv = pos.to_glm();
    auto m = glm::mat4(1.0);
    return glm::translate(m, posv);
  }
  void draw(Shader shdr) { shape.draw(shdr); }
  glm::vec3 color() const { return m_color.to_glm(); }
};

}; // namespace vivademos

#pragma once

// transformable object that is an object that can be moved
// by multiplying a model matrix
#include <external.hpp>
#include <stdexcept>

namespace vivademos {
struct Translatable {
  vivaphysics::v3 position;
  bool trans_check = false;
  Translatable() : trans_check(false) {}
  Translatable(const vivaphysics::v3 &p)
      : position(p), trans_check(true) {}
  glm::mat4 translate(const glm::mat4 &transMat) const {
    if (trans_check) {
      auto nmat = glm::translate(transMat, position);
      return nmat;
    } else {
      throw std::runtime_error(
          "position has not been initialized");
    }
  }
};
struct Rotatable {
  vivaphysics::v3 axis;
  vivaphysics::real angle;
  bool rot_check = false;
  Rotatable() : rot_check(false) {}
  Rotatable(const vivaphysics::v3 &a, vivaphysics::real r)
      : axis(a), angle(r), rot_check(true) {}
  glm::vec3 rotate(const glm::vec3 &pos) {
    //
    if (rot_check) {
      auto axis_normal = axis.normalized();
      auto rot =
          glm::rotate(pos, angle, axis_normal.to_glm());
      return rot;
    } else {
      throw std::runtime_error(
          "rotation values are not initialized");
    }
  }
  glm::mat4 rotate(const glm::mat4 &model) const {
    //
    if (rot_check) {
      auto axis_normal = axis.normalized();
      auto rot =
          glm::rotate(model, angle, axis_normal.to_glm());
      return rot;
    } else {
      throw std::runtime_error(
          "rotation values are not initialized");
    }
  }
};
struct Scalable {
  vivaphysics::v3 coeffs;
  bool scale_check = false;
  Scalable() : coeff_check(false) {}
  Scalable(const vivaphysics::v3 &cs)
      : coeffs(cs), coeff_check(true) {}
  glm::mat4 scale(const glm::mat4 &m) const {
    if (scale_check) {
      auto scaled_matrix = glm::scale(m, coeffs.to_glm());
      return scaled_matrix;
    } else {
      throw std::runtime_error(
          "scale coefficients are not initialized");
    }
  }
};
struct Transformable : Translatable, Rotatable, Scalable {
  Scalable sc;
  Rotatable rot;
  Translatable trans;
  Transformable() {}
  Transformable(const vivaphysics::v3 &pos) : trans(pos) {}
  Transformable(const vivaphysics::v3 &axis,
                vivaphysics::real angle)
      : rot(axis, angle) {}
  Transformable(const vivaphysics::v3 &pos,
                const vivaphysics::v3 &s)
      : trans(pos), sc(s) {}
  Transformable(const vivaphysics::v3 &pos,
                const vivaphysics::v3 &rot_axis,
                vivaphysics::real angle,
                const vivaphysics::v3 &s)
      : trans(pos), rot(rot_axis, angle), sc(s) {}
  /**
   Obtain model matrix of the transformable object*/
  glm::mat4 model() {
    auto m = glm::mat4(1.0f);
    if (trans.trans_check) {
      m = translate(m);
    }
    if (rot.rot_check) {
      m = rotate(m);
    }
    if (sc.scale_check) {
      m = scale(m);
    }
    return m;
  }
}

}; // namespace vivademos

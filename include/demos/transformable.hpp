#pragma once

// transformable object that is an object that can be moved
// by multiplying a model matrix
#include <external.hpp>
#include <vivaphysics/quaternion.hpp>
#include <vivaphysics/vec3.hpp>

namespace vivademos {
enum class MOVE_DIRECTION {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};
enum class ROTATE_DIRECTION {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  ROTATE_X,
  ROTATE_Y,
  ROTATE_Z,
  U_AXIS
};

struct Translatable {
  vivaphysics::v3 position;
  bool trans_check = false;
  Translatable() : trans_check(false) {}
  Translatable(const vivaphysics::v3 &p)
      : position(p), trans_check(true) {}
  void move(MOVE_DIRECTION md, real dtime) {
    switch (md) {
    case MOVE_DIRECTION::LEFT: {
      position.x -= dtime;
    }
    case MOVE_DIRECTION::RIGHT: {
      position.x += dtime;
    }
    case MOVE_DIRECTION::DOWN: {
      position.y -= dtime;
    }
    case MOVE_DIRECTION::UP: {
      position.y += dtime;
    }
    case MOVE_DIRECTION::BACKWARD: {
      position.z -= dtime;
    }
    case MOVE_DIRECTION::BACKWARD: {
      position.z += dtime;
    }
    }
  }
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
  vivaphysics::q4 quat;
  bool rot_check = false;
  Rotatable() : rot_check(false) {}
  Rotatable(const vivaphysics::v3 &a, vivaphysics::real r)
      : quat(r, a), rot_check(true) {}

  Rotatable(real r, real x, real y, real z)
      : quat(r, x, y, z), rot_check(true) {}
  static Rotatable
  fromEulerAxisAngle(real degree,
                     const vivaphysics::v3 &axis) {
    auto rad = static_cast<real>(glm::radians(degree));
    auto unit = axis.normalized();
    auto scast = static_cast<real>(2.0);
    auto q_r = std::cos(rad / scast);
    auto sin_theta = std::sin(rad / scast);
    auto q_i = unit.x * sin_theta;
    auto q_j = unit.y * sin_theta;
    auto q_k = unit.z * sin_theta;
    return Rotatable(q_r, q_i, q_j, q_k);
  }
  static Rotatable fromEulerAngles(real yaw, real pitch,
                                   real roll) {
    //
    auto sin_real = [](real euler) -> real {
      return static_cast<real>(std::sin(
          glm::radians(euler) / static_cast<real>(2)));
    };
    auto cos_real = [](real euler) -> real {
      return static_cast<real>(std::cos(
          glm::radians(euler) / static_cast<real>(2)));
    };
    auto roll_sin = sin_real(roll);
    auto roll_cos = cos_real(roll);
    auto yaw_sin = sin_real(yaw);
    auto yaw_cos = cos_real(yaw);
    auto pitch_sin = sin_real(pitch);
    auto pitch_cos = cos_real(pitch);

    //
    auto q_r = (roll_cos * yaw_cos * pitch_cos) +
               (roll_sin * yaw_sin * pitch_sin);
    auto q_k = (roll_cos * pitch_cos * yaw_sin) -
               (roll_sin * pitch_sin * yaw_cos);
    auto q_j = (roll_cos * yaw_cos * pitch_sin) +
               (roll_sin * yaw_sin * pitch_cos);
    auto q_i = (roll_sin * pitch_cos * yaw_cos) -
               (roll_cos * pitch_sin * yaw_sin);
    return Rotatable(q_r, q_i, q_j, q_k);
  }
  glm::vec3 rotate(const glm::vec3 &pos) {
    //
    if (rot_check) {
      auto axis_normal = quat.vector().normalized();
      auto angle =
          static_cast<real>(2 * std::acos(quat.scalar()));
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
      auto axis_normal = quat.vector().normalized();
      auto angle =
          static_cast<real>(2 * std::acos(quat.scalar()));
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
  Scalable() : scale_check(false) {}
  Scalable(const vivaphysics::v3 &cs)
      : coeffs(cs), scale_check(true) {}
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
};
}; // namespace vivademos

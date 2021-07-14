#pragma once

// transformable object that is an object that can be moved
// by multiplying a model matrix
#include <external.hpp>
#include <vivaphysics/eulerangle.hpp>
#include <vivaphysics/onb.hpp>
#include <vivaphysics/quaternion.hpp>
#include <vivaphysics/vec3.hpp>

namespace vivademos {
enum class MOVE_DIRECTION { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };
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
  Translatable(const vivaphysics::v3 &p) : position(p), trans_check(true) {}
  void move(MOVE_DIRECTION md, real dtime, const onb &basis) {
    switch (md) {
    case MOVE_DIRECTION::LEFT: {
      position -= basis.u() * dtime;
      break;
    }
    case MOVE_DIRECTION::RIGHT: {
      position += basis.u() * dtime;
      break;
    }
    case MOVE_DIRECTION::DOWN: {
      position -= basis.v() * dtime;
      break;
    }
    case MOVE_DIRECTION::UP: {
      position += basis.v() * dtime;
      break;
    }
    case MOVE_DIRECTION::BACKWARD: {
      position -= basis.w() * dtime;
      break;
    }
    case MOVE_DIRECTION::FORWARD: {
      position += basis.w() * dtime;
      break;
    }
    }
  }
  void move(MOVE_DIRECTION md, real dtime) {
    onb basis;
    basis.set_w(v3(0, 0, 1));
    basis.set_v(v3(0, 1, 0));
    basis.set_u(v3(1, 0, 0));
    move(md, dtime, basis);
  }
  void set_position(const vivaphysics::v3 &p) { position = p; }
  glm::mat4 translate(const glm::mat4 &transMat) const {
    if (trans_check) {
      auto nmat = glm::translate(transMat, position);
      return nmat;
    } else {
      throw std::runtime_error("position has not been initialized");
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
  static Rotatable fromEulerAxisAngle(real degree,
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
  static Rotatable fromEulerAngles(real yaw, real pitch, real roll) {
    //
    auto sin_real = [](real euler) -> real {
      return static_cast<real>(
          std::sin(glm::radians(euler) / static_cast<real>(2)));
    };
    auto cos_real = [](real euler) -> real {
      return static_cast<real>(
          std::cos(glm::radians(euler) / static_cast<real>(2)));
    };
    auto roll_sin = sin_real(roll);
    auto roll_cos = cos_real(roll);
    auto yaw_sin = sin_real(yaw);
    auto yaw_cos = cos_real(yaw);
    auto pitch_sin = sin_real(pitch);
    auto pitch_cos = cos_real(pitch);

    //
    auto q_r =
        (roll_cos * yaw_cos * pitch_cos) + (roll_sin * yaw_sin * pitch_sin);
    auto q_k =
        (roll_cos * pitch_cos * yaw_sin) - (roll_sin * pitch_sin * yaw_cos);
    auto q_j =
        (roll_cos * yaw_cos * pitch_sin) + (roll_sin * yaw_sin * pitch_cos);
    auto q_i =
        (roll_sin * pitch_cos * yaw_cos) - (roll_cos * pitch_sin * yaw_sin);
    return Rotatable(q_r, q_i, q_j, q_k);
  }

  static Rotatable fromEulerAngles(const vivaphysics::euler_angles &angles) {
    //
    return fromEulerAngles(angles.yaw(), angles.pitch(), angles.roll());
  }
  vivaphysics::euler_angles to_euler() const {
    auto qri = 2 * (quat.scalar() * quat.x() + quat.y() * quat.z());
    auto qcos = 1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y());
    // compute roll (x-axis rotation) value
    auto roll = std::atan2(qri, qcos);

    // compute pitch (y axis rotation) value
    auto sinp = 2 * (quat.scalar() * quat.y() - quat.z() * quat.x());
    real pitch;
    if (std::abs(sinp) >= 1) {
      pitch = std::copysign(M_PI / 2, sinp);
    } else {
      pitch = std::asin(sinp);
    }
    // compute yaw (z axis rotation) value
    auto sin_yaw = 2 * (quat.scalar() * quat.z() + quat.x() * quat.y());
    auto cos_yaw = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
    auto yaw = std::atan2(sin_yaw, cos_yaw);
    vivaphysics::euler_angles angles;
    angles.set_yaw(yaw);
    angles.set_roll(roll);
    angles.set_pitch(pitch);
    return angles;
  }
  void set_euler(vivaphysics::real v, vivaphysics::EULER_ANGLE a) {
    auto angles = to_euler();
    switch (a) {
    case vivaphysics::EULER_ANGLE::YAW: {
      angles.set_yaw(v);
    }
    case vivaphysics::EULER_ANGLE::ROLL: {
      angles.set_roll(v);
    }
    case vivaphysics::EULER_ANGLE::PITCH: {
      angles.set_pitch(v);
    }
    }
    *this = Rotatable::fromEulerAngles(angles);
  }
  void set_yaw(vivaphysics::real nyaw) {
    set_euler(nyaw, vivaphysics::EULER_ANGLE::YAW);
  }
  void set_pitch(vivaphysics::real np) {
    set_euler(np, vivaphysics::EULER_ANGLE::PITCH);
  }
  void set_roll(vivaphysics::real nr) {
    set_euler(nr, vivaphysics::EULER_ANGLE::ROLL);
  }
  void rotate_by_euler(ROTATE_DIRECTION direction, vivaphysics::real deltaTime,
                       vivaphysics::real speed) {
    deltaTime *= speed;
    euler_angles angles = to_euler();
    vivaphysics::real yaw = angles.yaw();
    vivaphysics::real pitch = angles.pitch();
    if (direction == ROTATE_DIRECTION::FORWARD) {
      pitch += deltaTime;
    } else if (direction == ROTATE_DIRECTION::BACKWARD) {
      pitch -= deltaTime;
    } else if (direction == ROTATE_DIRECTION::LEFT) {
      yaw -= deltaTime;
    } else if (direction == ROTATE_DIRECTION::RIGHT) {
      yaw += deltaTime;
    } else {
      throw std::runtime_error("rotate by euler accepts only forward,backward, "
                               "left, right rotation directions");
    }
    angles.set_yaw(yaw);
    angles.set_pitch(pitch);
    *this = Rotatable::fromEulerAngles(angles);
  }
  glm::mat4 rotate(const glm::mat4 &model) const {
    //
    if (rot_check) {
      auto axis_normal = quat.vector().normalized();
      auto angle = static_cast<real>(2 * std::acos(quat.scalar()));
      auto rot = glm::rotate(model, angle, axis_normal.to_glm());
      return rot;
    } else {
      throw std::runtime_error("rotation values are not initialized");
    }
  }
};
struct Scalable {
  vivaphysics::v3 coeffs;
  bool scale_check = false;
  Scalable() : scale_check(false) {}
  Scalable(const vivaphysics::v3 &cs) : coeffs(cs), scale_check(true) {}
  glm::mat4 scale(const glm::mat4 &m) const {
    if (scale_check) {
      auto scaled_matrix = glm::scale(m, coeffs.to_glm());
      return scaled_matrix;
    } else {
      throw std::runtime_error("scale coefficients are not initialized");
    }
  }
};
struct Transformable {
  Scalable sc;
  Rotatable rot;
  Translatable trans;
  Transformable() {}
  Transformable(const Translatable &t, const Rotatable &r, const Scalable &s)
      : sc(s), rot(r), trans(t) {}
  Transformable(const Translatable &t, const Rotatable &r) : rot(r), trans(t) {}
  Transformable(const Translatable &t) : trans(t) {}

  Transformable(const vivaphysics::v3 &pos) : trans(pos) {}
  Transformable(const vivaphysics::v3 &axis, vivaphysics::real angle)
      : rot(axis, angle) {}
  Transformable(const vivaphysics::v3 &pos, const vivaphysics::v3 &s)
      : sc(s), trans(pos) {}
  Transformable(const vivaphysics::v3 &pos, const vivaphysics::v3 &rot_axis,
                vivaphysics::real angle, const vivaphysics::v3 &s)
      : sc(s), rot(rot_axis, angle), trans(pos) {}
  void set_rotatable(const Rotatable &rot_) { rot = rot_; }
  glm::mat4 get_view_matrix(const onb &basis) const {
    //
    glm::vec3 pos = trans.position.to_glm();
    glm::vec3 target = pos + basis.front().to_glm();
    glm::vec3 upvec = basis.up().to_glm();
    glm::vec3 look_direction = glm::normalize(pos - target);
    glm::vec3 right = glm::normalize(glm::cross(upvec, look_direction));
    glm::vec3 realUp =
        glm::normalize(glm::cross(look_direction, basis.right().to_glm()));
    //
    glm::mat4 trans(1.0f);
    trans[3][0] = -pos.x;
    trans[3][1] = -pos.y;
    trans[3][2] = -pos.z;

    //
    glm::mat4 rotation(1.0f);
    rotation[0][0] = right.x;
    rotation[1][0] = right.y;
    rotation[2][0] = right.z;
    rotation[0][1] = realUp.x;
    rotation[1][1] = realUp.y;
    rotation[2][1] = realUp.z;
    rotation[0][2] = look_direction.x;
    rotation[1][2] = look_direction.y;
    rotation[2][2] = look_direction.z;
    return rotation * trans;
  }
  void set_scalable(const Scalable &s_) { sc = s_; }
  void set_translatable(const Translatable &t_) { trans = t_; }
  /**
   Obtain model matrix of the transformable object*/
  glm::mat4 model() {
    auto m = glm::mat4(1.0f);
    if (trans.trans_check) {
      m = trans.translate(m);
    }
    if (rot.rot_check) {
      m = rot.rotate(m);
    }
    if (sc.scale_check) {
      m = sc.scale(m);
    }
    return m;
  }
};
}; // namespace vivademos

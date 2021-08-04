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
  glm::mat4 translate() const {
    if (trans_check) {
      auto tmat = glm::translate(position.to_glm());
      return tmat;
    } else {
      throw std::runtime_error("position has not been initialized");
    }
  }
};
struct Rotatable {
  vivaphysics::q4 quat;
  bool rot_check = false;

  // euler angles
  euler_angles angles;
  bool has_yaw = false;
  bool has_pitch = false;
  bool has_roll = false;

  //
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
    auto q_i = unit.x() * sin_theta;
    auto q_j = unit.y() * sin_theta;
    auto q_k = unit.z() * sin_theta;
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
    auto rot = Rotatable(q_r, q_i, q_j, q_k);

    // set given euler angles
    // set yaw
    rot.angles.set_yaw(yaw);
    rot.has_yaw = true;

    // set pitch value
    rot.angles.set_pitch(pitch);
    rot.has_pitch = true;

    // set roll value
    rot.angles.set_roll(roll);
    rot.has_roll = true;
    return rot;
  }
  static Rotatable fromEulerAngles(const vivaphysics::euler_angles &angles) {
    //
    return fromEulerAngles(angles.yaw(), angles.pitch(), angles.roll());
  }
  static Rotatable fromRotationMatrix(const glm::mat3 &m) {
    auto q_r = 0.5 * std::sqrt(1.0 + m[0].x + m[1].y + m[2].z);
    auto q4_r = 1.0 / (4.0 * q_r);
    auto q_i = q4_r * (m[2].y - m[1].z);
    auto q_j = q4_r * (m[0].z - m[2].x);
    auto q_k = q4_r * (m[1].x - m[0].y);
    return Rotatable(q_r, q_i, q_j, q_k);
  }
  vivaphysics::euler_angles to_euler() const {
    // declare euler angles
    vivaphysics::euler_angles eangles;

    // set roll
    if (has_roll == false) {
      auto qri = 2 * (quat.scalar() * quat.x() + quat.y() * quat.z());
      auto qcos = 1 - 2 * (quat.x() * quat.x() + quat.y() * quat.y());
      // compute roll (x-axis rotation) value
      auto roll = std::atan2(qri, qcos);
      eangles.set_roll(roll);
    } else {
      eangles.set_roll(angles.roll());
    }

    //
    if (has_pitch == false) {
      // compute pitch (y axis rotation) value
      auto sinp = 2 * (quat.scalar() * quat.y() - quat.z() * quat.x());
      real pitch;
      if (std::abs(sinp) >= 1) {
        pitch = std::copysign(M_PI / 2, sinp);
      } else {
        pitch = std::asin(sinp);
      }
      eangles.set_pitch(pitch);
    } else {
      eangles.set_pitch(angles.pitch());
    }
    // compute yaw (z axis rotation) value
    if (has_yaw == false) {
      auto sin_yaw = 2 * (quat.scalar() * quat.z() + quat.x() * quat.y());
      auto cos_yaw = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
      auto yaw = std::atan2(sin_yaw, cos_yaw);
      eangles.set_yaw(yaw);
    } else {
      eangles.set_yaw(angles.yaw());
    }
    return eangles;
  }
  void set_euler(vivaphysics::real v, vivaphysics::EULER_ANGLE_TYPE a) {
    switch (a) {
    case vivaphysics::EULER_ANGLE_TYPE::YAW: {
      angles.set_yaw(v);
    }
    case vivaphysics::EULER_ANGLE_TYPE::ROLL: {
      angles.set_roll(v);
    }
    case vivaphysics::EULER_ANGLE_TYPE::PITCH: {
      angles.set_pitch(v);
    }
    }
    *this = Rotatable::fromEulerAngles(angles);
  }
  void set_yaw(vivaphysics::real nyaw) {
    set_euler(nyaw, vivaphysics::EULER_ANGLE_TYPE::YAW);
  }
  void set_pitch(vivaphysics::real np) {
    set_euler(np, vivaphysics::EULER_ANGLE_TYPE::PITCH);
  }
  void set_roll(vivaphysics::real nr) {
    set_euler(nr, vivaphysics::EULER_ANGLE_TYPE::ROLL);
  }
  void rotate_by_euler(ROTATE_DIRECTION direction, vivaphysics::real deltaTime,
                       vivaphysics::real speed) {
    deltaTime *= speed;
    euler_angles eangles = to_euler();
    vivaphysics::real yaw = eangles.yaw();
    vivaphysics::real pitch = eangles.pitch();
    vivaphysics::real roll = eangles.roll();
    switch (direction) {

    case ROTATE_DIRECTION::FORWARD: {
      pitch += deltaTime;
      break;
    }
    case ROTATE_DIRECTION::BACKWARD: {
      pitch -= deltaTime;
      break;
    }
    case ROTATE_DIRECTION::LEFT: {
      yaw -= deltaTime;
      break;
    }
    case ROTATE_DIRECTION::RIGHT: {
      yaw += deltaTime;
      break;
    }
    case ROTATE_DIRECTION::ROTATE_X: {
      roll += deltaTime;
    }
    case ROTATE_DIRECTION::ROTATE_Y: {
      yaw += deltaTime;
    }
    case ROTATE_DIRECTION::ROTATE_Z: {
      pitch += deltaTime;
    }
    case ROTATE_DIRECTION::U_AXIS: {
      pitch += deltaTime;
      yaw += deltaTime;
      roll += deltaTime;
    }
    }
    eangles.set_yaw(yaw);
    eangles.set_pitch(pitch);
    eangles.set_roll(roll);
    *this = Rotatable::fromEulerAngles(eangles);
  }

  /**axis angle representation*/
  glm::vec4 to_angle_axis() const {
    //
    auto q = quat.normalized();
    auto q_w = q.scalar();
    auto q_x = q.x();
    auto q_y = q.y();
    auto q_z = q.z();
    vivaphysics::real coeff = std::sqrt(1.0 - q_w * q_w);
    coeff = coeff < 0.001 ? 0.001 : coeff;
    vivaphysics::real angle = 2.0 * std::acos(q_w);
    vivaphysics::real x = q_x / coeff;
    vivaphysics::real y = q_y / coeff;
    vivaphysics::real z = q_z / coeff;
    glm::vec4 v(x, y, z, angle);
    return v;
  }

  /**to rotation matrix
    from wikipedia

    A = [
        ((1 − 2*q_j^2 − 2*q_k^2), 2(q_i*q_j − q_k*q_r), 2(q_i*q_k + q_j*q_r)),
        (2(q_i*q_j + q_k*q_r), (1 − 2*q_i^2 − 2*q_k^2), 2(q_j*q_k − q_i*q_r))
        (2(q_i*q_k − q_j*q_r), 2(q_j*q_k + q_i*q_r), (1 − 2*q_i^2 − 2*q_j^2))
        ]
   */
  glm::mat3 to_mat() const {
    //
    auto q_r = quat.scalar();
    auto q_i = quat.x();
    auto q_j = quat.y();
    auto q_k = quat.z();
    //
    auto r1_x = 1.0 - 2.0 * (q_j * q_j) - 2.0 * (q_k * q_k);
    auto r1_y = 2.0 * (q_i * q_j - q_k * q_r);
    auto r1_z = 2.0 * (q_i * q_k + q_j * q_r);
    //
    auto r2_x = 2 * (q_i * q_j + q_k * q_r);
    auto r2_y = 1.0 - 2.0 * (q_i * q_i) - 2.0 * (q_k * q_k);
    auto r2_z = 2.0 * (q_j * q_k - q_i * q_r);
    //
    auto r3_x = 2.0 * (q_i * q_k - q_j * q_r);
    auto r3_y = 2.0 * (q_j * q_k + q_i * q_r);
    auto r3_z = 1.0 - 2.0 * (q_i * q_i) - 2.0 * (q_j * q_j);
    //
    glm::mat3 m(1.0);
    //
    m[0].x = r1_x;
    m[0].y = r2_x;
    m[0].z = r3_x;
    //
    m[1].x = r1_y;
    m[1].y = r2_y;
    m[1].z = r3_y;
    //
    m[2].x = r1_z;
    m[2].y = r2_z;
    m[2].z = r3_z;
    //
    return m;
  }
  glm::mat4 rotate(const glm::mat4 &model) const {
    //
    if (rot_check) {
      auto m = to_mat();
      glm::mat4 mat = model * glm::mat4(m);
      return mat;
    } else {
      throw std::runtime_error("rotation values are not initialized");
    }
  }
}; // namespace vivademos
struct Scalable {
  vivaphysics::v3 coeffs;
  bool scale_check = false;
  Scalable() : scale_check(false) {}
  Scalable(const vivaphysics::v3 &cs) : coeffs(cs), scale_check(true) {}
  void set_scale(const vivaphysics::v3 &cs) { coeffs = cs; }
  glm::mat4 scale(const glm::mat4 &m) const {
    if (scale_check) {
      auto smat = glm::scale(coeffs.to_glm());
      auto scaled_matrix = m * smat;
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
      m = trans.translate();
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

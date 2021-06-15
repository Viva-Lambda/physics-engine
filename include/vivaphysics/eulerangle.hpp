#pragma once
#include <external.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>
#include <vivaphysics/vec3.hpp>

using namespace vivaphysics;

namespace vivaphysics {
enum class EULER_ANGLE { YAW, PITCH, ROLL };
struct euler_angles {
  real r = 0.0;
  real y = 0.0;
  real p = 0.0;
  euler_angles() {}
  euler_angles(real r_, real y_, real p_)
      : r(r_), y(y_), p(p_) {}
  real yaw() const { return y; }
  real pitch() const { return p; }
  real roll() const { return r; }
  void set_yaw(real v) { y = v; }
  void set_roll(real v) { r = v; }
  void set_pitch(real v) { p = v; }
  v3 to_vec() const { return v3(r, p, y); }
  static euler_angles from_vec(real roll, real pitch,
                               real yaw) {
    euler_angles angles;
    angles.r = roll;
    angles.p = pitch;
    angles.y = yaw;
    return angles;
  }
};
};

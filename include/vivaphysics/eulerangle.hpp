#pragma once
#include <cstdint>
#include <external.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>
#include <vivaphysics/vec3.hpp>

using namespace vivaphysics;

namespace vivaphysics {
enum class EULER_ANGLE_TYPE : std::uint8_t { YAW, PITCH, ROLL };

struct euler_angle {
  real val;
  real high;
  real low;
  bool is_limited;
  EULER_ANGLE_TYPE type;

  euler_angle()
      : val(0), high(0), low(0), is_limited(false),
        type(EULER_ANGLE_TYPE::PITCH) {}
  euler_angle(real v, EULER_ANGLE_TYPE t)
      : val(v), high(0), low(0), is_limited(false), type(t) {}
  euler_angle(real v, real h, real lo, EULER_ANGLE_TYPE t)
      : val(v), high(h), low(lo), is_limited(true), type(t) {}
  real value() const {
    if (is_limited) {
      if (val < low)
        return low;
      if (val > high)
        return high;
      return val;
    } else {
      return val;
    }
  }
  void set(real v) { val = v; }
};

struct euler_angles {
  euler_angle r;
  euler_angle y;
  euler_angle p;

  euler_angles() {}
  euler_angles(real r_, real y_, real p_)
      : r(r_, EULER_ANGLE_TYPE::ROLL), y(y_, EULER_ANGLE_TYPE::YAW),
        p(p_, 89.0, -89.0, EULER_ANGLE_TYPE::PITCH) {}
  euler_angles(real r_, real rh, real rl, real y_, real yh, real yl, real p_,
               real ph, real pl)
      : r(r_, rh, rl, EULER_ANGLE_TYPE::ROLL),
        y(y_, yh, yl, EULER_ANGLE_TYPE::YAW),
        p(p_, ph, pl, EULER_ANGLE_TYPE::PITCH) {}
  real yaw() const { return y.value(); }
  real pitch() const { return p.value(); }
  real roll() const { return r.value(); }

  void set_yaw(real v) { y.set(v); }
  void set_roll(real v) { r.set(v); }
  void set_pitch(real v) { p.set(v); }

  v3 to_vec() const { return v3(r.value(), p.value(), y.value()); }
  static euler_angles from_vec(real roll, real pitch, real yaw) {
    euler_angles angles(roll, yaw, pitch);
    return angles;
  }
};
}; // namespace vivaphysics

#pragma once
#include <external.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/eulerangle.hpp>
#include <vivaphysics/precision.hpp>
#include <vivaphysics/quaternion.hpp>
#include <vivaphysics/vec3.hpp>

using namespace vivaphysics;

namespace vivaphysics {
struct onb {
  v3 axis[3];
  // components
  onb() {}
  v3 u() const { return axis[0]; }
  v3 v() const { return axis[1]; }
  v3 w() const { return axis[2]; }
  void set_w(const v3 &w_) { axis[2] = w_; }
  void set_v(const v3 &v_) { axis[1] = v_; }
  void set_u(const v3 &u_) { axis[0] = u_; }
  v3 local(real a, real b, real c) const {
    return a * u() + b * v() + c * w();
  }
  v3 local(v3 pos) const {
    return pos.x * u() + pos.y * v() + pos.z * w();
  }
  void from_w(real x, real y, real z) {
    v3 snormal(x, y, z);
    from_w(snormal);
  }
  /**build orthonormal basis from surface normal*/
  void from_w(const v3 &snormal) {
    axis[2] = snormal.normalized();
    v3 a =
        std::fabs(w().x) > 0.9 ? v3(0, 1, 0) : v3(1, 0, 0);
    axis[1] = w().cross_product(a).normalized();
    axis[0] = w().cross_product(v());
  }
  /**
    build orthonormal basis from surface normal, ie front,
    and world up
   */
  void from_w_up(const v3 &snormal, const v3 &up) {
    axis[2] = snormal.normalized();
    axis[1] = w().cross_product(up).normalized();
    axis[0] = w().cross_product(v());
  }
  /**construct onb from euler angles*/
  void from_euler(const euler_angles &angles) {
    real yaw = angles.yaw();
    real pitch = angles.pitch();
    // compute new front
    vivaphysics::v3 front;
    front.x = cos(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    from_w(front_);
  }
  void from_quaternion(const q4 &quat) {}
};
};

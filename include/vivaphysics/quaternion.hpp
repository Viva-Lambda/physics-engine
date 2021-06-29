#pragma once
#include <external.hpp>
#include <thirdparty/quaternion.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>
#include <vivaphysics/vec3.hpp>

using namespace vivaphysics;

namespace vivaphysics {

/**
  \brief Quaternion with real and vector components
  Quaternion is expressed as q = scalar + xi + yj + zk
 */
class q4 : public quat::quaternion<real> {
public:
  //
  /** Taking variable names from Vince 2011 p. 53 */
  //
  q4() : quat::quaternion<real>() {}
  q4(real x, real y, real z, real w)
      : quat::quaternion<real>(x, y, z, w) {}
  q4(real _r, const v3 &_v)
      : quat::quaternion<real>(_r, _v.x, _v.y, _v.z) {}
  q4(real _r, const quat::quat_c<real> &bi,
     const quat::quat_c<real> &cj,
     const quat::quat_c<real> &dk)
      : quat::quaternion<real>(_r, bi, cj, dk){}

  //
  std::array<real, 4> to_arr() const {
    std::array<real, 4> arr = {r(), x(), y(), z()};
    return arr;
  }
  std::vector<real> to_vec() const {
    std::vector<real> vs = {r(), x(), y(), z()};
    return vs;
  }
  v3 vector() const {
    auto v = v3(x(), y(), z());
    return v;
  }
};
};

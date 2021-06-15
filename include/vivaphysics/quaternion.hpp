#pragma once
#include <external.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>
#include <vivaphysics/vec3.hpp>

using namespace vivaphysics;

namespace vivaphysics {

enum class QUATERNION_BASE { I, J, K };

/**
  \brief Quaternion component
 */
struct quat_c {
  QUATERNION_BASE base;
  real r;
  quat_c() {}
  quat_c(QUATERNION_BASE b, real a) : base(b), r(a) {}
};

/**
  \brief Quaternion with real and vector components
  Quaternion is expressed as q = scalar + xi + yj + zk
 */
class q4 {
public:
  //
  /** Taking variable names from Vince 2011 p. 53 */
  real s;
  quat_c x_i;
  quat_c y_j;
  quat_c z_k;
  //
  q4() {}
  q4(real x, real y, real z, real w)
      : s(x), x_i(QUATERNION_BASE::I, y),
        y_j(QUATERNION_BASE::J, z),
        z_k(QUATERNION_BASE::K, w) {}
  q4(real _r, const v3 &_v)
      : s(_r), x_i(QUATERNION_BASE::I, _v.x),
        y_j(QUATERNION_BASE::J, _v.y),
        z_k(QUATERNION_BASE::K, _v.z) {}
  q4(real _r, const quat_c &bi, const quat_c &cj,
     const quat_c &dk)
      : s(_r), x_i(bi), y_j(cj), z_k(dk) {}

  //
  std::array<real, 4> to_arr() const {
    std::array<real, 4> arr = {r(), x(), y(), z()};
    return arr;
  }
  std::vector<real> to_vec() const {
    std::vector<real> vs = {r(), x(), y(), z()};
    return vs;
  }
  real scalar() const { return r(); }
  v3 vector() const { return v3(x(), y(), z()); }
  real r() const { return s; }
  real x() const { return x_i.r; }
  real y() const { return y_j.r; }
  real z() const { return z_k.r; }

  q4 hamilton_product(const q4 &q) const {
    //
    real a1 = r();
    real a2 = q.r();
    real b1 = x();
    real b2 = q.x();
    real c1 = y();
    real c2 = q.y();
    real d1 = z();
    real d2 = q.z();
    auto a12 = a1 * a2;
    auto b12 = b1 * b2;
    auto c12 = c1 * c2;
    auto d12 = d1 * d2;
    auto r_val = a12 - b12 - c12 - d12;
    auto b_val = a1 * b2 + b1 * a2 + c1 * d2 - d1 * c2;
    auto c_val = a1 * c2 - b1 * d2 + c1 * a2 + d1 * b2;
    auto d_val = a1 * d2 + b1 * c2 - c1 * b2 + d1 * a2;
    return q4(r_val, b_val, c_val, d_val);
  }
  /**
   \brief from Vince 2011 - Quaternions for Computer
   Graphics p. 69
  */
  q4 conjugate() const {
    auto a1 = r();
    auto b1 = x();
    auto c1 = y();
    auto d1 = z();
    return q4(a1, -b1, -c1, -d1);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  q4 normalized() const {
    auto inv_mag = static_cast<real>(1.0) / norm();
    return q4(scalar() * inv_mag, vector() * inv_mag);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  q4 inversed() const {
    //
    auto inv_mag2 = static_cast<real>(1.0) / det();
    auto conj = conjugate();
    return q4(conj.scalar() * inv_mag2,
              conj.vector() * inv_mag2);
  }
  /**
   \brief from Vince 2011 - Quaternions for Computer
   Graphics p. 69
   */
  q4 operator+(const q4 &q) const {
    return q4(scalar() + q.scalar(), q.vector() + vector());
  }
  /**
   \brief from Vince 2011 - Quaternions for Computer
   Graphics p. 69
  */
  q4 operator-(const q4 &q) const {
    return q4(scalar() - q.scalar(), vector() - q.vector());
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  q4 operator*(const q4 &q) const {
    auto r_mult = scalar() * q.scalar();
    auto v_dot = vector().inner_product(q.vector());
    auto r_scalar = r_mult - v_dot;
    auto r1v2 = scalar() * q.vector();
    auto r2v1 = q.scalar() * vector();
    auto v1xv2 = vector().cross_product(q.vector());
    return q4(r_scalar, r1v2 + r2v1 + v1xv2);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  q4 operator*(real r) const {
    return q4(r * scalar(), vector() * r);
  }
  q4 operator/(real r) const {
    auto a = static_cast<real>(1.0) / r;
    return q4(a * scalar(), vector() * a);
  }

  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  real norm() const { return std::sqrt(det()); }

  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 25
   */
  real determinant() const {
    auto a2 = r() * r();
    auto b2 = x() * x();
    auto c2 = y() * y();
    auto d2 = z() * z();
    return a2 + b2 + c2 + d2;
  }
  real det() const { return determinant(); }
  real magnitude() const { return norm(); }
  friend std::ostream &operator<<(std::ostream &out,
                                  const q4 &q);
};
std::ostream &operator<<(std::ostream &out, const q4 &q) {
  out << q.r() << " + " << q.x() << "i"
      << " + " << q.y() << "j"
      << " + " << q.z() << "k" << std::endl;
  return out;
}
};

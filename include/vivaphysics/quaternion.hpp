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
 */
class q4 {
public:
  real r;
  //
  quat_c b_i;
  quat_c c_j;
  quat_c d_k;
  //
  q4() {}
  q4(real x, real y, real z, real w)
      : r(x), b_i(QUATERNION_BASE::I, y),
        c_j(QUATERNION_BASE::J, z),
        d_k(QUATERNION_BASE::K, w) {}
  q4(real _r, const v3 &_v)
      : r(_r), b_i(QUATERNION_BASE::I, _v.x),
        c_j(QUATERNION_BASE::J, _v.y),
        d_k(QUATERNION_BASE::K, _v.z) {}
  q4(real _r, const quat_c &bi,
     const quat_c &cj const quat_c &dk)
      : r(_r), b_i(bi), c_j(cj), d_k(dk) {}

  //
  std::array<real, 4> to_arr() const {
    std::array<real, 4> arr = {a(), b_i(), c_j(), d_k()};
    return arr;
  }
  std::vector<real> to_vec() const {
    std::vector<real> vs = {a(), b_i(), c_j(), d_k()};
    return vs;
  }
  real scalar() const { return a(); }
  v3 vector() const { return v3(b(), c(), d()); }
  real a() const { return r; }
  real b() const { return b_i.r; }
  real c() const { return c_j.r; }
  real d() const { return d_k.r; }

  q4 hamilton_product(const q4 &q) const {
    //
    real a1 = a();
    real a2 = q.a();
    real b1 = b();
    real b2 = q.b();
    real c1 = c();
    real c2 = q.c();
    real d1 = d();
    real d2 = q.d();
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
    auto a1 = a();
    auto b1 = b();
    auto c1 = c();
    auto d1 = d();
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
    auto a2 = a() * a();
    auto b2 = b() * b();
    auto c2 = c() * c();
    auto d2 = d() * d();
    return a2 + b2 + c2 + d2;
  }
  real det() const { return determinant(); }
  real magnitude() const { return norm(); }
  friend std::cout &operator<<(std::cout &out, const q4 &q);
};
std::cout &operator<<(std::cout &out, const q4 &q) {
  out << q.a() << " + " << q.b() << "i"
      << " + " << q.c() << "j"
      << " + " << q.d() << "k" << std::endl;
  return out;
}
};

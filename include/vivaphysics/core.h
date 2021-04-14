// core math lib
#pragma once

#include <external.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>

using namespace vivaphysics;

namespace vivaphysics {

class v3 : public glm::vec3 {
public:
  v3() : glm::vec3() {}
  v3(real v) : glm::vec3(v) {}
  v3(real r, real g, real b) : glm::vec3(r, g, b) {}
  v3(glm::vec3 v) : glm::vec3(v.x, v.y, v.z) {}
  glm::vec3 to_glm() const { return glm::vec3(x, y, z); }
  /**operators*/
  v3 operator+(const v3 &v) const {
    return v3(to_glm() + v.to_glm());
  }
  v3 operator+(real v) const { return v3(to_glm() + v); }

  void operator+=(const v3 &v) {
    auto a = to_glm() + v.to_glm();
    *this = v3(a);
  }
  void operator+=(real v) {
    auto a = to_glm() + v;
    *this = v3(a);
  }

  v3 operator-(const v3 &v) const {
    return v3(to_glm() - v.to_glm());
  }
  v3 operator-(real v) const { return v3(to_glm() - v); }

  void operator-=(const v3 &v) {
    auto a = to_glm() - v.to_glm();
    *this = v3(a);
  }
  void operator-=(real v) {
    auto a = to_glm() - v;
    *this = v3(a);
  }

  v3 operator*(const v3 &v) const {
    return v3(to_glm() * v.to_glm());
  }
  v3 operator*(real v) const { return v3(to_glm() * v); }

  void operator*=(const v3 &v) {
    auto a = to_glm() * v.to_glm();
    *this = v3(a);
  }
  void operator*=(real v) {
    auto a = to_glm() * v;
    *this = v3(a);
  }

  v3 cross_product(const v3 &v) const {
    return v3(glm::cross(to_glm(), v.to_glm()));
  }
  v3 vector_product(const v3 &v) const {
    return cross_product(v);
  }
  real dot(const v3 &v) const {
    return static_cast<real>(
        glm::dot(to_glm(), v.to_glm()));
  }
  real inner_product(const v3 &v) { return dot(v); }
  real scalar_product(const v3 &v) { return dot(v); }

  void add_scaled_vector(const v3 &v, real s) {
    auto vs = v.to_glm() * s;
    auto gv = to_glm();
    gv += vs;
    *this = v3(gv);
  }

  /**\brief if positive to negative if negative to
   * positive*/
  void invert() { *this = v3(to_glm() * -1.0f); }

  /** magnitude/size of vector*/
  real magnitude() const {
    return static_cast<real>(glm::length(to_glm()));
  }
  real length() const { return magnitude(); }

  /***/
  real square_magnitude() const {
    return magnitude() * magnitude();
  }
  v3 normalized() const {
    real size = magnitude();
    if (size > 0) {
      auto v = to_glm();
      v /= size;
      return v3(v);
    }
    return v3(0.0);
  }
  v3 unit() const { return normalized(); }
  void normalize() { *this = v3(unit()); }

  /** limit size of the vector to size */
  v3 trimmed(real size) const {
    if (square_magnitude() > size * size) {
      auto vec = normalized();
      vec = vec.to_glm() * size;
      return vec;
    } else {
      return *this;
    }
  }
  void trim(real size) { *this = trimmed(size); }
  void clear() { *this = v3(0.0f); }
};

typedef v3 point3;
typedef v3 color;

void make_orthonormal_basis(const v3 &a, const v3 &b,
                            v3 out[3]) {
  auto anorm = a.normalized();

  out[0] = anorm;
  auto c = anorm.cross_product(b);
  auto csqr = v3(c).square_magnitude();
  COMP_CHECK_MSG(csqr == 0.0, csqr, 0.0,
                 "the magnitude of csqr should not be 0");
  auto cnorm = c.normalized();
  out[1] = cnorm;
  out[2] = cnorm.cross_product(anorm);
}
};

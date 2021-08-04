#pragma once

#include <external.hpp>
#include <ostream>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>

using namespace vivaphysics;

// n dimensional vector lib

namespace vivaphysics {

class v3 {
  vepp::VecN<real, 3> vdata;

  // private methods

  real get(unsigned int i) const {
    real v = static_cast<real>(0);
    vdata.get(i, v);
    return v;
  }
  // static methods
public:
  static const v3 GRAVITY;
  static const v3 HIGH_GRAVITY;
  static const v3 UP;
  static const v3 RIGHT;
  static const v3 OUT_OF_SCREEN;
  static const v3 X;
  static const v3 Y;
  static const v3 Z;

public:
  v3() {}
  v3(real v) : vdata(v) {}
  v3(real r, real g, real b) : vdata(std::array<real, 3>({r, g, b})) {}
  v3(glm::vec3 v) : vdata(std::array<real, 3>({v.x, v.y, v.z})) {}
  real x() const { return get(0); }
  real y() const { return get(1); }
  real z() const { return get(2); }
  glm::vec3 to_glm() const { return glm::vec3(x(), y(), z()); }
  /**operators*/
  bool operator==(const v3 &v) const {
    if (to_glm() == v.to_glm())
      return true;
    return false;
  }
  bool operator!=(const v3 &v) const { return !(*this == v); }
  v3 operator+(const v3 &v) const { return v3(to_glm() + v.to_glm()); }
  v3 operator+(real v) const { return v3(to_glm() + v); }

  v3 &operator+=(const v3 &v) {
    auto a = to_glm() + v.to_glm();
    *this = v3(a);
    return *this;
  }
  v3 &operator+=(real v) {
    auto a = to_glm() + v;
    *this = v3(a);
    return *this;
  }

  v3 operator-(const v3 &v) const { return v3(to_glm() - v.to_glm()); }
  v3 operator-(real v) const { return v3(to_glm() - v); }

  v3 &operator-=(const v3 &v) {
    auto a = to_glm() - v.to_glm();
    *this = v3(a);
    return *this;
  }
  v3 &operator-=(real v) {
    auto a = to_glm() - v;
    *this = v3(a);
    return *this;
  }

  v3 operator*(const v3 &v) const { return v3(to_glm() * v.to_glm()); }
  v3 operator*(real v) const { return v3(to_glm() * v); }

  v3 &operator*=(const v3 &v) {
    auto a = to_glm() * v.to_glm();
    *this = v3(a);
    return *this;
  }
  v3 &operator*=(real v) {
    auto a = to_glm() * v;
    *this = v3(a);
    return *this;
  }

  v3 cross_product(const v3 &v) const {
    return v3(glm::cross(to_glm(), v.to_glm()));
  }
  v3 vector_product(const v3 &v) const { return cross_product(v); }
  real dot(const v3 &v) const {
    return static_cast<real>(glm::dot(to_glm(), v.to_glm()));
  }
  real inner_product(const v3 &v) const { return dot(v); }
  real scalar_product(const v3 &v) const { return dot(v); }

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
  real magnitude() const { return static_cast<real>(glm::length(to_glm())); }
  real length() const { return magnitude(); }

  /***/
  real square_magnitude() const { return magnitude() * magnitude(); }
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

  friend std::ostream &operator<<(std::ostream &out, const v3 &v);
};

std::ostream &operator<<(std::ostream &out, const v3 &v) {
  out << v.x() << " " << v.y() << " " << v.z();
  return out;
}

const v3 v3::GRAVITY = v3(0, -9.81, 0);
const v3 v3::HIGH_GRAVITY = v3(0, -20.62, 0);
const v3 v3::UP = v3(0, 1, 0);
const v3 v3::RIGHT = v3(1, 0, 0);
const v3 v3::OUT_OF_SCREEN = v3(0, 0, 1);
const v3 v3::X = v3(0, 1, 0);
const v3 v3::Y = v3(1, 0, 0);
const v3 v3::Z = v3(0, 0, 1);

typedef v3 point3;
typedef v3 color;

void make_orthonormal_basis(const v3 &a, const v3 &b, v3 out[3]) {
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
}; // namespace vivaphysics

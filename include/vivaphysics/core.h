// core math lib
#pragma once

#include <external.hpp>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>

using namespace vivaphysics;

namespace vivaphysics {

template <class VType> class Vector {
public:
  unsigned int nb_components;
  VType *components;
  Vector() {}
  ~Vector() { delete[] components; }
  Vector(const std::vector<VType> &cs)
      : nb_components(
            static_cast<unsigned int>(cs.size())) {
    components = new real[nb_components];
    for (unsigned int i = 0; i < nb_components; i++) {
      components[i] = cs[i];
    }
  }
  Vector(VType *cs, unsigned int nb)
      : nb_components(nb), components(cs) {}

  // operators defined per vector
  /** getter for component vector */
  VType operator[](unsigned int i) const {
    D_CHECK_MSG(i >= nb_components,
                "argument out of bounds");
    return components[i];
  }

  /** setter for component vector */
  VType &operator[](unsigned int i) {
    D_CHECK_MSG(i >= nb_components,
                "argument out of bounds");
    return components[i];
  }
  /** generic scalar transformation function*/
  Vector real_transform(
      const std::function<VType(VType)> &f) const {
    auto tv = Vector(components, nb_components);
    for (unsigned int i = 0; i < nb_components; i++) {
      real vec_value = tv[i];
      tv[i] = f(vec_value);
    }
    return tv;
  }

  void
  real_transformed(const std::function<VType(VType)> &f) {
    auto tv = real_transform(f);
    *this = tv;
  }
  Vector
  vec_transform(const std::function<VType(VType, VType)> &f,
                const Vector &v) const {
    auto tv = Vector(components, v.nb_components);
    for (unsigned int i = 0; i < nb_components; i++) {
      real vec_value = tv[i];
      tv[i] = f(vec_value, v[i]);
    }
    return tv;
  }

  void vec_transformed(
      const std::function<VType(VType, VType)> &f,
      const Vector &v) {
    auto tv = vec_transform(f, v);
    *this = tv;
  }

  /**
    \brief apply boolean operator
   */
  bool bapply(const std::function<bool(VType, VType)> &f,
              const Vector &v) const {
    D_CHECK_MSG(v.nb_components != nb_components,
                "argument size not compatible");
    for (unsigned int i = 0; i < nb_components; i++) {
      if (!f(v[i], components[i])) {
        return false;
      }
    }
    return true;
  }

  bool operator==(const Vector<VType> &v) const {
    return bapply([](VType i, VType j) { return i == j; },
                  v);
  }
  bool operator!=(const Vector<VType> &v) const {
    return !(*this == v);
  }
  bool operator<(const Vector<VType> &v) {
    return bapply([](VType i, VType j) { return i < j; },
                  v);
  }
  bool operator<=(const Vector<VType> &v) {
    return bapply([](VType i, VType j) { return i < j; },
                  v);
  }
  bool operator>(const Vector<VType> &v) {
    return bapply([](VType i, VType j) { return i < j; },
                  v);
  }
  bool operator>=(const Vector<VType> &v) {
    return bapply([](VType i, VType j) { return i < j; },
                  v);
  }
  void to_zero() {
    for (unsigned int i = 0; i < nb_components; i++) {
      components[i] = 0;
    }
  }
  void clear() {
    components = nullptr;
    nb_components = 0;
  }
};

class v3 : public glm::vec3 {
public:
  v3() : glm::vec3() {}
  v3(real v) : glm::vec3(v) {}
  v3(real r, real g, real b) : glm::vec3(r, g, b) {}
  v3(glm::vec3 v) : glm::vec3(v.x, v.y, v.z) {}
  glm::vec3 to_glm() const { return glm::vec3(x, y, z); }
  glm::vec3 cross_product(const v3 &v) const {
    return glm::cross(to_glm(), v.to_glm());
  }
  glm::vec3 vector_product(const v3 &v) const {
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

class Vector3 : public Vector<real> {

private:
  /** padding that might be required for libs */
  real pad;

public:
  Vector3()
      : Vector<real>(std::vector<real>({0.0, 0.0, 0.0})) {}
  Vector3(const real _x, const real _y, const real _z)
      : Vector<real>(std::vector<real>({_x, _y, _z})) {}
  Vector3(const real _x)
      : Vector<real>(std::vector<real>({_x, _x, _x})) {}
  Vector3(const Vector<real> &v)
      : Vector<real>(
            std::vector<real>({v[0], v[1], v[2]})) {}

  real x() const { return components[0]; }
  real y() const { return components[1]; }
  real z() const { return components[2]; }
  glm::vec3 to_glm() const {
    auto v = glm::vec3(x(), y(), z());
    return v;
  }

  // ----------- operators start ----------------
  /** sum vector components with given value */
  void operator+=(const Vector3 &v) {
    *this = Vector3(vec_transform(
        [](real i, real j) { return i + j; }, v));
  }
  void operator+=(const real v) {
    *this = Vector3(
        real_transform([v](real i) { return i + v; }));
  }
  /** sum vector components with given value */
  Vector3 operator+(const Vector3 &v) const {
    return Vector3(vec_transform(
        [](real i, real j) { return i + j; }, v));
  }
  Vector3 operator+(const real v) const {
    return Vector3(
        real_transform([v](real i) { return v + i; }));
  }

  /** multiply vector components with given value */
  void operator*=(const real v) {
    *this = Vector3(
        real_transform([v](real i) { return i * v; }));
  }
  Vector3 operator*(const real v) const {
    return Vector3(
        real_transform([v](real i) { return i * v; }));
  }
  real operator*(const Vector3 &v) const { return dot(v); }
  friend Vector3 operator*(const real n,
                           const Vector3 &v3) {
    return v3 * n;
  }

  /** subtract vector components with given value */
  void operator-=(const real v) {
    *this = Vector3(
        real_transform([v](real i) { return i - v; }));
  }
  Vector3 operator-(const real v) const {
    return Vector3(
        real_transform([v](real i) { return i - v; }));
  }
  Vector3 operator-(const Vector3 &v) const {
    return Vector3(vec_transform(
        [](real i, real j) { return i - j; }, v));
  }
  /** divide vector components with given value */
  void operator/=(const real v) {

    D_CHECK_MSG(v == 0, "zero division");
    *this = Vector3(
        real_transform([v](real i) { return i / v; }));
  }
  Vector3 operator/(const real v) const {
    D_CHECK_MSG(v == 0, "zero division");
    return Vector3(
        real_transform([v](real i) { return i / v; }));
  }
  /** special cross product operator */
  Vector3 operator%(const Vector3 &v) const {
    return cross_product(v);
  }
  void operator%=(const Vector3 &v) {
    *this = cross_product(v);
  }
  // ------------ operators end ------------

  /** component products*/
  Vector3 component_product(const Vector3 &v) const {
    return Vector3(vec_transform(
        [](real i, real j) { return i * j; }, v));
  }
  void component_product_update(const Vector3 &v) {
    *this = Vector3(vec_transform(
        [](real i, real j) { return i * j; }, v));
  }
  Vector3 cross_product(const Vector3 &v) const {
    return Vector3(y() * v.z() - z() * v.y(),
                   z() * v.x() - x() * v.z(),
                   x() * v.y() - y() * v.x());
  }
  Vector3 vector_product(const Vector3 &v) const {
    return cross_product(v);
  }
  real dot(const Vector3 &v) const {
    return x() * v.x() + y() * v.y() + z() * v.z();
  }
  real inner_product(const Vector3 &v) { return dot(v); }
  real scalar_product(const Vector3 &v) { return dot(v); }

  void add_scaled_vector(const Vector3 &v, real s) {
    auto vs = v * s;
    *this += vs;
  }

  /**\brief if positive to negative if negative to
   * positive*/
  void invert() {
    *this = Vector3(
        real_transform([](real i) { return i * -1; }));
  }

  /** magnitude/size of vector*/
  real magnitude() const {
    return static_cast<real>(
        sqrt(x() * x() + y() * y() + z() * z()));
  }
  real length() const { return magnitude(); }

  /***/
  real square_magnitude() const {
    return x() * x() + y() * y() + z() * z();
  }
  Vector3 normalized() const {
    real size = magnitude();
    if (size > 0) {
      return *this / size;
    }
    return Vector3(0.0);
  }
  Vector3 unit() const { return normalized(); }
  void normalize() { *this /= magnitude(); }

  /** limit size of the vector to size */
  Vector3 trimmed(real size) const {
    if (square_magnitude() > size * size) {
      auto vec = normalized();
      vec *= size;
      return vec;
    } else {
      return *this;
    }
  }
  void trim(real size) { *this = trimmed(size); }
};
void make_orthonormal_basis(const Vector3 &a,
                            const Vector3 &b,
                            Vector3 out[3]) {
  auto anorm = a.normalized();
  out[0] = anorm;
  auto c = anorm.cross_product(b);
  auto csqr = c.square_magnitude();
  COMP_CHECK_MSG(csqr == 0.0, csqr, 0.0,
                 "the magnitude of csqr should not be 0");
  auto cnorm = c.normalized();
  out[1] = cnorm;
  out[2] = cnorm.cross_product(anorm);
}
};

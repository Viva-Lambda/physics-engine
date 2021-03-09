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
  void clean() {
    components = nullptr;
    nb_components = 0;
  }
};

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
  real normalized() const {
    real size = magnitude();
    if (size > 0) {
      return static_cast<real>(1 / size);
    }
    return 0.0;
  }
};
};

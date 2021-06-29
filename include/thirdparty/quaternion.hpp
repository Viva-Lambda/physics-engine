/*
MIT License
Copyright (c) 2021 Viva Lambda email
<76657254+Viva-Lambda@users.noreply.github.com>
Permission is hereby granted, free of charge, to any person
obtaining a copy
of this software and associated documentation files (the
"Software"), to deal
in the Software without restriction, including without
limitation the rights
to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall
be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef QUATERNION_HPP
#define QUATERNION_HPP
#include <math.h>
#include <ostream>
#include <stdio.h>

namespace quat {
// holds quaternion related operations

/**Quaternion bases*/
enum QUATERNION_BASE {
  SCALAR_BASE, // null value for quaternion base it has no
               // effect on computation
  I,           // i base for the second quaternion component
  J,           // j base for the third quaternion component
  K            // k base for the fourth quaternion component
};

/**
  \brief Quaternion component
 */
template <class T> struct quat_c {
  QUATERNION_BASE base;
  T r;
  quat_c() {}
  quat_c(QUATERNION_BASE b, T a) : base(b), r(a) {}
  friend std::ostream &operator<<(std::ostream &out,
                                  const quat_c<T> &c);
};

template <typename T>
std::ostream &operator<<(std::ostream &out,
                         const quat_c<T> &c) {
  switch (c.base) {
  case SCALAR_BASE: {
    out << "SCALAR_BASE::" << c.r << std::endl;
  }
  case I: {
    out << "I_BASE::" << c.r << std::endl;
  }
  case J: {
    out << "J_BASE::" << c.r << std::endl;
  }
  case K: {
    out << "K_BASE::" << c.r << std::endl;
  }
  }
  return out;
}

template <class T> class quaternion {
public:
  quaternion() {
    coeffs[0] = static_cast<T>(0);
    coeffs[1] = static_cast<T>(1);
    coeffs[2] = static_cast<T>(1);
    coeffs[3] = static_cast<T>(1);
  }
  quaternion(T x, T y, T z, T w) {
    coeffs[0] = x;
    coeffs[1] = y;
    coeffs[2] = z;
    coeffs[3] = w;
  }
  quaternion(const T c[4]) {
    for (unsigned int i = 0; i < 4; i++) {
      coeffs[i] = c[i];
    }
  }
  quaternion(T c1, const quat_c<T> &qc2,
             const quat_c<T> &qc3, const quat_c<T> &qc4) {
    coeffs[0] = c1;
    coeffs[1] = qc2.r;
    coeffs[2] = qc3.r;
    coeffs[3] = qc4.r;
  }

  quaternion(const quat_c<T> &c1, const quat_c<T> &qc2,
             const quat_c<T> &qc3, const quat_c<T> &qc4) {
    coeffs[0] = c1.r;
    coeffs[1] = qc2.r;
    coeffs[2] = qc3.r;
    coeffs[3] = qc4.r;
  }
  quaternion(T c1, T cs[3]) {
    coeffs[0] = c1;
    coeffs[1] = cs[0];
    coeffs[2] = cs[1];
    coeffs[3] = cs[2];
  }
  T scalar() const { return coeffs[0]; }
  void vector(T v[3]) const {
    v[0] = x();
    v[1] = y();
    v[2] = z();
  }
  bool vector_multiplication(T t) {
    coeffs[1] *= t;
    coeffs[2] *= t;
    coeffs[3] *= t;
    return true;
  }
  bool vector_addition(T t) {
    coeffs[1] += t;
    coeffs[2] += t;
    coeffs[3] += t;
    return true;
  }
  bool vector_subtraction(T t) {
    coeffs[1] -= t;
    coeffs[2] -= t;
    coeffs[3] -= t;
    return true;
  }
  bool vector_division(T t) {
    if (t == 0)
      return false;
    coeffs[1] /= t;
    coeffs[2] /= t;
    coeffs[3] /= t;
    return true;
  }
  /** arithmetic operations with a scalar on vector part*/
  bool vector_multiplication(T v[3], T t) const {
    v[0] = x() * t;
    v[1] = y() * t;
    v[2] = z() * t;
    return true;
  }
  bool vector_addition(T v[3], T t) const {
    v[0] = x() + t;
    v[1] = y() + t;
    v[2] = z() + t;
    return true;
  }
  bool vector_subtraction(T v[3], T t) const {
    v[0] = x() - t;
    v[1] = y() - t;
    v[2] = z() - t;
    return true;
  }
  bool vector_division(T v[3], T t) const {
    if (t == 0)
      return false;
    v[0] = x() / t;
    v[1] = y() / t;
    v[2] = z() / t;
    return true;
  }
  /** arithmetic operations with a vector on vector part*/
  bool vector_multiplication(T v[3], T t[3]) const {
    v[0] = x() * t[0];
    v[1] = y() * t[1];
    v[2] = z() * t[2];
    return true;
  }
  bool vector_addition(T v[3], T t[3]) const {
    v[0] = x() + t[0];
    v[1] = y() + t[1];
    v[2] = z() + t[2];
    return true;
  }
  bool vector_subtraction(T v[3], T t[3]) const {
    v[0] = x() - t[0];
    v[1] = y() - t[1];
    v[2] = z() - t[2];
    return true;
  }
  bool vector_division(T v[3], T t[3]) const {
    for (unsigned int i = 0; i < 3; i++) {
      if (t[i] == 0)
        return false;
    }
    v[0] = x() / t[0];
    v[1] = y() / t[1];
    v[2] = z() / t[2];
    return true;
  }
  /** dot product and cross product for two vec3*/
  T vector_dot(T t[3]) const {
    return t[0] * x() + t[1] * y() + t[2] * z();
  }
  T vector_dot(T v[3], T t[3]) const {
    return t[0] * v[0] + t[1] * v[1] + t[2] * v[2];
  }
  bool vector_cross(T out[3], T t[3]) const {
    out[0] = y() * t[2] - z() * t[1];
    out[1] = z() * t[0] - x() * t[2];
    out[2] = x() * t[1] - y() * t[0];
    return true;
  }

  T r() const { return coeffs[0]; }
  T x() const { return coeffs[1]; }
  T y() const { return coeffs[2]; }
  T z() const { return coeffs[3]; }

  /** Quaternion product as it is shown by Vince 2011 p.
   * 63
   Given a quaternion \f[q_a = [s_a, a]\f] and
   another quaternion \f[q_b = [s_b, b]\f]
   Their product is equal to:
   \f[s_a s_b - a \cdot b, s_a b + s_b a + a \times b \f]
   */
  quaternion hamilton_product(const quaternion &q_b) const {
    // s_a, s_b, a, b
    T s_a = scalar();
    T s_b = q_b.scalar();
    T a[3];
    vector(a);
    T b[3];
    q_b.vector(b);

    // s_a * s_b
    T s_ab = s_a * s_b;

    // a \cdot b
    T a_dot_b = vector_dot(a, b);

    // a \times b
    T cross_ab[3];
    vector_cross(cross_ab, b);

    // s_a * b + s_b * a + a \times b
    T out[3];
    for (unsigned int i = 0; i < 3; i++) {
      out[i] = s_a * b[i] + s_b * a[i] + cross_ab[i];
    }
    return quaternion(s_ab - a_dot_b, out);
  }
  quaternion conjugate() const {
    T a1 = r();
    T b1 = x() * -1;
    T c1 = y() * -1;
    T d1 = z() * -1;
    return quaternion(a1, b1, c1, d1);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  quaternion normalized() const {
    T inv_mag = static_cast<T>(1.0) / norm();
    T scalar_part = scalar() * inv_mag;
    T vs[3];
    vector(vs);
    vector_multiplication(vs, inv_mag);
    return quaternion(scalar_part, vs);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  quaternion inversed() const {
    //
    T inv_mag2 = static_cast<T>(1.0) / det();
    quaternion conj = conjugate();
    T spart = conj.scalar() * inv_mag2;
    T vs[3];
    vector_multiplication(vs, inv_mag2);
    return quaternion(spart, vs);
  }
  /**
   \brief from Vince 2011 - Quaternions for Computer
   Graphics p. 69
   */
  quaternion operator+(const quaternion &q) const {
    T out[3];
    T vs[3];
    q.vector(vs);
    vector_addition(out, vs);
    return quaternion(scalar() + q.scalar(), out);
  }
  /**
   \brief from Vince 2011 - Quaternions for Computer
   Graphics p. 69
  */
  quaternion operator-(const quaternion &q) const {
    T out[3];
    T vs[3];
    q.vector(vs);
    vector_subtraction(out, vs);
    return quaternion(scalar() - q.scalar(), out);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  quaternion operator*(const quaternion &q) const {
    return hamilton_product(q);
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  quaternion operator*(T r) const {
    T out[3];
    vector_multiplication(out, r);
    return quaternion(scalar() * r, out);
  }
  quaternion power(unsigned int i) const {
    quaternion accumulant = *this;
    quaternion result2 = *this;
    for (unsigned int j = 1; j < i; j++) {
      accumulant = accumulant.hamilton_product(result2);
    }
    return accumulant;
  }
  quaternion squared() const {
    quaternion r1 = *this;
    quaternion r2 = *this;
    return r1 * r2;
  }
  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 69
   */
  T norm() const { return sqrt(det()); }

  /**
    \brief from Vince 2011 - Quaternions for Computer
    Graphics p. 25
   */
  T determinant() const {
    T a2 = r() * r();
    T b2 = x() * x();
    T c2 = y() * y();
    T d2 = z() * z();
    return a2 + b2 + c2 + d2;
  }
  T det() const { return determinant(); }
  T magnitude() const { return norm(); }
  friend std::ostream &operator<<(std::ostream &out,
                                  const quaternion &q);

  bool get_component(std::size_t i, quat_c<T> &c) const {
    if (i == 0) {
      quat_c<T> c_;
      c_.r = coeffs[0];
      c = quat_c(SCALAR_BASE, coeffs[0]);
      return true;
    } else if (i == 1) {
      c = quat_c(I, coeffs[1]);
      return true;
    } else if (i == 2) {
      c = quat_c(J, coeffs[2]);
      return true;
    } else if (i == 3) {
      c = quat_c(K, coeffs[3]);
      return true;
    } else {
      return false;
    }
  }

private:
  T coeffs[4];
};

template <typename T>
std::ostream &operator<<(std::ostream &out,
                         const quaternion<T> &q) {
  out << q.r() << " + " << q.x() << "i"
      << " + " << q.y() << "j"
      << " + " << q.z() << "k" << std::endl;
  return out;
}
};

#endif

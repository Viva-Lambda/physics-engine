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

#ifndef VEPP_HPP
#define VEPP_HPP
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <vector>

namespace vepp {

enum status_t : std::uint8_t {
  SUCCESS = 1,
  SIZE_ERROR = 2,
  INDEX_ERROR = 3,
  ARG_ERROR = 4,
  NOT_CALLED = 5,
  NOT_IMPLEMENTED = 6
};

/** VecN operator flags*/
struct Result {
  status_t status = NOT_CALLED;
  unsigned int line_info = 0;
  std::string file_name = "";
  std::string fn_name = "";
  std::string call_name = "";

  bool success = false;

  Result() {}
  Result(unsigned int l, const std::string &f, const std::string &fn,
         status_t s)
      : status(s), line_info(l), file_name(f), fn_name(fn),
        success(s == SUCCESS) {}
  Result(unsigned int l, const char *f, const char *fn, status_t s)
      : status(s), line_info(l), file_name(f), fn_name(fn),
        success(s == SUCCESS) {}
};

std::ostream &operator<<(std::ostream &out, const Result &flag) {
  switch (flag.status) {
  case SUCCESS: {
    out << "SUCCESS";
    break;
  }
  case INDEX_ERROR: {
    out << "INDEX_ERROR";
    break;
  }
  case ARG_ERROR: {
    out << "ARG_ERROR";
    break;
  }
  case SIZE_ERROR: {
    out << "SIZE_ERROR";
    break;
  }
  case NOT_CALLED: {
    out << "NOT_CALLED";
    break;
  }
  case NOT_IMPLEMENTED: {
    out << "NOT_IMPLEMENTED";
    break;
  }
  }
  return out;
}

template <class T, unsigned int N> class VecN {
  /** holds the vector data*/
  std::array<T, N> data;

public:
  /*! Tested */
  VecN() {}
  /*! Tested */
  VecN(const std::vector<T> &vd) {
    int nb_s = vd.size() - N;
    if (nb_s > 0) {
      // vector size is bigger than current vector
      for (unsigned int i = 0; i < N; i++) {
        data[i] = vd[i];
      }
    } else {
      // vector size is smaller than current vector
      for (unsigned int i = 0; i < N; i++) {
        if (i < vd.size()) {
          set(i, vd[i]);
        } else {
          set(i, static_cast<T>(0));
        }
      }
    }
  } /*! Tested */
  VecN(const std::array<T, N> &arr) : data(arr) {}
  VecN(T s) {
    for (unsigned int i = 0; i < N; i++) {
      data[i] = static_cast<T>(s);
    }
  }
  /*! Tested */
  Result size(unsigned int &out) const {
    out = static_cast<unsigned int>(data.size());
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  /*! Tested */
  Result get(unsigned int index, T &out) const {
    if (index >= data.size()) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, INDEX_ERROR);
      return vflag;
    }
    out = data[index];
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result get(std::array<T, N> &out) const {
    out = data;
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  /*! Tested */
  Result set(unsigned int index, T el) {
    if (index >= data.size()) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, INDEX_ERROR);
      return vflag;
    }
    data[index] = el;

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  /*! Tested */
  static Result base(unsigned int nb_dimensions, unsigned int base_order,
                     std::vector<T> &out) {
    if (base_order >= nb_dimensions) {

      Result vflag(__LINE__, __FILE__, __FUNCTION__, ARG_ERROR);
      return vflag;
    }
    //
    if (out.size() != nb_dimensions) {
      out.clear();
      out.resize(static_cast<std::size_t>(nb_dimensions));
    }
    for (unsigned int i = 0; i < nb_dimensions; i++) {
      out[i] = static_cast<T>(0);
    }
    out[base_order] = static_cast<T>(1);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  /*! Tested */
  static Result base(unsigned int base_order, VecN<T, N> &vout) {
    std::vector<T> out;
    Result result =
        VecN<T, N>::base(static_cast<unsigned int>(N), base_order, out);
    if (result.status != SUCCESS)
      return result;
    vout = VecN<T, N>(out);
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result apply_el(T v, const std::function<T(T, T)> &fn,
                  std::vector<T> &out) const {
    if (out.size() != data.size()) {
      out.resize(data.size());
    }
    for (unsigned int i = 0; i < data.size(); i++) {
      out[i] = fn(data[i], v);
    }
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result apply_el(const std::vector<T> &v, const std::function<T(T, T)> &fn,
                  std::vector<T> &out) const {
    if (v.size() != data.size()) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, SIZE_ERROR);
      return vflag;
    }
    if (data.size() != out.size()) {
      out.resize(data.size());
    }
    for (unsigned int i = 0; i < data.size(); i++) {
      out[i] = fn(data[i], v[i]);
    }
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result apply_el(T v, const std::function<T(T, T)> &fn,
                  VecN<T, N> &vout) const {
    std::vector<T> out;
    Result result = apply_el(v, fn, out);
    if (result.status != SUCCESS) {
      return result;
    }
    vout = VecN<T, N>(out);
    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result apply_el(const VecN<T, N> &v, const std::function<T(T, T)> &fn,
                  VecN<T, N> &vout) const {

    for (unsigned int i = 0; i < data.size(); i++) {
      T tout = static_cast<T>(0);
      v.get(i, tout);
      vout.set(i, fn(data[i], tout));
    }

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  /*! Tested */
  Result add(T v, std::vector<T> &out) const {
    auto fn = [](T thisel, T argel) { return thisel + argel; };
    auto res = apply_el(v, fn, out);
    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }

  /*! Tested */
  Result add(T v, VecN<T, N> &vout) const {
    auto fn = [](T thisel, T argel) { return thisel + argel; };
    auto res = apply_el(v, fn, vout);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result add(const std::vector<T> &v, std::vector<T> &out) const {
    auto fn = [](T thisel, T argel) { return thisel + argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result add(const VecN<T, N> &v, VecN<T, N> &out) const {
    auto fn = [](T thisel, T argel) { return thisel + argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  //
  Result subtract(T v, std::vector<T> &out) const {
    auto fn = [](T thisel, T argel) { return thisel - argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result subtract(T v, VecN<T, N> &vout) const {
    auto fn = [](T thisel, T argel) { return thisel - argel; };
    auto res = apply_el(v, fn, vout);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result subtract(const std::vector<T> &v, std::vector<T> &out) const {
    auto fn = [](T thisel, T argel) { return thisel - argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result subtract(const VecN<T, N> &v, VecN<T, N> &out) const {
    auto fn = [](T thisel, T argel) { return thisel - argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  //
  Result multiply(T v, std::vector<T> &out) const {
    auto fn = [](T thisel, T argel) { return thisel * argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result multiply(T v, VecN<T, N> &vout) const {
    auto fn = [](T thisel, T argel) { return thisel * argel; };
    auto res = apply_el(v, fn, vout);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result multiply(const std::vector<T> &v, std::vector<T> &out) const {
    auto fn = [](T thisel, T argel) { return thisel * argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result multiply(const VecN<T, N> &v, VecN<T, N> &out) const {
    auto fn = [](T thisel, T argel) { return thisel * argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }

  //
  Result divide(T v, std::vector<T> &out) const {
    if (v == static_cast<T>(0)) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, ARG_ERROR);
      return vflag;
    }

    auto fn = [](T thisel, T argel) { return thisel / argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result divide(T v, VecN<T, N> &vout) const {
    // check for zero division
    if (v == static_cast<T>(0)) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, ARG_ERROR);
      return vflag;
    }
    auto fn = [](T thisel, T argel) { return thisel / argel; };
    auto res = apply_el(v, fn, vout);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result divide(const std::vector<T> &v, std::vector<T> &out) const {
    for (unsigned int j = 0; j < v.size(); j++) {
      if (v[j] == static_cast<T>(0)) {
        Result vflag(__LINE__, __FILE__, __FUNCTION__, ARG_ERROR);
        return vflag;
      }
    }
    auto fn = [](T thisel, T argel) { return thisel / argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  /*! Tested */
  Result divide(const VecN<T, N> &v, VecN<T, N> &out) const {
    unsigned int vsize = 0;
    v.size(vsize);
    // check zero division
    for (unsigned int j = 0; j < vsize; j++) {
      T vout = static_cast<T>(0);
      v.get(j, vout);
      if (vout == static_cast<T>(0)) {

        Result vflag(__LINE__, __FILE__, __FUNCTION__, ARG_ERROR);
        return vflag;
      }
    }
    auto fn = [](T thisel, T argel) { return thisel / argel; };
    auto res = apply_el(v, fn, out);

    Result vflag(__LINE__, __FILE__, __FUNCTION__, res.status);
    return vflag;
  }
  Result dot(const T &v, T &out) const {
    out = static_cast<T>(0);
    for (unsigned int i = 0; i < data.size(); i++) {
      out += data[i] * v;
    }

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result dot(const std::vector<T> &v, T &out) const {
    if (v.size() != data.size()) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, SIZE_ERROR);
      return vflag;
    }
    out = static_cast<T>(0);
    for (unsigned int i = 0; i < data.size(); i++) {
      out += data[i] * v[i];
    }

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
  Result dot(const VecN<T, N> &v, T &out) const {

    unsigned int vsize = 0;
    v.size(vsize);
    // check size
    if (vsize != data.size()) {
      Result vflag(__LINE__, __FILE__, __FUNCTION__, SIZE_ERROR);
      return vflag;
    }

    out = static_cast<T>(0);
    for (unsigned int i = 0; i < data.size(); i++) {
      T tout = static_cast<T>(0);
      v.get(i, tout);
      out += data[i] * tout;
    }

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }

  Result cross(const std::vector<T> &v, std::vector<T> &out) const {
    Result vflag(__LINE__, __FILE__, __FUNCTION__, NOT_IMPLEMENTED);
    return vflag;
  }
  Result cross(const VecN<T, N> &v, VecN<T, N> &out) const {
    Result vflag(__LINE__, __FILE__, __FUNCTION__, NOT_IMPLEMENTED);
    return vflag;
  }

private:
  Result n_n_matrix(unsigned int n, std::vector<std::vector<T>> &out) const {
    std::vector<std::vector<T>> mat;
    mat.resize(n);
    for (unsigned int i = 0; i < n; i++) {
      std::vector<T> row;
      row.resize(n);
      mat[i] = row;
      for (unsigned int j = 0; j < n; j++) {
        mat[i][j] = static_cast<T>(0);
      }
    }
    out = mat;

    Result vflag(__LINE__, __FILE__, __FUNCTION__, SUCCESS);
    return vflag;
  }
};

bool CHECK(Result res) { return res.status == SUCCESS; }

#define CHECK_M(call, res)                                                     \
  do {                                                                         \
    res = call;                                                                \
    res.call_name = #call;                                                     \
    res.line_info = __LINE__;                                                  \
    res.file_name = __FILE__;                                                  \
  } while (0)

Result INFO(Result res) {
  res.line_info = __LINE__;
  res.file_name = __FILE__;
  if (res.status != SUCCESS) {
    std::cerr << res << " at " << res.fn_name << " :: " << res.file_name
              << " :: " << res.line_info << std::endl;
  }
  return res;
}

#define INFO_M(call, res)                                                      \
  do {                                                                         \
    res = call;                                                                \
    res = INFO(res);                                                           \
    res.call_name = #call;                                                     \
    res.line_info = __LINE__;                                                  \
    res.file_name = __FILE__;                                                  \
  } while (0)

Result INFO_VERBOSE(Result res) {
  res.line_info = __LINE__;
  res.file_name = __FILE__;
  std::cerr << res << " at " << res.fn_name << " :: " << res.file_name
            << " :: " << res.line_info << std::endl;
  return res;
}
#define INFO_VERBOSE_M(call, res)                                              \
  do {                                                                         \
    res = call;                                                                \
    res = INFO_VERBOSE(res);                                                   \
    res.call_name = #call;                                                     \
    res.line_info = __LINE__;                                                  \
    res.file_name = __FILE__;                                                  \
  } while (0)

} // namespace vepp

#endif

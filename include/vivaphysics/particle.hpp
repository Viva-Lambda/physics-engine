#pragma once

#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/debug.hpp>
#include <vivaphysics/precision.hpp>

using namespace vivaphysics;

namespace vivaphysics {

/** the simplest object that can be simulated*/
class Particle {
  //
protected:
  Vector3 position;
  Vector3 velocity;
  Vector3 acceleration;
  Vector3 accumulated_force;

  /**
    we are using the inverse of the mass for numerical
    stability
   */
  real inverse_mass;
  real damping;

public:
  void integrate(real duration) {

    // can not move the object
    if (inverse_mass <= 0.0)
      return;

    D_CHECK_MSG(duration > 0.0,
                "duration should be bigger than 0");

    // update the linear position to
    // where will we be at the end of the duration
    // v1 ----------- v2
    // v2 = v1 * k + P
    position.add_scaled_vector(velocity, duration);

    // resulting acceleration from applying the force
    auto acc_result = acceleration;

    // force = mass * acceleration
    // acceleration = force / mass
    // acceleration = force * (1 / mass)
    acc_result.add_scaled_vector(accumulated_force,
                                 inverse_mass);

    //
    velocity.add_scaled_vector(acc_result, duration);

    // dampen the value for numerical stability
    velocity *= static_cast<real>(pow(damping, duration));

    // clear the accumulated_force
    accumulated_force.clear();
  }

  /** \name Accessor Functions for the Particle

    These functions help us to set/get the particle state.
   */
  /**@{*/

  /**\name mass related*/
  /**@{*/
  void set_mass(const real mass) {
    D_CHECK_MSG(mass == 0, "mass can not be 0");
    inverse_mass = static_cast<real>(1.0 / mass);
  }
  void set_inverse_mass(real imass) {
    inverse_mass = imass;
  }
  real get_inverse_mass() const { return inverse_mass; }
  bool has_finite_mass() { return inverse_mass >= 0.0; }
  /**@}*/

  /**\name damping related*/
  /**@{*/
  void set_damping(real d) { damping = d; }
  real get_damping() const { return damping; }
  /**@}*/

  /**\name position related*/
  /**@{*/
  void set_position(const Vector3 &v) { position = v; }
  void set_position(const real &x, const real &y,
                    const real &z) {
    position = Vector3(x, y, z);
  }
  Vector3 get_position() const { return position; }
  /**@}*/

  /**\name velocity related*/
  /**@{*/
  void set_velocity(const Vector3 &v) { velocity = v; }
  void set_velocity(const real &x, const real &y,
                    const real &z) {
    velocity = Vector3(x, y, z);
  }
  Vector3 get_velocity() const { return velocity; }
  /**@}*/

  /**\name acceleration related*/
  /**@{*/
  void set_acceleration(const Vector3 &v) {
    acceleration = v;
  }
  void set_acceleration(const real &x, const real &y,
                        const real &z) {
    acceleration = Vector3(x, y, z);
  }
  Vector3 get_acceleration() const { return acceleration; }
  /**@}*/

  /**\name accumulated_force related*/
  /**@{*/
  void set_accumulated_force(const Vector3 &v) {
    accumulated_force = v;
  }
  void set_accumulated_force(const real &x, const real &y,
                             const real &z) {
    accumulated_force = Vector3(x, y, z);
  }
  Vector3 get_accumulated_force() const {
    return accumulated_force;
  }
  Vector3 add_force(const Vector3 &v) {
    accumulated_force += v;
  }
  /**@}*/

  /**@}*/
};
};

#pragma once
// particle contact
#include <external.hpp>
#include <vivaphysics/particle.hpp>

using namespace vivaphysics;

namespace vivaphysics {

struct ContactParticles {
  Particle p1, p2;
  bool is_double = false;
};

class ParticleContact {
public:
  /**holds the particles that are involved in the contact*/
  ContactParticles particles;

  /** holds the restitution coefficient*/
  real restitution;

  /** direction of contact in world coordinates*/
  v3 contact_normal;

  /** depth of penetration at the contact*/
  real penetration;

  /**holds the amount each particle is moved during the
   * contact resolution
   * phase*/
  v3 particle_movement[3];

  /** resolve the contact for both velocity and
   * interpenetration*/
  void resolve(real duration) {
    resolve_velocity(duration);
    resolve_interpenetration(duration);
  }

  real compute_separating_velocity() const {
    v3 relative_velocity = particles.p1.get_velocity();
    if (particles.is_double) {
      relative_velocity -= particles.p2.get_velocity();
    }
    return relative_velocity.dot(contact_normal);
  }

  /** resolve velocity for the contact*/
  void resolve_velocity(real duration) {
    real sep_velocity = compute_separating_velocity();

    // check if the contact needs to be resolved
    if (sep_velocity > 0) {
      // contact is either separating or stationary
      // no need to resolve the contact
      return;
    }
    real sep_v = -sep_velocity * restitution;

    v3 acceleration_caused_velocity =
        particles.p1.get_acceleration();
    if (particles.is_double) {
      acceleration_caused_velocity -=
          particles.p2.get_acceleration();
    }
    real velocity_caused_by_acceleration =
        acceleration_caused_velocity.dot(contact_normal);
    velocity_caused_by_acceleration *= duration;

    // velocity caused by acceleration building up, ie
    // momentum
    if (velocity_caused_by_acceleration < 0) {
      sep_v +=
          restitution * velocity_caused_by_acceleration;
      if (sep_v < 0)
        sep_v = 0;
    }

    real delta_velocity = sep_v - sep_velocity;

    real total_inverse_mass =
        particles.p1.get_inverse_mass();
    if (particles.is_double) {
      total_inverse_mass += particles.p2.get_inverse_mass();
    }

    if (total_inverse_mass <= 0)
      return;

    /***/
    real impulse = delta_velocity / total_inverse_mass;

    // impulse per inverse mass
    v3 impulse_per_inverse_mass = contact_normal * impulse;

    // apply the impulse
    v3 p1_velocity = particles.p1.get_velocity();
    real p1_inverse_mass = particles.p1.get_inverse_mass();
    impulse_per_inverse_mass *= p1_inverse_mass;
    particles.p1.set_velocity(p1_velocity +
                              impulse_per_inverse_mass);
    if (particles.is_double) {
      v3 p2_velocity = particles.p2.get_velocity();
      real p2_inverse_mass =
          particles.p2.get_inverse_mass();
      particles.p2.set_velocity(p2_velocity +
                                impulse_per_inverse_mass *
                                    (-p2_inverse_mass));
    }
  }

  /**resolve interpenetration for the contact*/
  void resolve_interpenetration(real duration) {
    if (penetration <= 0)
      return;

    //
    real total_inverse_mass =
        particles.p1.get_inverse_mass();
    if (particles.is_double) {
      total_inverse_mass += particles.p2.get_inverse_mass();
    }
    if (total_inverse_mass <= 0)
      return;

    //
    v3 move_per_inverse_mass =
        contact_normal * (penetration / total_inverse_mass);

    //
    particle_movement[0] = move_per_inverse_mass *
                           particles.p1.get_inverse_mass();
    if (particles.is_double) {
      particle_movement[1] =
          move_per_inverse_mass *
          (-particles.p2.get_inverse_mass());
    } else {
      particle_movement[1].clear();
    }

    // compute new position of the particle
    particles.p1.set_position(particles.p1.get_position() +
                              particle_movement[0]);
    if (particles.is_double) {
      particles.p2.set_position(
          particles.p2.get_position() +
          particle_movement[1]);
    }
  }
};

class ParticleContactResolver {
public:
  /** number of iterations allowed*/
  unsigned int nb_iterations;

  /**number of used iterations*/
  unsigned int iteration_used;
  ParticleContactResolver(unsigned int iter)
      : nb_iterations(iter) {}
  void set_iterations(unsigned int iter) {
    nb_iterations = iter;
  }

  void
  resolve_contacts(std::vector<ParticleContact> &contacts,
                   real duration) {
    unsigned int nb_contacts =
        static_cast<unsigned int>(contacts.size());
    unsigned int i;
    iteration_used = 0;
    while (iteration_used < nb_iterations) {
      real rmax = REAL_MAX;
      unsigned int max_index = nb_contacts;
      for (i = 0; i < nb_contacts; i++) {
        auto contact = contacts[i];
        real sep_vel =
            contact.compute_separating_velocity();
        auto cond1 = sep_vel < rmax;
        auto cond2 = sep_vel < 0;
        auto cond3 = contact.penetration > 0;
        if (cond1 && (cond2 || cond3)) {
          rmax = sep_vel;
          max_index = i;
        }
      }
      //
      // there's nothing worth solving
      if (max_index == nb_contacts)
        break;

      //
      auto max_contact = contacts[max_index];
      max_contact.resolve(duration);
      auto max_contact_p1 = max_contact.particles.p1;
      auto max_contact_p2 = max_contact.particles.p2;

      // update particles
      v3 *move = max_contact.particle_movement;
      for (auto &contact : contacts) {
        auto p1 = contact.particles.p1;
        auto is_double = contact.particles.is_double;
        if (p1 == max_contact_p1) {
          contact.penetration -=
              move[0].dot(contact.contact_normal);
        } else if (p1 == max_contact_p2) {
          contact.penetration -=
              move[1].dot(contact.contact_normal);
        }
        if (is_double) {
          auto p2 = contact.particles.p2;
          if (p2 == max_contact_p1) {
            contact.penetration +=
                move[0].dot(contact.contact_normal);
          } else if (p2 == max_contact_p2) {
            contact.penetration +=
                move[1].dot(contact.contact_normal);
          }
        }
      }
      iteration_used++;
    }
  }
};

template <class T> struct ParticleContactGenerator {

  /**
    \brief Fill the given contact structure with the
    generated contact

    \param contact the contact array holding particle
    contacts to be written

    \param limit maximum number of contacts in the vector
    that can be written to.

    \param obj generator object
   */
  static unsigned int
  add_contact(T &obj, std::vector<ParticleContact> &contact,
              unsigned int contact_limit) {
    return 0;
  }
};
};

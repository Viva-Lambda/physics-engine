#pragma once

// particle links
#include <external.hpp>
#include <vivaphysics/pcontact.hpp>
#include <vivaphysics/pfgen.hpp>
#include <vivaphysics/plink.hpp>

using namespace vivaphysics;

namespace vivaphysics {

typedef std::vector<
    ParticleContactGenerator<ParticleContactWrapper>>
    ContactGenerators;

//
class ParticleWorld {
public:
  // constructor
  ParticleWorld(unsigned int max_contacts,
                unsigned int iterations)
      : resolver(iterations),
        compute_iterations(iterations == 0),
        max_contact_nb(max_contacts) {}
  /**holds the particles*/
  Particles particles;

  ContactGenerators generators;

  bool compute_iterations;

  ParticleForceRegistry registry;

  ParticleContactResolver resolver;

  std::vector<ParticleContact> contacts;
  unsigned int max_contact_nb;

  // generate particle contacts
  unsigned int generate_contacts() {
    auto limit = max_contact_nb;
    auto contact_start = 0;
    for (unsigned int i = contact_start;
         i < generators.size(); i++) {
      auto contact_generator = generators[i];
      ParticleContactWrapper wrapper;
      auto used_nb_contacts = contact_generator.add_contact(
          wrapper, contacts, contact_start, limit);
      limit -= used_nb_contacts;
      contact_start += used_nb_contacts;
      if (limit <= 0)
        break;
    }
    return max_contact_nb - limit;
  }

  // move particles
  void integrate(real duration) {
    for (auto &particle_ptr : particles) {
      particle_ptr->integrate(duration);
    }
  }

  // run all the physics related operations
  void run(real duration) {
    // apply the force generators
    registry.update_forces(duration);

    // move particles
    integrate(duration);

    //
    auto used_nb_contacts = generate_contacts();
    if (used_nb_contacts != 0) {
      if (compute_iterations) {
        resolver.set_iterations(used_nb_contacts * 2);
      }
      resolver.resolve_contacts(contacts, used_nb_contacts,
                                duration);
    }
  }

  // start physics operations
  void start() {
    // clear any accumulated force from particle
    for (auto &particle_ptr : particles) {
      particle_ptr->clear_accumulator();
    }
  }

  void get_particles(Particles &ps) { ps = particles; }
  void get_contact_generators(ContactGenerators &gs) {
    gs = generators;
  }
  void get_force_registry(ParticleForceRegistry &fs) {
    fs = registry;
  }
};
};

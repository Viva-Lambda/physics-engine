#pragma once

// particle links
#include <external.hpp>
#include <vivaphysics/pcontact.hpp>
#include <vivaphysics/pfgen.hpp>
#include <vivaphysics/plink.hpp>

using namespace vivaphysics;

namespace vivaphysics {

struct ContactGenerators {
  std::vector<
      ParticleContactGenerator<ParticleContactWrapper>>
      generators;
  std::vector<ParticleContactWrapper> contact_data;
  ContactGenerators() {}
  ContactGenerators(
      const std::vector<ParticleContactGenerator<
          ParticleContactWrapper>> &gens,
      const std::vector<ParticleContactWrapper> &pcws)
      : generators(gens), contact_data(pcws) {
    COMP_CHECK_MSG(
        generators.size() == contact_data.size(),
        generators.size(), contact_data.size(),
        "vector size must match for the argument");
  }
  void
  add(const ParticleContactGenerator<ParticleContactWrapper>
          &pcgen,
      const ParticleContactWrapper &pcw) {
    generators.push_back(pcgen);
    contact_data.push_back(pcw);
  }

  unsigned int size() {
    return static_cast<unsigned int>(generators.size());
  }
};

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

  ContactGenerators contact_generators;

  bool compute_iterations;

  ParticleForceRegistry registry;

  ParticleContactResolver resolver;

  std::vector<ParticleContact> contacts;
  unsigned int max_contact_nb;

  void add_contact_generator(
      const ParticleContactGenerator<ParticleContactWrapper>
          &pcgen,
      const ParticleContactWrapper &pcw) {
    contact_generators.add(pcgen, pcw);
  }

  // generate particle contacts
  unsigned int generate_contacts() {
    auto limit = max_contact_nb;
    auto contact_start = 0;
    for (unsigned int i = contact_start;
         i < contact_generators.size(); i++) {
      auto contact_generator =
          contact_generators.generators[i];
      ParticleContactWrapper wrapper =
          contact_generators.contact_data[i];
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
    gs = contact_generators;
  }
  ContactGenerators get_contact_generators() {
    return contact_generators;
  }
  void get_force_registry(ParticleForceRegistry &fs) {
    fs = registry;
  }
  ParticleForceRegistry get_force_registry() {
    return registry;
  }
};
};

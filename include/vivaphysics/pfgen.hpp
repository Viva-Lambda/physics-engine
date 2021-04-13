// particle force generator
#pragma once
#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>

using namespace vivaphysics;

namespace vivaphysics {
class ParticleForceGenerator {
public:
  /**Compute the force that is going to be applied to given
   * particle*/
  virtual void update_force(Particle *p, real duration) = 0;
};

class ParticleGravity : public ParticleForceGenerator {
  //
public:
  v3 gravity;
  ParticleGravity(const v3 &g) : gravity(g) {}
  void update_force(Particle &p, real duration) override {
    // check if object is immovable
    if (!p.has_finite_mass())
      return;

    // apply the given force
    p.add_force(gravity * p.get_mass());
  }
};

class ParticleForceRegistry {
protected:
  typedef std::map<Particle *, ParticleForceGenerator *>
      Registry;
  Registry force_register;

public:
  /** registers the given particle with given generator */
  void add(Particle *p, ParticleForceGenerator *gen) {
    auto particle_gen_pair = std::make_pair(p, gen);
    force_register.insert(particle_gen_pair);
  }

  /** removes the given particle with given generator */
  void remove(Particle *p, ParticleForceGenerator *gen);

  /**clears out the registry*/
  void clear();

  /**update forces of the registry*/
  void update_forces(real duration) {
    for (auto & [ index, particle_gen_pair ] :
         force_register) {
      particle_gen_pair.second->update_force(
          particle_gen_pair.first, duration);
    }
  }
};
};

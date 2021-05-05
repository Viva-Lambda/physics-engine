// particle force generator
#pragma once
#include <external.hpp>
#include <memory>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>
#include <vivaphysics/pfgenenum.hpp>

using namespace vivaphysics;

namespace vivaphysics {
template <class T> struct ParticleForceGenerator {
  /**Compute the force that is going to be applied to given
   * particle*/
  void update_force(const T &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {}
};

struct ParticleGravity {
  //
  v3 gravity;
  ParticleGravity(const v3 &g) : gravity(g) {}
};
struct ParticleDrag {
  /**velocity drag coefficient*/
  real k1;
  /**velocity squared drag coefficient*/
  real k2;
  ParticleDrag(real k_1, real k_2) : k1(k_1), k2(k_2) {}
};

struct ParticleAnchoredSpring {
  /**location of the anchored end of the spring*/
  point3 anchor;

  /** spring constant*/
  real spring_constant;

  /** rest length of the spring*/
  real rest_length;
  ParticleAnchoredSpring() {}
  ParticleAnchoredSpring(const point3 &a, real sc, real rl)
      : anchor(a), spring_constant(sc), rest_length(rl) {}
};

struct ParticleAnchoredBungee : ParticleAnchoredSpring {
  ParticleAnchoredBungee(const point3 &a, real sc, real rl)
      : ParticleAnchoredSpring(a, sc, rl) {}
};

struct ParticleFakeSpring {
  /**location of the anchored end of the spring*/
  point3 anchor;

  /** spring constant*/
  real spring_constant;

  /** damping level to apply to spring's oscilliation*/
  real damping;
  ParticleFakeSpring() {}
  ParticleFakeSpring(const point3 &a, real sc, real d)
      : anchor(a), spring_constant(sc), damping(d) {}
};
struct ParticleSpring {
  /**particle at the end of spring*/
  Particle end_p;

  /** spring constant*/
  real spring_constant;

  /** rest length of the spring*/
  real rest_length;
  ParticleSpring(const Particle &p, real sc, real rl)
      : end_p(p), spring_constant(sc), rest_length(rl) {}
};
struct ParticleBungee {
  Particle end_p;

  /** spring constant*/
  real spring_constant;

  /** rest length of the bungee*/
  real rest_length;
  ParticleBungee(const Particle &p, real sc, real rl)
      : end_p(p), spring_constant(sc), rest_length(rl) {}
};

struct ParticleBuoyancy {
  /**maximum submersion depth of the object
    before it generates its maximum buoyancy force*/
  real max_depth;

  /**volume of the object*/
  real volume;

  /**height of the liquid*/
  real liquid_height;

  /**liquid density*/
  real liquid_density;
  ParticleBuoyancy(real mdepth, real v, real lh,
                   real ld = 1000.0f)
      : max_depth(mdepth), volume(v), liquid_height(lh),
        liquid_density(ld) {}
};

struct ParticleForceGeneratorWrapper {
  real k1_spring_constant_max_depth =
      0.0; // particle gravity
           // anchored
  // spring, bungee, spring, bungee, buoyancy

  real k2_rest_length_volume =
      0.0; // particle gravity anchored
  // spring, bungee, spring, bungee

  real damping_liquid_height = 0.0;

  v3 gravity_anchor =
      v3(0.0); // spring, bungee, spring, bungee

  Particle end_p; // particle spring, particle bungee
  real liquid_density = 0.0; // particle buoyancy

  ParticleForceGeneratorType gtype;
  bool
  operator==(const ParticleForceGeneratorWrapper &w) const {
    if (w.gtype != gtype)
      return false;
    if (w.gravity_anchor != gravity_anchor)
      return false;
    if (w.k1_spring_constant_max_depth !=
        k1_spring_constant_max_depth)
      return false;
    if (w.k2_rest_length_volume != k2_rest_length_volume)
      return false;
    if (w.damping_liquid_height != damping_liquid_height)
      return false;
    if (w.end_p != end_p)
      return false;
    if (w.liquid_density != liquid_density)
      return false;
    return true;
  }
  bool
  operator!=(const ParticleForceGeneratorWrapper &w) const {
    return !(*this == w);
  }
  ParticleForceGeneratorWrapper() {}
  ParticleForceGeneratorWrapper(const ParticleGravity &g)
      : gravity_anchor(g.gravity),
        gtype(ParticleForceGeneratorType::GRAVITY) {}
  ParticleGravity to_gravity() const {
    return ParticleGravity(gravity_anchor);
  }
  ParticleForceGeneratorWrapper(const ParticleDrag &g)
      : k1_spring_constant_max_depth(g.k1),
        k2_rest_length_volume(g.k2),
        gtype(ParticleForceGeneratorType::DRAG) {}
  ParticleDrag to_drag() const {
    return ParticleDrag(k1_spring_constant_max_depth,
                        k2_rest_length_volume);
  }
  ParticleForceGeneratorWrapper(
      const ParticleAnchoredSpring &g)
      : k1_spring_constant_max_depth(g.spring_constant),
        k2_rest_length_volume(g.rest_length),
        gravity_anchor(g.anchor),
        gtype(ParticleForceGeneratorType::ANCHORED_SPRING) {
  }
  ParticleAnchoredSpring to_anchor_spring() const {
    return ParticleAnchoredSpring(
        gravity_anchor, k1_spring_constant_max_depth,
        k2_rest_length_volume);
  }
  ParticleForceGeneratorWrapper(
      const ParticleAnchoredBungee &g)
      : k1_spring_constant_max_depth(g.spring_constant),
        k2_rest_length_volume(g.rest_length),
        gravity_anchor(g.anchor),
        gtype(ParticleForceGeneratorType::ANCHORED_BUNGEE) {
  }
  ParticleAnchoredBungee to_anchor_bungee() const {
    return ParticleAnchoredBungee(
        gravity_anchor, k1_spring_constant_max_depth,
        k2_rest_length_volume);
  }

  ParticleForceGeneratorWrapper(const ParticleFakeSpring &g)
      : k1_spring_constant_max_depth(g.spring_constant),
        damping_liquid_height(g.damping),
        gravity_anchor(g.anchor),
        gtype(ParticleForceGeneratorType::FAKE_SPRING) {}
  ParticleFakeSpring to_fake_spring() const {
    return ParticleFakeSpring(gravity_anchor,
                              k1_spring_constant_max_depth,
                              damping_liquid_height);
  }
  ParticleSpring to_spring() const {
    return ParticleSpring(end_p,
                          k1_spring_constant_max_depth,
                          k2_rest_length_volume);
  }
  ParticleBungee to_bungee() const {
    return ParticleBungee(end_p,
                          k1_spring_constant_max_depth,
                          k2_rest_length_volume);
  }

  ParticleForceGeneratorWrapper(const ParticleSpring &g)
      : k1_spring_constant_max_depth(g.spring_constant),
        k2_rest_length_volume(g.rest_length),
        end_p(g.end_p),
        gtype(ParticleForceGeneratorType::SPRING) {}

  ParticleForceGeneratorWrapper(const ParticleBungee &g)
      : k1_spring_constant_max_depth(g.spring_constant),
        k2_rest_length_volume(g.rest_length),
        end_p(g.end_p),
        gtype(ParticleForceGeneratorType::BUNGEE) {}

  ParticleForceGeneratorWrapper(const ParticleBuoyancy &g)
      : k1_spring_constant_max_depth(g.max_depth),
        k2_rest_length_volume(g.volume),
        damping_liquid_height(g.liquid_height),
        liquid_density(g.liquid_density),
        gtype(ParticleForceGeneratorType::BUOYANCY) {}

  ParticleBuoyancy to_buoyancy() const {
    return ParticleBuoyancy(
        k1_spring_constant_max_depth, k2_rest_length_volume,
        damping_liquid_height, liquid_density);
  }
};

template <> struct ParticleForceGenerator<ParticleGravity> {

  void update_force(const ParticleGravity &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    // check if object is immovable
    if (!p->has_finite_mass())
      return;

    // apply the given force
    p->add_force(generator.gravity * p->get_mass());
  }
};

template <> struct ParticleForceGenerator<ParticleDrag> {
  void update_force(const ParticleDrag &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    // check if object is immovable
    v3 force;
    p->get_velocity(force);

    // compute drag coefficient
    real dcoeff = force.magnitude();
    dcoeff = generator.k1 * dcoeff +
             generator.k2 * dcoeff * dcoeff;

    // normalize the force
    force.normalize();
    force *= -dcoeff;

    // apply the given force
    p->add_force(force);
  }
};

template <>
struct ParticleForceGenerator<ParticleAnchoredSpring> {
  void update_force(const ParticleAnchoredSpring &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    // check if object is immovable
    v3 force;
    p->get_position(force);
    force -= generator.anchor;

    // compute magnitude of the force
    real mag = force.magnitude();
    mag = (generator.rest_length - mag) *
          generator.spring_constant;

    // normalize the force
    force.normalize();
    force *= mag;

    // apply the given force
    p->add_force(force);
  }
};

template <>
struct ParticleForceGenerator<ParticleAnchoredBungee> {
  void update_force(const ParticleAnchoredBungee &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    // check if object is immovable
    v3 force;
    p->get_position(force);
    force -= generator.anchor;

    // compute magnitude of the force
    real mag = force.magnitude();
    if (mag < generator.rest_length)
      return;
    mag -= generator.rest_length;
    mag *= generator.spring_constant;

    // normalize the force
    force.normalize();
    force *= -mag;

    // apply the given force
    p->add_force(force);
  }
};
template <>
struct ParticleForceGenerator<ParticleFakeSpring> {
  void update_force(const ParticleFakeSpring &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    // check if object is immovable
    if (!p->has_finite_mass())
      return;

    point3 pos;
    p->get_position(pos);
    pos -= generator.anchor;

    // compute constants
    auto v = 4 * generator.spring_constant -
             generator.damping * generator.damping;
    real g = 0.5f * static_cast<real>(sqrt(v));
    if (g == 0.0f)
      return;

    v3 c = pos * (generator.damping / (2.0f * g));
    c += p->get_velocity() * (1.0f / g);

    // compute target position
    point3 target =
        pos * static_cast<real>(cos(g * duration));
    target += c * static_cast<real>(sin(g * duration));
    target *= static_cast<real>(
        exp(-0.5f * duration * generator.damping));

    // compute acceleration and thus force
    v3 accel = (target - pos);
    accel *= static_cast<real>(1.0 / duration * duration);
    accel -= p->get_velocity();
    accel *= static_cast<real>(1.0 / duration);

    // apply the given force
    p->add_force(accel * p->get_mass());
  }
};
template <> struct ParticleForceGenerator<ParticleSpring> {
  void update_force(const ParticleSpring &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    v3 force;
    p->get_position(force);
    force -= generator.end_p.get_position();

    // compute magnitude of the resulting force
    real mag = force.magnitude();
    mag =
        static_cast<real>(abs(mag - generator.rest_length));
    mag *= generator.spring_constant;

    // apply the given force
    force.normalize();
    force *= -mag;
    p->add_force(force);
  }
};

template <> struct ParticleForceGenerator<ParticleBungee> {
  void update_force(const ParticleBungee &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    v3 force;
    p->get_position(force);
    force -= generator.end_p.get_position();

    // compute magnitude of the resulting force
    real mag = force.magnitude();
    if (mag <= generator.rest_length)
      return;
    mag = static_cast<real>(mag - generator.rest_length);
    mag *= generator.spring_constant;

    // apply the given force
    force.normalize();
    force *= -mag;
    p->add_force(force);
  }
};

template <>
struct ParticleForceGenerator<ParticleBuoyancy> {
  void update_force(const ParticleBuoyancy &generator,
                    std::shared_ptr<Particle> p,
                    real duration) {
    real depth = p->get_position().y;

    // are we in admissible depth
    if (depth >=
        generator.liquid_height + generator.max_depth)
      return;

    v3 force(0.0f);
    if (depth <=
        generator.liquid_height - generator.max_depth) {
      force.y = generator.liquid_density * generator.volume;
      p->add_force(force);
      return;
    }
    // we are partly submerged
    force.y = generator.liquid_density * generator.volume;
    force.y *= (depth - generator.max_depth -
                generator.liquid_height);
    force.y /= (2 * generator.max_depth);
    p->add_force(force);
  }
};
template <>
struct ParticleForceGenerator<
    ParticleForceGeneratorWrapper> {
  void update_force(
      const ParticleForceGeneratorWrapper &generator,
      std::shared_ptr<Particle> p, real duration) {
    switch (generator.gtype) {
    case ParticleForceGeneratorType::GRAVITY:
      ParticleForceGenerator<ParticleGravity> gen;
      gen.update_force(generator.to_gravity(), p, duration);
      break;
    case ParticleForceGeneratorType::DRAG:
      ParticleForceGenerator<ParticleDrag> gen2;
      gen2.update_force(generator.to_drag(), p, duration);
      break;
    case ParticleForceGeneratorType::ANCHORED_SPRING:
      ParticleForceGenerator<ParticleAnchoredSpring> gen3;
      gen3.update_force(generator.to_anchor_spring(), p,
                        duration);
      break;
    case ParticleForceGeneratorType::ANCHORED_BUNGEE:
      ParticleForceGenerator<ParticleAnchoredBungee> gen4;
      gen4.update_force(generator.to_anchor_bungee(), p,
                        duration);
      break;
    case ParticleForceGeneratorType::FAKE_SPRING:
      ParticleForceGenerator<ParticleFakeSpring> gen5;
      gen5.update_force(generator.to_fake_spring(), p,
                        duration);
      break;
    case ParticleForceGeneratorType::SPRING:
      ParticleForceGenerator<ParticleSpring> gen6;
      gen6.update_force(generator.to_spring(), p, duration);
      break;
    case ParticleForceGeneratorType::BUNGEE:
      ParticleForceGenerator<ParticleBungee> gen7;
      gen7.update_force(generator.to_bungee(), p, duration);
      break;
    case ParticleForceGeneratorType::BUOYANCY:
      ParticleForceGenerator<ParticleBuoyancy> gen8;
      gen8.update_force(generator.to_buoyancy(), p,
                        duration);
      break;
    }
  }
};

class ParticleForceRegistry {
protected:
  typedef std::vector<
      std::pair<std::shared_ptr<Particle>,
                ParticleForceGeneratorWrapper>>
      Registry;
  Registry force_register;

public:
  /** registers the given particle with given generator */
  void add(std::shared_ptr<Particle> p,
           ParticleForceGeneratorWrapper gwrapper) {
    auto particle_gen_pair = std::make_pair(p, gwrapper);
    force_register.push_back(particle_gen_pair);
  }

  /** removes the given particle with given generator */
  void remove(std::shared_ptr<Particle> p,
              ParticleForceGeneratorWrapper gen) {
    for (unsigned int i = 0; i < force_register.size();
         i++) {
      auto vpair = force_register[i];
      if (vpair.first == p && vpair.second == gen) {
        force_register.erase(force_register.begin() + i);
      }
    }
  }

  /**clears out the registry*/
  void clear() { force_register.clear(); }

  /**update forces of the registry*/
  void update_forces(real duration) {
    for (auto & [ part_ptr, generator_wrapper ] :
         force_register) {
      auto generator = ParticleForceGenerator<
          ParticleForceGeneratorWrapper>();
      generator.update_force(generator_wrapper, part_ptr,
                             duration);
    }
  }
};
};

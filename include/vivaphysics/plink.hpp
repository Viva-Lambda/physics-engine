#pragma once

// particle links
#include <external.hpp>
#include <vivaphysics/pcontact.hpp>
#include <vivaphysics/plinkenum.hpp>

using namespace vivaphysics;

namespace vivaphysics {

struct ParticleLink {
  ContactParticles particle;

  real current_length() const {
    v3 relative_position = particle.p1.get_position() -
                           particle.p2.get_position();
    return relative_position.magnitude();
  }
};
struct ParticleCable : ParticleLink {
  /**maximum length of the cable*/
  real max_length = 0;

  /**bounciness of the cable*/
  real restitution = 0;
  ParticleCable() {}
};
struct ParticleRod : ParticleLink {
  real length = 0;
  ParticleRod() {}
};
struct ParticleConstraint {
  ContactParticles particle;
  v3 anchor;

  real current_length() const {
    //
    v3 relative_position =
        particle.p1.get_position() - anchor;
    return relative_position.magnitude();
  }
};
struct ParticleCableConstraint : ParticleConstraint {
  real max_length = 0;
  real restitution = 0;
  ParticleCableConstraint() {}
};
struct ParticleRodConstraint : ParticleConstraint {
  real length = 0;
  ParticleRodConstraint() {}
};

struct ParticleContactWrapper {
  ContactParticles particle;
  real length_max_length;
  real restitution;
  v3 anchor;
  ParticleContactGeneratorType type;
  ParticleContactWrapper() {}

  ParticleContactWrapper(const ParticleCable &c)
      : length_max_length(c.max_length),
        restitution(c.restitution), particle(c.particle),
        type(ParticleContactGeneratorType::CABLE) {}

  ParticleContactWrapper(const ParticleRod &r)
      : length_max_length(r.length), particle(r.particle),
        type(ParticleContactGeneratorType::ROD) {}

  ParticleContactWrapper(const ParticleCableConstraint &c)
      : length_max_length(c.max_length),
        restitution(c.restitution), particle(c.particle),
        anchor(c.anchor),
        type(ParticleContactGeneratorType::
                 CABLE_CONSTRAINT) {}

  ParticleContactWrapper(const ParticleRodConstraint &r)
      : length_max_length(r.length), particle(r.particle),
        anchor(r.anchor),
        type(ParticleContactGeneratorType::ROD_CONSTRAINT) {
  }
  ParticleCable to_cable() const {
    ParticleCable cable;
    cable.particle = particle;
    cable.max_length = length_max_length;
    cable.restitution = restitution;
    return cable;
  }
  ParticleRod to_rod() const {
    ParticleRod rod;
    rod.particle = particle;
    rod.length = length_max_length;
    return rod;
  }
  ParticleCableConstraint to_cable_constraint() const {
    ParticleCableConstraint c;
    c.particle = particle;
    c.anchor = anchor;
    c.max_length = length_max_length;
    c.restitution = restitution;
    return c;
  }
  ParticleRodConstraint to_rod_constraint() const {
    ParticleRodConstraint c;
    c.particle = particle;
    c.anchor = anchor;
    c.length = length_max_length;
    return c;
  }
};

template <> struct ParticleContactGenerator<ParticleCable> {

  unsigned int
  add_contact(ParticleCable &cable,
              std::vector<ParticleContact> &contact,
              unsigned int contact_start,
              unsigned int contact_end) {
    // length of the cable
    real length = cable.current_length();

    // over extended or not ?
    if (length < cable.max_length) {
      return 0;
    }
    contact[contact_start].particles = cable.particle;

    // calculate the normal
    v3 normal = cable.particle.p2.get_position() -
                cable.particle.p1.get_position();
    normal.normalize();
    contact[contact_start].contact_normal = normal;
    contact[contact_start].penetration =
        length - cable.max_length;
    contact[contact_start].restitution = cable.restitution;

    return 1;
  }
};
template <> struct ParticleContactGenerator<ParticleRod> {

  unsigned int
  add_contact(ParticleRod &rod,
              std::vector<ParticleContact> &contact,
              unsigned int contact_start,
              unsigned int contact_end) {
    // length of the cable
    real cur_length = rod.current_length();

    // over extended or not ?
    if (cur_length == rod.length) {
      return 0;
    }
    contact[contact_start].particles = rod.particle;

    // calculate the normal
    v3 normal = rod.particle.p2.get_position() -
                rod.particle.p1.get_position();
    normal.normalize();

    if (cur_length > rod.length) {
      contact[contact_start].contact_normal = normal;
      contact[contact_start].penetration =
          cur_length - rod.length;
    } else {
      contact[contact_start].contact_normal = normal * -1;
      contact[contact_start].penetration =
          rod.length - cur_length;
    }
    // no bounciness
    contact[contact_start].restitution = 0;

    return 1;
  }
};

template <>
struct ParticleContactGenerator<ParticleCableConstraint> {

  unsigned int
  add_contact(ParticleCableConstraint &cable,
              std::vector<ParticleContact> &contact,
              unsigned int contact_start,
              unsigned int contact_end) {
    // length of the cable
    real cur_length = cable.current_length();

    // over extended or not ?
    if (cur_length < cable.max_length) {
      return 0;
    }
    contact[contact_start].particles.p1 = cable.particle.p1;
    contact[contact_start].particles.is_double = false;

    // calculate the normal
    v3 normal =
        cable.anchor - cable.particle.p1.get_position();
    normal.normalize();
    contact[contact_start].contact_normal = normal;

    contact[contact_start].penetration =
        cur_length - cable.max_length;
    contact[contact_start].restitution = cable.restitution;

    return 1;
  }
};

template <>
struct ParticleContactGenerator<ParticleRodConstraint> {

  unsigned int
  add_contact(ParticleRodConstraint &rod,
              std::vector<ParticleContact> &contact,
              unsigned int contact_start,
              unsigned int contact_end) {
    // length of the cable
    real cur_length = rod.current_length();

    // over extended or not ?
    if (cur_length == rod.length) {
      return 0;
    }
    contact[contact_start].particles.p1 = rod.particle.p1;
    contact[contact_start].particles.is_double = false;

    // calculate the normal
    v3 normal = rod.anchor - rod.particle.p1.get_position();
    normal.normalize();

    if (cur_length > rod.length) {
      contact[contact_start].contact_normal = normal;
      contact[contact_start].penetration =
          cur_length - rod.length;
    } else {
      contact[contact_start].contact_normal = normal * -1;
      contact[contact_start].penetration =
          rod.length - cur_length;
    }
    // no bounciness
    contact[contact_start].restitution = 0;
    return 1;
  }
};

template <>
struct ParticleContactGenerator<ParticleContactWrapper> {

  unsigned int
  add_contact(ParticleContactWrapper &w,
              std::vector<ParticleContact> &contact,
              unsigned int contact_start,
              unsigned int contact_end) {
    //
    unsigned int retval = 0;
    switch (w.type) {
    case ParticleContactGeneratorType::CABLE: {
      auto c1 = w.to_cable();
      ParticleContactGenerator<ParticleCable> pcg1;
      retval = pcg1.add_contact(c1, contact, contact_start,
                                contact_end);
      break;
    }

    case ParticleContactGeneratorType::ROD: {
      auto c2 = w.to_rod();
      ParticleContactGenerator<ParticleRod> pcg2;
      retval = pcg2.add_contact(c2, contact, contact_start,
                                contact_end);
      break;
    }
    case ParticleContactGeneratorType::CABLE_CONSTRAINT: {
      auto c3 = w.to_cable_constraint();
      ParticleContactGenerator<ParticleCableConstraint>
          pcg3;
      retval = pcg3.add_contact(c3, contact, contact_start,
                                contact_end);
      break;
    }
    case ParticleContactGeneratorType::ROD_CONSTRAINT: {
      auto c4 = w.to_rod_constraint();
      ParticleContactGenerator<ParticleRodConstraint> pcg4;
      retval = pcg4.add_contact(c4, contact, contact_start,
                                contact_end);
      break;
    }
    }
    return retval;
  }
};
};

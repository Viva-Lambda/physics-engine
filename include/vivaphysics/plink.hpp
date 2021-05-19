#pragma once

// particle links
#include <external.hpp>
#include <vivaphysics/pcontact.hpp>
#include <vivaphysics/plinkenum.hpp>

using namespace vivaphysics;

namespace vivaphysics {

struct ParticleLink {
  ContactParticles contact_ps;

  real current_length() const {
    v3 relative_position =
        contact_ps.ps[0]->get_position() -
        contact_ps.ps[1]->get_position();
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
  ContactParticles contact_ps;
  v3 anchor;

  real current_length() const {
    //
    v3 relative_position =
        contact_ps.ps[0]->get_position() - anchor;
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

struct GroundContacts {
  Particles particles;
  GroundContacts(Particles &ps) : particles(ps) {}
};

struct ParticleContactWrapper {
  ContactParticles contact_ps;
  real length_max_length;
  real restitution;
  v3 anchor;
  ParticleContactGeneratorType type;
  ParticleContactWrapper() {}

  ParticleContactWrapper(const ParticleCable &c)
      : contact_ps(c.contact_ps),
        length_max_length(c.max_length),
        restitution(c.restitution),
        type(ParticleContactGeneratorType::CABLE) {}

  ParticleContactWrapper(const ParticleRod &r)
      : contact_ps(r.contact_ps),
        length_max_length(r.length),
        type(ParticleContactGeneratorType::ROD) {}

  ParticleContactWrapper(const ParticleCableConstraint &c)
      : contact_ps(c.contact_ps),
        length_max_length(c.max_length),
        restitution(c.restitution), anchor(c.anchor),
        type(ParticleContactGeneratorType::
                 CABLE_CONSTRAINT) {}

  ParticleContactWrapper(const ParticleRodConstraint &r)
      : contact_ps(r.contact_ps),
        length_max_length(r.length), anchor(r.anchor),
        type(ParticleContactGeneratorType::ROD_CONSTRAINT) {
  }
  ParticleContactWrapper(const GroundContacts &g)
      : type(ParticleContactGeneratorType::GROUND),
        contact_ps(g.particles) {}
  ParticleCable to_cable() const {
    ParticleCable cable;
    cable.contact_ps = contact_ps;
    cable.max_length = length_max_length;
    cable.restitution = restitution;
    return cable;
  }
  ParticleRod to_rod() const {
    ParticleRod rod;
    rod.contact_ps = contact_ps;
    rod.length = length_max_length;
    return rod;
  }
  ParticleCableConstraint to_cable_constraint() const {
    ParticleCableConstraint c;
    c.contact_ps = contact_ps;
    c.anchor = anchor;
    c.max_length = length_max_length;
    c.restitution = restitution;
    return c;
  }
  ParticleRodConstraint to_rod_constraint() const {
    ParticleRodConstraint c;
    c.contact_ps = contact_ps;
    c.anchor = anchor;
    c.length = length_max_length;
    return c;
  }
  GroundContacts to_ground() const {
    auto particles = contact_ps.ps;
    auto gc = GroundContacts(particles);
    return gc;
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
    contact[contact_start].particles = cable.contact_ps;

    // calculate the normal
    v3 normal = cable.contact_ps.ps[1]->get_position() -
                cable.contact_ps.ps[0]->get_position();
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
    contact[contact_start].particles = rod.contact_ps;

    // calculate the normal
    v3 normal = rod.contact_ps.ps[1]->get_position() -
                rod.contact_ps.ps[0]->get_position();
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
    contact[contact_start].particles.ps[0] =
        cable.contact_ps.ps[0];
    contact[contact_start].particles.is_double = false;

    // calculate the normal
    v3 normal = cable.anchor -
                cable.contact_ps.ps[0]->get_position();
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
    contact[contact_start].particles = rod.contact_ps;
    contact[contact_start].particles.is_double = false;

    // calculate the normal
    v3 normal =
        rod.anchor - rod.contact_ps.ps[0]->get_position();
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
struct ParticleContactGenerator<GroundContacts> {
  unsigned int
  add_contact(const GroundContacts &gs,
              std::vector<ParticleContact> &contacts,
              unsigned int contact_start,
              unsigned int contact_end) {
    unsigned int count = 0;
    for (auto &particle_ptr : gs.particles) {
      real y = particle_ptr->get_position().y;
      if (y < 0.0) {
        contacts[contact_start].contact_normal = v3::UP;
        contacts[contact_start].particles = *particle_ptr;
        contacts[contact_start].particles.is_double = false;
        contacts[contact_start].penetration = -y;
        contacts[contact_start].restitution = 0.2f;
        contact_start++;
        count++;
      }
      if (count >= contact_end)
        return count;
    }
    return count;
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
    case ParticleContactGeneratorType::GROUND: {
      auto c5 = w.to_ground();
      ParticleContactGenerator<GroundContacts> pcg_gc;
      retval = pcg_gc.add_contact(
          c5, contact, contact_start, contact_end);
    }
    }
    return retval;
  }
};
};

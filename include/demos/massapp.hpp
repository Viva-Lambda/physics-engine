#pragma once
// mass aggregate engine application
#include <demos/mesh.hpp>
#include <demos/meshapp.hpp>
#include <external.hpp>
#include <vivaphysics/pworld.hpp>

using namespace vivademos;

namespace vivademos {
class MassAggregateApp : public MeshDemoApp {
  //
public:
  vivaphysics::ParticleWorld world;
  Particles particles;
  vivaphysics::ParticleContactGenerator<
      vivaphysics::ParticleContactWrapper>
      ground_contact_gen;
  vivaphysics::ParticleContactWrapper gcontact_wrapper;
  //
  MassAggregateApp(unsigned int particle_count)
      : world(particle_count * 10, 0) {
    //
    particles = std::vector<std::shared_ptr<Particle>>();
    for (unsigned int i = 0; i < particle_count; i++) {
      auto particle_ptr = std::make_shared<Particle>();
      particles.push_back(particle_ptr);
      world.particles.push_back(particle_ptr);
    }
    //
    auto gcontact = GroundContacts(world.particles);
    gcontact_wrapper = ParticleContactWrapper(gcontact);
    ground_contact_gen =
        ParticleContactGenerator<ParticleContactWrapper>();
    world.add_contact_generator(ground_contact_gen,
                                gcontact_wrapper);
  }
};
};

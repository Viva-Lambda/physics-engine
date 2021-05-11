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
  std::vector<std::shared_ptr<vivaphysics::Particle>>
      particles;
  vivaphysics::ParticleContactGenerator<GroundContacts>
      ground_contact_gen;
  MassAggregateApp(unsigned int particle_count): world(particle_count * 10, 0){
      //
  }
};
};

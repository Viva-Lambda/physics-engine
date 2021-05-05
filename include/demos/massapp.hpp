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
};
};

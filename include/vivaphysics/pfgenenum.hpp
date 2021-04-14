#pragma once

#include <external.hpp>

using namespace vivaphysics;

namespace vivaphysics {

enum class ParticleForceGeneratorType {
  GRAVITY = 0,
  DRAG = 1,
  ANCHORED_SPRING = 2,
  ANCHORED_BUNGEE = 3,
  FAKE_SPRING = 4,
  SPRING = 5,
  BUNGEE = 6,
  BUOYANCY = 7
};
};

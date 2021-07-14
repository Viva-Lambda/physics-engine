#pragma once
// simple vertex object
#include <demos/meshutils.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>

using namespace vivademos;
//
namespace vivademos {

struct Vertex {
  // pos
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texCoord;
  //
  glm::vec3 tangent;
  glm::vec3 bitangent;
  Vertex() {}
  Vertex(const glm::vec3 &p, const glm::vec3 &n, const glm::vec2 &t,
         const glm::vec3 &tan, const glm::vec3 &bitan)
      : pos(p), normal(n), texCoord(t), tangent(tan), bitangent(bitan) {}
  Vertex(const glm::vec3 &p, const glm::vec3 &n, const glm::vec2 &t)
      : pos(p), normal(n), texCoord(t), tangent(0, 0, 0), bitangent(0, 0, 0) {}
  Vertex(vivaphysics::real px, vivaphysics::real py, vivaphysics::real pz,
         vivaphysics::real nx, vivaphysics::real ny, vivaphysics::real nz,
         vivaphysics::real tx, vivaphysics::real ty, vivaphysics::real tanx,
         vivaphysics::real tany, vivaphysics::real tanz,
         vivaphysics::real bitanx, vivaphysics::real bitany,
         vivaphysics::real bitanz)
      : pos(px, py, pz), normal(nx, ny, nz), texCoord(tx, ty),
        tangent(tanx, tany, tanz), bitangent(bitanx, bitany, bitanz) {}
  Vertex(vivaphysics::real px, vivaphysics::real py, vivaphysics::real pz,
         vivaphysics::real nx, vivaphysics::real ny, vivaphysics::real nz,
         vivaphysics::real tx, vivaphysics::real ty)
      : pos(px, py, pz), normal(nx, ny, nz), texCoord(tx, ty), tangent(0, 0, 0),
        bitangent(0, 0, 0) {}
};
} // namespace vivademos

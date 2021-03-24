#pragma once
// utility functions for mesh
#include <external.hpp>

namespace vivademos {

glm::vec3 getTangent(glm::vec2 deltaUV2, glm::vec2 deltaUV1,
                     glm::vec3 edge1, glm::vec3 edge2) {
  // Compute tangent
  GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y -
                      deltaUV2.x * deltaUV1.y);
  glm::vec3 tangent;
  tangent.x =
      f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  tangent.y =
      f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  tangent.z =
      f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  tangent = glm::normalize(tangent);
  return tangent;
}
glm::vec3 getBiTangent(glm::vec2 deltaUV2,
                       glm::vec2 deltaUV1, glm::vec3 edge1,
                       glm::vec3 edge2) {
  GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y -
                      deltaUV2.x * deltaUV1.y);
  glm::vec3 bitangent;
  bitangent.x =
      f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  bitangent.y =
      f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  bitangent.z =
      f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
  bitangent = glm::normalize(bitangent);
  return bitangent;
}

};

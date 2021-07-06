#pragma once
// vertex maker functions
#include <demos/vertex.hpp>
#include <external.hpp>

using namespace vivademos;

namespace vivademos {
void mk_triangle_vertices(const glm::vec3 ps[3], const glm::vec2 ts[3],
                          const glm::vec3 &n, Vertex vs[3]) {
  glm::vec3 edge1 = ps[1] - ps[0];
  glm::vec3 edge2 = ps[2] - ps[0];
  glm::vec2 deltaUV1 = ts[1] - ts[0];
  glm::vec2 deltaUV2 = ts[2] - ts[0];

  glm::vec3 tan = getTangent(deltaUV2, deltaUV1, edge1, edge2);
  glm::vec3 bitan = getBiTangent(deltaUV2, deltaUV1, edge1, edge2);

  vs[0] = {ps[0], n, ts[0], tan, bitan};
  vs[1] = {ps[1], n, ts[1], tan, bitan};
  vs[2] = {ps[2], n, ts[2], tan, bitan};
}
std::vector<Vertex> mk_triangle_vertices(const glm::vec3 ps[3],
                                         const glm::vec2 ts[3],
                                         const glm::vec3 &n) {
  Vertex vs[3] = {Vertex(), Vertex(), Vertex()};
  mk_triangle_vertices(ps, ts, n, vs);
  std::vector<Vertex> v = {vs[0], vs[1], vs[2]};
  return v;
}
/** create rectangle vertices == two triangles*/
void mk_rectangle_vertices(const glm::vec3 &btmleft, const glm::vec3 &tpright,
                           const glm::vec3 &tpleft, const glm::vec3 &btmright,
                           const glm::vec2 &btmleft_tx,
                           const glm::vec2 &tpright_tx,
                           const glm::vec2 &tpleft_tx,
                           const glm::vec2 &btmright_tx, const glm::vec3 &n,
                           Vertex rect_vs[6], unsigned int indices[6]) {

  const glm::vec3 first_ps[3] = {tpright, btmright, tpleft};
  const glm::vec2 first_ts[3] = {tpright_tx, btmright_tx, tpleft_tx};
  Vertex *first_triangle = new Vertex[3];
  mk_triangle_vertices(first_ps, first_ts, n, first_triangle);

  // second triangle
  const glm::vec3 second_ps[3] = {btmright, btmleft, tpleft};
  const glm::vec2 second_ts[3] = {btmright_tx, btmleft_tx, tpleft_tx};
  Vertex *second_triangle = new Vertex[3];
  mk_triangle_vertices(second_ps, second_ts, n, second_triangle);
  for (unsigned int i = 0; i < 3; i++) {
    rect_vs[i] = first_triangle[i];
    indices[i] = i;
  }
  for (unsigned int i = 3; i < 6; i++) {
    rect_vs[i] = second_triangle[i];
    indices[i] = i;
  }
}
std::vector<Vertex> triangle_vertices() {
  std::vector<Vertex> vs = {Vertex(-0.5, -0.5, -0.5, 0.0, 0.0, -1.0, 0.0, 0.0f),
                            Vertex(0.5, -0.5, -0.5, 0.0, 0.0, -1.0, 1.0, 0.0),
                            Vertex(0.5, 0.5, -0.5, 0.0, 0.0, -1.0, 1.0, 1.0)};
  return vs;
}
std::vector<unsigned int> triangle_indices() {
  std::vector<unsigned int> indices = {0, 1, 2};
  return indices;
}
std::vector<Vertex> quat_vertices() {
  // TODO change vertex coordinates
  std::vector<Vertex> vs = {//
                            Vertex(-1.0, 1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0),
                            Vertex(-1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0),
                            Vertex(1.0, 1.0, 0.0, 0.0, 0.0, -1.0, 1.0, 1.0),
                            Vertex(1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 1.0, 0.0)};
  return vs;
}
std::vector<unsigned int> quat_indices() {
  std::vector<unsigned int> indices = {0, 1, 2, 1, 2, 3};
  return indices;
}
std::vector<Vertex> cube_vertices() {
  std::vector<Vertex> vertices = {
      // first triangle
      //  position         // normal         // texture
      //  coord
      Vertex(-1.0, -1.0, -1.0, 0.0, 0.0, -1.0, 0.0,
             0.0), // bottom-left
      Vertex(1.0, 1.0, -1.0, 0.0, 0.0, -1.0, 1.0,
             1.0), // top-right
      Vertex(1.0, -1.0, -1.0, 0.0, 0.0, -1.0, 1.0,
             0.0), // bottom-right

      // second triangle
      Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,
             1.0f), // top-right
      Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
             0.0f), // bottom-left
      Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
             1.0f), // top-left
                    // front face
      Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.0f), // bottom-left
      Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             0.0f), // bottom-right
      Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             1.0f), // top-right
      Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             1.0f), // top-right
      Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             1.0f), // top-left
      Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.0f), // bottom-left
                    // left face
      Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
             0.0f), // top-right
      Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
             1.0f), // top-left
      Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f), // bottom-left
      Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f), // bottom-left
      Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             0.0f), // bottom-right
      Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
             0.0f), // top-right
                    // right face
      Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.0f), // top-left
      Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             1.0f), // bottom-right
      Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             1.0f), // top-right
      Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             1.0f), // bottom-right
      Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.0f), // top-left
      Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             0.0f), // bottom-left
                    // bottom face
      Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
             1.0f), // top-right
      Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
             1.0f), // top-left
      Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
             0.0f), // bottom-left
      Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
             0.0f), // bottom-left
      Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
             0.0f), // bottom-right
      Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
             1.0f), // top-right
                    // top face
      Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f), // top-left
      Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.0f), // bottom-right
      Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             1.0f), // top-right
      Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.0f), // bottom-right
      Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f), // top-left
      Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.0f) // bottom-left
  };
  return vertices;
};

std::vector<unsigned int> cube_indices() {
  auto vs = cube_vertices();
  std::vector<unsigned int> indices;
  for (unsigned int i = 0; i < vs.size(); i++) {
    indices.push_back(i);
  }
  return indices;
}
}; // namespace vivademos

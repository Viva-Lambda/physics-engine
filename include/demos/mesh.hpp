#pragma once
// mesh object and other related drawing
#include <demos/meshutils.hpp>
#include <demos/shader.hpp>
#include <demos/texture.hpp>
#include <demos/vertex.hpp>
#include <external.hpp>
using namespace vivademos;
namespace vivademos {

class Mesh {
  //
public:
  Vertex *vertices;
  Texture *textures;
  unsigned int *indices;
  //
  unsigned int vertex_nb;
  unsigned int index_nb;
  unsigned int texture_nb;
  //
  GLuint vao;
  Mesh()
      : vertices(nullptr), textures(nullptr),
        indices(nullptr), vertex_nb(0), index_nb(0),
        texture_nb(0) {}
  Mesh(Vertex *vs, Texture *txts, unsigned int *inds,
       unsigned int vnb, unsigned int inb, unsigned int tnb)
      : vertices(vs), indices(inds), textures(txts),
        vertex_nb(vnb), index_nb(inb), texture_nb(tnb) {
    setupMesh();
  }
  void bind_textures(Shader shdr) {
    unsigned int diffuseNb = 1;
    unsigned int specularNb = 1;
    unsigned int normalNb = 1;
    unsigned int heightNb = 1;
    unsigned int dispNb = 1;
    unsigned int aoNb = 1;
    unsigned int roughNb = 1;
    unsigned int metalNb = 1;
    unsigned int albedoNb = 1;
    for (int i = 0; i < texture_nb; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      std::string nb;
      TextureType ttype = textures[i].type;
      std::string tname = textures[i].name;
      switch (ttype) {
      case TextureType::Diffuse:
        nb = std::to_string(diffuseNb++);
        break;
      case TextureType::Specular:
        nb = std::to_string(specularNb++);
        break;
      case TextureType::Normal:
        nb = std::to_string(normalNb++);
        break;
      case TextureType::Height:
        nb = std::to_string(heightNb++);
        break;
      case TextureType::Displacement:
        nb = std::to_string(dispNb++);
        break;
      case TextureType::Ao:
        nb = std::to_string(aoNb++);
        break;
      case TextureType::Roughness:
        nb = std::to_string(roughNb++);
        break;
      case TextureType::Albedo:
        nb = std::to_string(albedoNb++);
        break;
      case TextureType::Metallic:
        nb = std::to_string(metalNb++);
        break;
      }
      shdr.setIntUni(tname + nb, i);
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
  }
  void draw(Shader shdr) {
    // bind textures
    if (texture_nb > 0) {
      bind_textures(shdr);
    }
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, index_nb, GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);
    //
    glActiveTexture(GL_TEXTURE0);
  }

private:
  GLuint ebo, vbo;

  void setupMesh() {
    //
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // bind vao
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //
    glBufferData(GL_ARRAY_BUFFER,
                 vertex_nb * sizeof(Vertex), &vertices[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 index_nb * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    //
    // vertex pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)0);

    // vertex normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    // texcoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void *)offsetof(Vertex, texCoord));

    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void *)offsetof(Vertex, tangent));
    // bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void *)offsetof(Vertex, bitangent));

    //
    glBindVertexArray(0);
  }
};

/**Make a triangle mesh*/
Mesh mk_triangle(const glm::vec3 ps[3],
                 const glm::vec2 ts[3],
                 const glm::vec3 &n) {
  //
  unsigned int vnb = 3;
  Vertex *vs = new Vertex[vnb];
  mk_triangle_vertices(ps, ts, n, vs);

  unsigned int inds[] = {0, 1, 2};
  return Mesh(vs, nullptr, inds, vnb, vnb, 0);
};
/**Make a cube mesh*/
Mesh mk_cube() {
  Vertex cube_vs[36];
  unsigned int cube_indices[36];
  // front face
  glm::vec3 front_normal(0.0f, 0.0f, -1.0f);
  //
  glm::vec3 front_btm_left(-1.0f, -1.0f, -1.0f);
  glm::vec2 front_btm_left_tx(0.0f, 0.0f);
  glm::vec3 front_btm_r(1.0f, -1.0f, -1.0f);
  glm::vec2 front_btm_r_tx(1.0f, 0.0f);
  glm::vec3 front_tp_left(-1.0f, 1.0f, -1.0f);
  glm::vec2 front_tp_left_tx(0.0f, 1.0f);
  glm::vec3 front_tp_r(1.0f, 1.0f, -1.0f);
  glm::vec2 front_tp_r_tx(1.0f, 1.0f);
  Vertex front_face[6];
  unsigned int front_indices[6];
  mk_rectangle_vertices(
      front_btm_left, front_tp_r, front_tp_left,
      front_btm_r, front_btm_left_tx, front_tp_r_tx,
      front_tp_left_tx, front_btm_r_tx, front_normal,
      front_face, front_indices);
  unsigned int oset = 0;
  unsigned int stride = 6;
  for (unsigned int i = 0; i < stride; i++) {
    cube_vs[i + oset] = front_face[i];
    cube_indices[i + oset] = i + oset;
  }
  oset += 6;

  // left face
  glm::vec3 left_normal(0.0f, 0.0f, 1.0f);
  //
  glm::vec3 left_btm_left(-1.0f, -1.0f, 1.0f);
  glm::vec2 left_btm_left_tx(0.0f, 0.0f);
  glm::vec3 left_btm_r(1.0f, -1.0f, 1.0f);
  glm::vec2 left_btm_r_tx(1.0f, 0.0f);
  glm::vec3 left_tp_left(-1.0f, 1.0f, 1.0f);
  glm::vec2 left_tp_left_tx(0.0f, 1.0f);
  glm::vec3 left_tp_r(1.0f, 1.0f, 1.0f);
  glm::vec2 left_tp_r_tx(1.0f, 1.0f);

  Vertex left_face[6];
  unsigned int left_indices[6];
  mk_rectangle_vertices(
      left_btm_left, left_tp_r, left_tp_left, left_btm_r,
      left_btm_left_tx, left_tp_r_tx, left_tp_left_tx,
      left_btm_r_tx, left_normal, left_face, left_indices);

  for (unsigned int i = 0; i < stride; i++) {
    cube_vs[i + oset] = left_face[i];
    cube_indices[i + oset] = i + oset;
  }
  oset += 6;

  // right face
  glm::vec3 right_normal(-1.0f, 0.0f, 0.0f);
  //
  glm::vec3 right_btm_left(-1.0f, -1.0f, -1.0f);
  glm::vec2 right_btm_left_tx(0.0f, 1.0f);
  glm::vec3 right_btm_r(-1.0f, -1.0f, 1.0f);
  glm::vec2 right_btm_r_tx(0.0f, 0.0f);
  glm::vec3 right_tp_left(-1.0f, 1.0f, -1.0f);
  glm::vec2 right_tp_left_tx(1.0f, 1.0f);
  glm::vec3 right_tp_r(-1.0f, 1.0f, 1.0f);
  glm::vec2 right_tp_r_tx(1.0f, 0.0f);
  Vertex right_face[6];
  unsigned int right_indices[6];
  mk_rectangle_vertices(
      right_btm_left, right_tp_r, right_tp_left,
      right_btm_r, right_btm_left_tx, right_tp_r_tx,
      right_tp_left_tx, right_btm_r_tx, right_normal,
      right_face, right_indices);

  for (unsigned int i = 0; i < stride; i++) {
    cube_vs[i + oset] = right_face[i];
    cube_indices[i + oset] = i + oset;
  }
  oset += 6;

  // bottom face
  glm::vec3 bottom_normal(1.0f, 0.0f, 1.0f);
  //
  glm::vec3 bottom_btm_left(1.0f, -1.0f, 1.0f);
  glm::vec2 bottom_btm_left_tx(1.0f, 0.0f);

  glm::vec3 bottom_btm_r(1.0f, -1.0f, -1.0f);
  glm::vec2 bottom_btm_r_tx(0.0f, 1.0f);

  glm::vec3 bottom_tp_left(1.0f, 1.0f, 1.0f);
  glm::vec2 bottom_tp_left_tx(1.0f, 0.0f);

  glm::vec3 bottom_tp_r(1.0f, 1.0f, -1.0f);
  glm::vec2 bottom_tp_r_tx(1.0f, 1.0f);

  Vertex bottom_face[6];
  unsigned int bottom_indices[6];
  mk_rectangle_vertices(
      bottom_btm_left, bottom_tp_r, bottom_tp_left,
      bottom_btm_r, bottom_btm_left_tx, bottom_tp_r_tx,
      bottom_tp_left_tx, bottom_btm_r_tx, bottom_normal,
      bottom_face, bottom_indices);

  for (unsigned int i = 0; i < stride; i++) {
    cube_vs[i + oset] = bottom_face[i];
    cube_indices[i + oset] = i + oset;
  }
  oset += 6;

  // top face
  glm::vec3 top_normal(0.0f, -1.0f, 0.0f);
  //
  glm::vec3 top_btm_left(1.0f, -1.0f, 1.0f);
  glm::vec2 top_btm_left_tx(1.0f, 0.0f);

  glm::vec3 top_btm_r(-1.0f, -1.0f, 1.0f);
  glm::vec2 top_btm_r_tx(0.0f, 0.0f);

  glm::vec3 top_tp_left(1.0f, -1.0f, -1.0f);
  glm::vec2 top_tp_left_tx(1.0f, 1.0f);

  glm::vec3 top_tp_r(-1.0f, -1.0f, -1.0f);
  glm::vec2 top_tp_r_tx(0.0f, 1.0f);

  Vertex top_face[6];
  unsigned int top_indices[6];
  mk_rectangle_vertices(
      top_btm_left, top_tp_r, top_tp_left, top_btm_r,
      top_btm_left_tx, top_tp_r_tx, top_tp_left_tx,
      top_btm_r_tx, top_normal, top_face, top_indices);

  for (unsigned int i = 0; i < stride; i++) {
    cube_vs[i + oset] = top_face[i];
    cube_indices[i + oset] = i + oset;
  }
  oset += 6;

  // some face
  glm::vec3 some_normal(0.0f, 1.0f, 0.0f);
  //
  glm::vec3 some_btm_left(-1.0f, 1.0f, 1.0f);
  glm::vec2 some_btm_left_tx(0.0f, 0.0f);

  glm::vec3 some_btm_r(1.0f, 1.0f, 1.0f);
  glm::vec2 some_btm_r_tx(1.0f, 0.0f);

  glm::vec3 some_tp_left(-1.0f, 1.0f, -1.0f);
  glm::vec2 some_tp_left_tx(0.0f, 1.0f);

  glm::vec3 some_tp_r(1.0f, 1.0f, -1.0f);
  glm::vec2 some_tp_r_tx(1.0f, 1.0f);

  Vertex some_face[6];
  unsigned int some_indices[6];
  mk_rectangle_vertices(
      some_btm_left, some_tp_r, some_tp_left, some_btm_r,
      some_btm_left_tx, some_tp_r_tx, some_tp_left_tx,
      some_btm_r_tx, some_normal, some_face, some_indices);

  for (unsigned int i = 0; i < stride; i++) {
    cube_vs[i + oset] = some_face[i];
    cube_indices[i + oset] = i + oset;
  }
  oset += 6;
  return Mesh(cube_vs, nullptr, cube_indices, oset, oset,
              0);
};
};

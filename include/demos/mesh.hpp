#pragma once
// mesh object and other related drawing
#include <demos/meshutils.hpp>
#include <demos/rendershape.hpp>
#include <demos/shader.hpp>
#include <demos/texture.hpp>
#include <demos/vertex.hpp>
#include <demos/vmaker.hpp>
#include <external.hpp>
#include <vivaphysics/particle.hpp>

using namespace vivademos;

namespace vivademos {

struct Mesh {

  // opengl related
  GLuint vao;
  GLuint vbo;
  GLuint ebo;

  //
  std::vector<Vertex> vertices;
  std::vector<Texture> textures;
  std::vector<unsigned int> indices;
  Mesh() {}
  Mesh(std::vector<Vertex> vs, std::vector<Texture> ts,
       std::vector<unsigned int> ids)
      : vertices(vs), textures(ts), indices(ids) {
    setup_mesh();
  }

  void draw(Shader shdr) {
    //
    // bind textures
    unsigned int diffuseNb = 1;
    unsigned int specularNb = 1;
    unsigned int normalNb = 1;
    unsigned int heightNb = 1;
    unsigned int dispNb = 1;
    unsigned int aoNb = 1;
    unsigned int roughNb = 1;
    unsigned int metalNb = 1;
    unsigned int albedoNb = 1;
    for (unsigned int i = 0; i < textures.size(); i++) {
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
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //
    glActiveTexture(GL_TEXTURE0);
  }

  void setup_mesh() {
    //
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // bind vao
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    //
    // vertex pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
    // texcoord
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, texCoord));

    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, tangent));
    // bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, bitangent));

    //
    glBindVertexArray(0);
  }
};

struct SimpleCubeMesh : Mesh {
  SimpleCubeMesh() {
    //
    vertices = cube_vertices();
    indices = cube_indices();
    std::vector<Texture> ts;
    textures = ts;
    setup_mesh();
  }
};
struct SimpleTriangleMesh : Mesh {
  SimpleTriangleMesh() {
    //
    vertices = triangle_vertices();
    indices = triangle_indices();
    std::vector<Texture> ts;
    textures = ts;
    setup_mesh();
  }
};
struct SimpleQuatMesh : Mesh {
  SimpleQuatMesh() {
    //
    vertices = quat_vertices();
    indices = quat_indices();
    std::vector<Texture> ts;
    textures = ts;
    setup_mesh();
  }
};

}; // namespace vivademos

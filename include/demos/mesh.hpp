#pragma once
// mesh object and other related drawing
#include <demos/meshutils.hpp>
#include <demos/shader.hpp>
#include <demos/texture.hpp>
#include <demos/vertex.hpp>
#include <external.hpp>
#include <vivaphysics/particle.hpp>

using namespace vivademos;

namespace vivademos {

class TriangleSurface {
public:
  static const unsigned int element_nb = 3;
  Vertex p1;
  Vertex p2;
  Vertex p3;

  unsigned int vertex_nb = element_nb;
  const unsigned int index_nb = element_nb;
  const unsigned int indices[element_nb] = {0, 1, 2};
  Texture *textures;
  unsigned int texture_nb;
  GLuint vao;
  TriangleSurface(const TriangleSurface &t) {
    texture_nb = t.texture_nb;
    if (texture_nb > 0) {
      textures = new Texture[texture_nb];
      for (unsigned int i = 0; i < texture_nb; i++) {
        textures[i] = t.textures[i];
      }
    } else {
      textures = nullptr;
    }
    p1 = t.p1;
    p2 = t.p2;
    p3 = t.p3;
    vao = t.vao;
    vbo = t.vbo;
    ebo = t.ebo;
  }
  TriangleSurface()
      : textures(nullptr), texture_nb(0), vao(0) {}
  void destroy() {
    if (textures != nullptr)
      delete[] textures;
    vao = 0;
    vbo = 0;
    ebo = 0;
  }
  TriangleSurface(const Vertex &v1, const Vertex &v2,
                  const Vertex &v3, Texture *txts,
                  unsigned int tnb)
      : textures(txts), texture_nb(tnb), p1(v1), p2(v2),
        p3(v3) {
    setup();
  }
  TriangleSurface(Vertex vs[element_nb], Texture *txts,
                  unsigned int tnb)
      : textures(txts), texture_nb(tnb), p1(vs[0]),
        p2(vs[1]), p3(vs[2]) {
    setup();
  }

  /**copy assignment operator*/
  TriangleSurface &operator=(const TriangleSurface &t) {
    //
    p1 = t.p1;
    p2 = t.p2;
    p3 = t.p3;
    vao = t.vao;
    vbo = t.vbo;
    ebo = t.ebo;
    texture_nb = t.texture_nb;
    if (texture_nb > 0) {
      textures = new Texture[texture_nb];
      for (unsigned int i = 0; i < texture_nb; i++) {
        textures[i] = t.textures[i];
      }
    } else {
      textures = nullptr;
    }

    return *this;
  }

private:
  GLuint ebo, vbo;

  void setup() {
    // setup opengl friendly object
    Vertex vertices[] = {p1, p2, p3};
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

    // bind indices
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

public:
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
};

class Mesh {
public:
  unsigned int nb_triangles;
  TriangleSurface *ts;
  Mesh() : nb_triangles(0), ts(nullptr) {}
  //~Mesh() { delete[] ts; }

  Mesh(TriangleSurface *triangles, unsigned int nbt)
      : nb_triangles(nbt) {
    ts = new TriangleSurface[nb_triangles];
    for (unsigned int i = 0; i < nb_triangles; i++) {
      ts[i] = triangles[i];
    }
  }
  Mesh(const Mesh &m)
      : ts(m.ts), nb_triangles(m.nb_triangles) {}
  void draw(Shader shdr) {
    for (unsigned int i = 0; i < nb_triangles; i++) {
      //
      ts[i].draw(shdr);
    }
  }
  void destroy() {
    for (unsigned int i = 0; i < nb_triangles; i++) {
      //
      ts[i].destroy();
    }
  }
};

enum ShotType {
  UNUSED = 0,
  PISTOL = 1,
  ARTILLERY = 2,
  FIREBALL = 3,
  LASER = 4
};

class AmmoRound : public Mesh {
public:
  vivaphysics::Particle particle;
  ShotType stype;
  unsigned int start_time;
  AmmoRound() : Mesh() {}
  AmmoRound(TriangleSurface *triangles, unsigned int nbt,
            ShotType st)
      : Mesh(triangles, nbt), stype(st) {}
  AmmoRound(const Mesh &m) : Mesh(m), stype(UNUSED) {}
  glm::mat4 set_model_mat(glm::mat4 mmat) const {
    vivaphysics::point3 pos;
    particle.get_position(pos);
    auto posv = pos.to_glm();
    return glm::translate(mmat, posv);
  }
};

/**Make a triangle surface*/
TriangleSurface mk_triangle_surface(const glm::vec3 ps[3],
                                    const glm::vec2 ts[3],
                                    const glm::vec3 &n) {
  //
  Vertex *vs = new Vertex[TriangleSurface::element_nb];
  mk_triangle_vertices(ps, ts, n, vs);
  return TriangleSurface(vs, nullptr, 0);
};
TriangleSurface mk_triangle_surface(const glm::vec3 &p1,
                                    const glm::vec3 &p2,
                                    const glm::vec3 &p3,
                                    const glm::vec2 &t1,
                                    const glm::vec2 &t2,
                                    const glm::vec2 &t3,
                                    const glm::vec3 &n) {
  const glm::vec3 ps[] = {p1, p2, p3};
  const glm::vec2 ts[] = {t1, t2, t3};
  return mk_triangle_surface(ps, ts, n);
}

TriangleSurface mk_triangle_surface(float vs[15],
                                    float n[3]) {
  glm::vec3 p1(vs[0], vs[1], vs[2]);
  glm::vec2 t1(vs[3], vs[4]);
  glm::vec3 p2(vs[5], vs[6], vs[7]);
  glm::vec2 t2(vs[8], vs[9]);
  glm::vec3 p3(vs[10], vs[11], vs[12]);
  glm::vec2 t3(vs[13], vs[14]);
  glm::vec3 norm(n[0], n[1], n[2]);

  const glm::vec3 ps[] = {p1, p2, p3};
  const glm::vec2 ts[] = {t1, t2, t3};
  return mk_triangle_surface(ps, ts, norm);
}

/**Make a triangle mesh*/
Mesh mk_triangle(const glm::vec3 ps[3],
                 const glm::vec2 ts[3],
                 const glm::vec3 &n) {
  auto t = mk_triangle_surface(ps, ts, n);
  return Mesh(&t, 1);
};
/**Make a cube mesh*/
Mesh mk_cube() {
  // front face
  float s1n[] = {0.0f, 0.0f, -1.0f};
  float s2n[] = {0.0f, 0.0f, 1.0f};
  float s3n[] = {-1.0f, 0.0f, 0.0f};
  float s4n[] = {1.0f, 0.0f, 0.0f};
  float s5n[] = {0.0f, -1.0f, 0.0f};
  float s6n[] = {0.0f, 1.0f, 0.0f};

  // positions        // texture coords
  float t1[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,  0.5f, -0.5f, -0.5f,
      1.0f,  0.0f,  0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  };
  auto tri1 = mk_triangle_surface(t1, s1n);
  float tt1[] = {
      0.5f, 0.5f, -0.5f, 1.0f,  1.0f,  -0.5f, 0.5f, -0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,
  };
  auto tri2 = mk_triangle_surface(tt1, s1n);

  float t2[] = {
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f,
      1.0f,  0.0f,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
  };

  auto tri3 = mk_triangle_surface(t2, s2n);
  float tt2[] = {
      0.5f, 0.5f, 0.5f,  1.0f,  1.0f, -0.5f, 0.5f, 0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,
  };
  auto tri4 = mk_triangle_surface(tt2, s2n);

  float t3[] = {
      -0.5f, 0.5f, 0.5f,  1.0,   0.0f,  -0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };
  auto tri5 = mk_triangle_surface(t3, s3n);

  float tt3[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  -0.5f, 0.5f, 0.5f, 1.0f,  0.0f,
  };
  auto tri6 = mk_triangle_surface(tt3, s3n);

  float t4[] = {
      0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.5f, 0.5f, -0.5f,
      1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  };
  auto tri7 = mk_triangle_surface(t4, s4n);
  float tt4[] = {
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };
  auto tri8 = mk_triangle_surface(tt4, s4n);

  float t5[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, -0.5f, -0.5f,
      1.0f,  1.0f,  0.5f,  -0.5f, 0.5f, 1.0f, 0.0f,
  };

  auto tri9 = mk_triangle_surface(t5, s5n);

  float tt5[] = {
      0.5f, -0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };
  auto tri10 = mk_triangle_surface(tt5, s5n);

  float t6[] = {
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, 0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  auto tri11 = mk_triangle_surface(t6, s6n);

  float tt6[] = {0.5f,  0.5f, 0.5f,  1.0f, 0.0f,
                 -0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
                 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
  auto tri12 = mk_triangle_surface(tt6, s6n);
  TriangleSurface tris[] = {tri1, tri2,  tri3,  tri4,
                            tri5, tri6,  tri7,  tri8,
                            tri9, tri10, tri11, tri12};
  return Mesh(tris, 12);
};

Mesh mk_plane() {
  // from learnopengl
  // ------------------------------------------------------------------
  glm::vec3 ftri_pos[3] = {
      //
      glm::vec3(25.0f, -0.5f, 25.0f),
      glm::vec3(-25.0f, -0.5f, 25.0f),
      glm::vec3(-25.0f, -0.5f, -25.0f)};
  glm::vec2 ftri_tex[3] = {
      glm::vec2(25.0f, 0.0f), glm::vec2(0.0f, 0.0f),
      glm::vec2(0.0f, 25.0f),
  };
  glm::vec3 f_norm = glm::vec3(0.0f, 1.0f, 0.0f);
  //
  glm::vec3 stri_pos[3] = {glm::vec3(25.0f, -0.5f, 25.0f),
                           glm::vec3(-25.0f, -0.5f, -25.0f),
                           glm::vec3(25.0f, -0.5f, -25.0f)};
  glm::vec2 stri_tex[3] = {glm::vec2(25.0f, 0.0f),
                           glm::vec2(0.0f, 25.0f),
                           glm::vec2(25.0f, 25.0f)};
  glm::vec3 s_norm = glm::vec3(0.0f, 1.0f, 0.0f);

  auto tri1 =
      mk_triangle_surface(ftri_pos, ftri_tex, f_norm);
  auto tri2 =
      mk_triangle_surface(stri_pos, stri_tex, s_norm);
  TriangleSurface tris[] = {tri1, tri2};
  return Mesh(tris, 2);
}

/** make cube ammo*/
AmmoRound mk_cube_ammo() {
  auto mesh = mk_cube();
  return AmmoRound(mesh);
}
};

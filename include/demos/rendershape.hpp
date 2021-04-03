#pragma once
// simple shapes for rendering
#include <demos/debug.hpp>
#include <external.hpp>

using namespace vivademos;

namespace vivademos {

struct VertexAttrib {
  GLuint loc;
  GLuint size;
  VertexAttrib(GLuint i, GLuint j) : loc(i), size(j) {}
};
bool compareAttribs(const VertexAttrib &v1,
                    const VertexAttrib &v2) {
  return v1.loc < v2.loc;
}

/**
  Set vertex attributes of a shader
 */
void setVertexAttrib(const VertexAttrib *vs,
                     unsigned int vnb) {
  size_t fsize = 0 * sizeof(float);
  for (unsigned int i = 0; i < vnb; i++) {
    fsize += vs[i].size * sizeof(float);
  }
  // now to locations
  size_t voffset = 0 * sizeof(float); // attribute offset
  for (unsigned int i = 0; i < vnb; i++) {
    auto v = vs[i];
    glEnableVertexAttribArray(v.loc); // location
    glVertexAttribPointer(v.loc,      // location ==  aPos
                          v.size,     // vec3
                          GL_FLOAT, GL_FALSE, fsize,
                          (void *)voffset);
    voffset += v.size * sizeof(float);
  }
}
void setVertexAttrib(std::array<VertexAttrib, 2> vs) {

  // deal with size
  std::sort(vs.begin(), vs.end(),
            compareAttribs); // 1,2,3,4
  setVertexAttrib(vs.data(), vs.size());
}
void setVertexAttrib(std::array<VertexAttrib, 3> vs) {

  // deal with size
  std::sort(vs.begin(), vs.end(),
            compareAttribs); // 1,2,3,4
  setVertexAttrib(vs.data(), vs.size());
}
void setVertexAttrib(std::array<VertexAttrib, 4> vs) {

  // deal with size
  std::sort(vs.begin(), vs.end(),
            compareAttribs); // 1,2,3,4
  setVertexAttrib(vs.data(), vs.size());
}

void setVertexAttrib(std::array<VertexAttrib, 5> vs) {

  // deal with size
  std::sort(vs.begin(), vs.end(),
            compareAttribs); // 1,2,3,4
  setVertexAttrib(vs.data(), vs.size());
}

void renderTriangleInTangentSpace(
    float vert[15], float normal[3],
    std::array<VertexAttrib, 5> vs) {
  GLuint triVBO, triVAO;
  glGenBuffers(1, &triVBO);
  glGenVertexArrays(1, &triVAO);

  // triangle points
  glm::vec3 p1(vert[0], vert[1], vert[2]);
  glm::vec2 tex1(vert[3], vert[4]);
  glm::vec3 p2(vert[5], vert[6], vert[7]);
  glm::vec2 tex2(vert[8], vert[9]);
  glm::vec3 p3(vert[10], vert[11], vert[12]);
  glm::vec2 tex3(vert[13], vert[14]);

  // there are two triangles in a square
  glm::vec3 snormal(normal[0], normal[1], normal[2]);

  // first triangle
  glm::vec3 edge1 = p2 - p1;
  glm::vec3 edge2 = p3 - p1;
  glm::vec2 deltaUV1 = tex2 - tex1;
  glm::vec2 deltaUV2 = tex3 - tex1;

  glm::vec3 tan =
      getTangent(deltaUV2, deltaUV1, edge1, edge2);
  glm::vec3 bitan =
      getBiTangent(deltaUV2, deltaUV1, edge1, edge2);

  float trivert[] = {
      p1.x,      p1.y,      p1.z,      snormal.x, snormal.y,
      snormal.z, tex1.x,    tex1.y,    tan.x,     tan.y,
      tan.z,     bitan.x,   bitan.y,   bitan.z,   p2.x,
      p2.y,      p2.z,      snormal.x, snormal.y, snormal.z,
      tex2.x,    tex2.y,    tan.x,     tan.y,     tan.z,
      bitan.x,   bitan.y,   bitan.z,   p3.x,      p3.y,
      p3.z,      snormal.x, snormal.y, snormal.z, tex3.x,
      tex3.y,    tan.x,     tan.y,     tan.z,     bitan.x,
      bitan.y,   bitan.z,
  };
  glBindVertexArray(triVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(trivert), &trivert,
               GL_STATIC_DRAW);
  // specify attributes
  setVertexAttrib(vs);
  gerr();
  glBindVertexArray(triVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &triVAO);
  glDeleteBuffers(1, &triVBO);
}

void renderTriangleInTangentSpace(float vert[15],
                                  float normal[3]) {
  std::array<VertexAttrib, 5> vs{
      VertexAttrib(0, 3), // aPos
      VertexAttrib(1, 3), // aNormal
      VertexAttrib(2, 2), // aTexCoord
      VertexAttrib(3, 3), // aTan
      VertexAttrib(4, 3), // aBitan
  };
  renderTriangleInTangentSpace(vert, normal, vs);
}

void renderTriangle(float vert[15], float normal[3],
                    std::array<VertexAttrib, 3> vs) {
  GLuint triVBO, triVAO;
  glGenBuffers(1, &triVBO);
  glGenVertexArrays(1, &triVAO);

  // there are two triangles in a square
  float trivert[] = {
      vert[0],   vert[1],   vert[2],   normal[0], normal[1],
      normal[2], vert[3],   vert[4],   vert[5],   vert[6],
      vert[7],   normal[0], normal[1], normal[2], vert[8],
      vert[9],   vert[10],  vert[11],  vert[12],  normal[0],
      normal[1], normal[2], vert[13],  vert[14],
  };
  glBindVertexArray(triVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(trivert), &trivert,
               GL_STATIC_DRAW);
  // specify attributes
  setVertexAttrib(vs);
  glBindVertexArray(triVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &triVAO);
  glDeleteBuffers(1, &triVBO);
}
void renderTriangle(float vert[15], float normal[3]) {
  std::array<VertexAttrib, 3> vs{VertexAttrib(0, 3),
                                 VertexAttrib(1, 3),
                                 VertexAttrib(2, 2)};
  renderTriangle(vert, normal, vs);
}
void renderLamp() {
  GLuint vbo, lightVao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(
      1, &lightVao); // separate object to isolate lamp from
  float vert[] = {-0.5f, -0.5f, -0.5f, -0.5f, 0.5f,
                  -0.5f, 0.5f,  -0.5f, -0.5f};

  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vert), &vert,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), (void *)0);
  glBindVertexArray(lightVao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &lightVao);
  glDeleteBuffers(1, &vbo);
}
void renderCubeInTangentSpace(
    std::array<VertexAttrib, 5> vs) {
  /*
     Draw cube
   */
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
  renderTriangleInTangentSpace(t1, s1n, vs);
  float tt1[] = {
      0.5f, 0.5f, -0.5f, 1.0f,  1.0f,  -0.5f, 0.5f, -0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,
  };
  renderTriangleInTangentSpace(tt1, s1n, vs);

  float t2[] = {
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f,
      1.0f,  0.0f,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
  };

  renderTriangleInTangentSpace(t2, s2n, vs);
  float tt2[] = {
      0.5f, 0.5f, 0.5f,  1.0f,  1.0f, -0.5f, 0.5f, 0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,
  };

  renderTriangleInTangentSpace(tt2, s2n, vs);

  float t3[] = {
      -0.5f, 0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };
  renderTriangleInTangentSpace(t3, s3n, vs);

  float tt3[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  -0.5f, 0.5f, 0.5f, 1.0f,  0.0f,
  };
  renderTriangleInTangentSpace(tt3, s3n, vs);

  float t4[] = {
      0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.5f, 0.5f, -0.5f,
      1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  };

  renderTriangleInTangentSpace(t4, s4n, vs);
  float tt4[] = {
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangleInTangentSpace(tt4, s4n, vs);

  float t5[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, -0.5f, -0.5f,
      1.0f,  1.0f,  0.5f,  -0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangleInTangentSpace(t5, s5n, vs);

  float tt5[] = {
      0.5f, -0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };

  renderTriangleInTangentSpace(tt5, s5n, vs);

  float t6[] = {
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, 0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangleInTangentSpace(t6, s6n, vs);

  float tt6[] = {0.5f,  0.5f, 0.5f,  1.0f, 0.0f,
                 -0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
                 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

  renderTriangleInTangentSpace(tt6, s6n, vs);
}
void renderCubeInTangentSpaceS() {
  /*
     Draw cube
   */
  std::array<VertexAttrib, 5> vs{
      VertexAttrib(0, 3), // aPos
      VertexAttrib(1, 3), // aNormal
      VertexAttrib(2, 2), // aTexCoord
      VertexAttrib(3, 3), // aTan
      VertexAttrib(4, 3), // aBitan
  };
  renderCubeInTangentSpace(vs);
}
void renderCube(std::array<VertexAttrib, 3> vs) {
  /*
     Draw cube
   */
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
  renderTriangle(t1, s1n, vs);
  float tt1[] = {
      0.5f, 0.5f, -0.5f, 1.0f,  1.0f,  -0.5f, 0.5f, -0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,
  };
  renderTriangle(tt1, s1n, vs);

  float t2[] = {
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f,
      1.0f,  0.0f,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
  };

  renderTriangle(t2, s2n, vs);
  float tt2[] = {
      0.5f, 0.5f, 0.5f,  1.0f,  1.0f, -0.5f, 0.5f, 0.5f,
      0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,
  };

  renderTriangle(tt2, s2n, vs);

  float t3[] = {
      -0.5f, 0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };
  renderTriangle(t3, s3n, vs);

  float tt3[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,
      0.0f,  0.0f,  -0.5f, 0.5f, 0.5f, 1.0f,  0.0f,
  };
  renderTriangle(tt3, s3n, vs);

  float t4[] = {
      0.5f, 0.5f, 0.5f, 1.0f,  0.0f,  0.5f, 0.5f, -0.5f,
      1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  };

  renderTriangle(t4, s4n, vs);
  float tt4[] = {
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangle(tt4, s4n, vs);

  float t5[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, -0.5f, -0.5f,
      1.0f,  1.0f,  0.5f,  -0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangle(t5, s5n, vs);

  float tt5[] = {
      0.5f, -0.5f, 0.5f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,
  };

  renderTriangle(tt5, s5n, vs);

  float t6[] = {
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f,
      1.0f,  1.0f, 0.5f,  0.5f, 0.5f, 1.0f, 0.0f,
  };

  renderTriangle(t6, s6n, vs);

  float tt6[] = {0.5f,  0.5f, 0.5f,  1.0f, 0.0f,
                 -0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
                 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

  renderTriangle(tt6, s6n, vs);
}
void renderCubeS() {
  /*
     Draw cube
   */
  std::array<VertexAttrib, 3> vs{
      VertexAttrib(0, 3), VertexAttrib(1, 3),
      VertexAttrib(2, 2),
  };
  renderCube(vs);
}
void renderCubeD(std::array<VertexAttrib, 3> vs) {
  //
  float vertices[] = {
      //  position         // normal         // texture
      //  coord
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
      0.0f, // bottom-left
      1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,
      1.0f, // top-right
      1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,
      0.0f, // bottom-right
      1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,
      1.0f, // top-right
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
      0.0f, // bottom-left
      -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
      1.0f, // top-left
      // front face
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, // bottom-left
      1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      0.0f, // bottom-right
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      1.0f, // top-right
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      1.0f, // top-right
      -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, // top-left
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, // bottom-left
      // left face
      -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, // top-right
      -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
      1.0f, // top-left
      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, // bottom-left
      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, // bottom-left
      -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, // bottom-right
      -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, // top-right
            // right face
      1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, // top-left
      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, // bottom-right
      1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      1.0f, // top-right
      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, // bottom-right
      1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, // top-left
      1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, // bottom-left
      // bottom face
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      1.0f, // top-right
      1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
      1.0f, // top-left
      1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
      0.0f, // bottom-left
      1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
      0.0f, // bottom-left
      -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      0.0f, // bottom-right
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      1.0f, // top-right
      // top face
      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      1.0f, // top-left
      1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      0.0f, // bottom-right
      1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, // top-right
      1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      0.0f, // bottom-right
      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      1.0f, // top-left
      -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.0f // bottom-left
  };
  GLuint cubeVAO;
  GLuint cubeVBO;

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  //
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices,
               GL_STATIC_DRAW);
  glBindVertexArray(cubeVAO);
  // ----------------- shader specific -----------------
  setVertexAttrib(vs);
  //
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  // render
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
}

void renderCubeD_S() {
  //
  std::array<VertexAttrib, 3> vs{VertexAttrib(0, 3),
                                 VertexAttrib(1, 3),
                                 VertexAttrib(2, 2)};
  renderCubeD(vs);
}
void renderQuad(std::array<VertexAttrib, 2> vs) {
  GLuint quadVAO;
  GLuint quadVBO;

  float screen[] = {//
                    // positions    texCoords //
                    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //

                    1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
                    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f}; //
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen), &screen,
               GL_STATIC_DRAW);
  // ------------- shader specific -------------
  setVertexAttrib(vs);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
}
void renderQuadS() {
  std::array<VertexAttrib, 2> vs{
      VertexAttrib(0, 3), // aPos
      VertexAttrib(1, 2)  // aTexCoord
  };
  renderQuad(vs);
}
void renderPlane(std::array<VertexAttrib, 3> vs) {
  float planeVertices[] = {
      // positions            // normals         //
      // texcoords
      25.0f,  -0.5f, 25.0f,  0.0f,
      1.0f,   0.0f,  25.0f,  0.0f, //
      -25.0f, -0.5f, 25.0f,  0.0f,
      1.0f,   0.0f,  0.0f,   0.0f, //
      -25.0f, -0.5f, -25.0f, 0.0f,
      1.0,    0.0f,  0.0f,   25.0f, //
      25.0f,  -0.5f, 25.0f,  0.0f,
      1.0f,   0.0f,  25.0f,  0.0f, //
      -25.0f, -0.5f, -25.0f, 0.0f,
      1.0f,   0.0f,  0.0f,   25.0f, //
      25.0f,  -0.5f, -25.0f, 0.0f,
      1.0f,   0.0f,  25.0f,  25.0f //
  };
  GLuint planeVAO;
  GLuint planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices),
               &planeVertices[0], GL_STATIC_DRAW);
  setVertexAttrib(vs);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
void renderPlaneS() {
  std::array<VertexAttrib, 3> vs{VertexAttrib(0, 3),
                                 VertexAttrib(1, 3),
                                 VertexAttrib(2, 2)};
  renderPlane(vs);
}
};

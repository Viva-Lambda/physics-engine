// shader program
#pragma once

#include <external.hpp>

void checkShaderCompilation(GLuint shader,
                            const char *shaderType,
                            const char *shaderFilePath) {
  // check the shader compilation
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderType
              << "::" << shaderFilePath
              << "::COMPILATION_FAILED" << std::endl
              << infoLog << std::endl;
  }
}

void checkShaderProgramCompilation(GLuint program,
                                   const char *shaderName) {
  // check the shader compilation
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << shaderName
              << "::PROGRAM"
              << "::LINK_FAILED\n"
              << infoLog << std::endl;
  }
}

void checkUniformLocation(int locVal, std::string uniName) {
  if (locVal == -1) {
    std::cout << "Shader program can not find the uniform "
                 "location for "
              << uniName << std::endl;
  }
}

class Shader {
public:
  // program id
  GLuint programId;
  std::string shaderName = "MyShader";

  // constructor takes the path of the shaders and builts
  // them
  Shader() {}
  Shader(const GLchar *vertexPath,
         const GLchar *fragmentPath);
  Shader(const GLchar *computePath);
  Shader(const GLchar *vertexPath,
         const GLchar *fragmentPath,
         const GLchar *computePath);
  Shader(std::string *shaderPaths, std::string *shaderTypes,
         unsigned int nb_paths);
  Shader(const GLchar *vertexSource,
         const GLchar *fragmentSource, bool fromSource);

  void useProgram();

  // utility functions for setting uniforms
  void setBoolUni(const std::string &name,
                  bool value) const {
    // set boolean value to given uniform name
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform1i(uniLocation, (int)value);
  };
  void setIntUni(const std::string &name, int value) const {
    // set boolean value to given uniform name
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform1i(uniLocation, value);
  };
  void setFloatUni(const std::string &name,
                   float value) const {
    // set boolean value to given uniform name
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform1f(uniLocation, value);
  };
  void setVec2Uni(const std::string &name,
                  const glm::vec2 &value) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform2fv(uniLocation, 1, glm::value_ptr(value));
  }
  void setVec2Uni(const std::string &name, float x,
                  float y) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform2f(uniLocation, x, y);
  }
  void setVec3Uni(const std::string &name,
                  const glm::vec3 &value) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform3fv(uniLocation, 1, glm::value_ptr(value));
  }
  void setVec3Uni(const std::string &name, float x, float y,
                  float z) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform3f(uniLocation, x, y, z);
  }
  void setVec4Uni(const std::string &name,
                  const glm::vec4 &value) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform4fv(uniLocation, 1, glm::value_ptr(value));
  }
  void setVec4Uni(const std::string &name, float x, float y,
                  float z, float w) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniform4f(uniLocation, x, y, z, w);
  }
  void setMat2Uni(const std::string &name,
                  glm::mat2 &value) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniformMatrix2fv(uniLocation, 1, GL_FALSE,
                       glm::value_ptr(value));
  }
  void setMat3Uni(const std::string &name,
                  glm::mat3 &value) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniformMatrix3fv(uniLocation, 1, GL_FALSE,
                       glm::value_ptr(value));
  }
  void setMat4Uni(const std::string &name,
                  glm::mat4 &value) const {
    int uniLocation =
        glGetUniformLocation(this->programId, name.c_str());
    checkUniformLocation(uniLocation, name);
    glUniformMatrix4fv(uniLocation, 1, GL_FALSE,
                       glm::value_ptr(value));
  }
  // load shader from file path
  GLuint loadShader(const GLchar *shaderFpath,
                    const char *shdrType);
};

GLuint Shader::loadShader(const GLchar *shaderFilePath,
                          const GLchar *shaderType) {
  // load shader file from system
  GLuint shader;
  std::string stype(shaderType);
  if (stype == "FRAGMENT") {
    shader = glCreateShader(GL_FRAGMENT_SHADER);
  } else if (stype == "VERTEX") {
    shader = glCreateShader(GL_VERTEX_SHADER);
  } else if (stype == "COMPUTE") {
    shader = glCreateShader(GL_COMPUTE_SHADER);
  } else if (stype == "GEOMETRY") {
    shader = glCreateShader(GL_GEOMETRY_SHADER);
  } else {
    std::cout << "Unknown shader type:\n"
              << shaderType << std::endl;
  }
  std::ifstream shdrFileStream;
  shdrFileStream.exceptions(std::ifstream::failbit |
                            std::ifstream::badbit);
  std::string shaderCodeStr;
  try {
    shdrFileStream.open(shaderFilePath);
    std::stringstream shaderSStream;
    shaderSStream << shdrFileStream.rdbuf();
    shdrFileStream.close();
    shaderCodeStr = shaderSStream.str();
  } catch (std::ifstream::failure e) {
    //
    std::cout << "shader file path: " << shaderFilePath
              << std::endl;
    std::cout << "shader type: " << shaderType << std::endl;
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ"
              << std::endl;
  }
  const char *shaderCode = shaderCodeStr.c_str();

  // lets source the shader
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);

  // a sanity check for unsuccessful compilations
  checkShaderCompilation(shader, shaderType,
                         shaderFilePath);
  return shader;
}

// first constructor
Shader::Shader(const GLchar *vertexPath,
               const GLchar *fragmentPath) {
  // loading shaders
  programId = glCreateProgram();
  GLuint vshader = loadShader(vertexPath, "VERTEX");
  GLuint fshader = loadShader(fragmentPath, "FRAGMENT");
  glAttachShader(programId, vshader);
  glAttachShader(programId, fshader);
  glLinkProgram(programId);
  checkShaderProgramCompilation(programId,
                                shaderName.c_str());
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}
// third constructor
Shader::Shader(const GLchar *computePath) {
  // loading shaders
  programId = glCreateProgram();
  GLuint cshader = loadShader(computePath, "COMPUTE");
  glAttachShader(programId, cshader);
  glLinkProgram(programId);
  checkShaderProgramCompilation(programId,
                                shaderName.c_str());
  glDeleteShader(cshader);
}
// second constructor
Shader::Shader(const GLchar *vertexPath,
               const GLchar *fragmentPath,
               const GLchar *computePath) {
  // loading shaders
  this->programId = glCreateProgram();
  GLuint vshader = this->loadShader(vertexPath, "VERTEX");
  GLuint fshader =
      this->loadShader(fragmentPath, "FRAGMENT");
  GLuint cshader = this->loadShader(computePath, "COMPUTE");
  glAttachShader(this->programId, vshader);
  glAttachShader(this->programId, fshader);
  glAttachShader(this->programId, cshader);
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId,
                                shaderName.c_str());
  glDeleteShader(vshader);
  glDeleteShader(fshader);
  glDeleteShader(cshader);
}

Shader::Shader(const GLchar *vertexSource,
               const GLchar *fragmentSource,
               bool fromSource) {
  if (!fromSource) {
    std::invalid_argument(
        "if passed source to args, check must be true");
  }
  this->programId = glCreateProgram();
  GLuint vshader;
  glShaderSource(vshader, 1, &vertexSource, NULL);
  glCompileShader(vshader);

  // a sanity check for unsuccessful compilations
  checkShaderCompilation(vshader, "VERTEX", "from source");
  glAttachShader(this->programId, vshader);
  GLuint fshader;
  glShaderSource(fshader, 1, &fragmentSource, NULL);
  glCompileShader(fshader);

  // a sanity check for unsuccessful compilations
  checkShaderCompilation(fshader, "FRAGMENT",
                         "from source");
  glAttachShader(this->programId, fshader);
  glLinkProgram(this->programId);
  checkShaderProgramCompilation(this->programId,
                                shaderName.c_str());
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}

// fourth more general constructor
Shader::Shader(std::string *shaderPaths,
               std::string *shaderTypes,
               unsigned int nb_paths) {
  programId = glCreateProgram();
  GLuint *shaderIds = new GLuint[nb_paths];
  for (unsigned int i = 0; i < nb_paths; i++) {
    //
    GLuint shdrId = loadShader(shaderPaths[i].c_str(),
                               shaderTypes[i].c_str());
    glAttachShader(programId, shdrId);
    shaderIds[i] = shdrId;
  }
  glLinkProgram(programId);
  checkShaderProgramCompilation(programId,
                                shaderName.c_str());
  for (unsigned int i = 0; i < nb_paths; i++) {
    //
    glDeleteShader(shaderIds[i]);
  }
  delete[] shaderIds;
}

void Shader::useProgram() { glUseProgram(programId); }

Shader mk_const_color_mesh_shader() {
  const GLchar *vertex_path =
      "media/demos/glsl/meshcolor.vert";
  const GLchar *fragment_source =
      "media/demos/glsl/meshcolor.frag";
  return Shader(vertex_path, fragment_source);
}
Shader mk_const_color_mesh_shader(const glm::vec3 &color) {
  Shader shdr = mk_const_color_mesh_shader();
  shdr.useProgram();
  shdr.setVec3Uni("diffColor", color);
  return shdr;
}
Shader mk_pointlight_lamp_shader() {
  const GLchar *vertex_path =
      "media/demos/glsl/basic_point_light.vert";
  const GLchar *fragment_source =
      "media/demos/glsl/basic_color_light.frag";
  return Shader(vertex_path, fragment_source);
}
Shader mk_depth_shader() {
  const GLchar *vertex_path =
      "media/demos/glsl/shadow_depth.vert";
  const GLchar *fragment_source =
      "media/demos/glsl/shadow_depth.frag";
  return Shader(vertex_path, fragment_source);
}
Shader mk_line_shader() {
  const GLchar *vertex_path =
      "media/demos/glsl/basic_point_light.vert";
  const GLchar *fragment_source =
      "media/demos/glsl/basic_color_light.frag";
  return Shader(vertex_path, fragment_source);
}

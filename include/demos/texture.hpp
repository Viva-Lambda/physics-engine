#pragma once
// texture related
#include <external.hpp>
namespace vivademos {
enum class TextureType {
  Specular,
  Normal,
  Diffuse,
  Height,
  Displacement,
  Roughness,
  Metallic,
  Ao,
  Albedo
};

inline std::ostream &operator<<(std::ostream &out,
                                const TextureType &t) {
  switch (t) {
  case TextureType::Diffuse:
    return out << "type diffuse";
  case TextureType::Specular:
    return out << "type specular";
  case TextureType::Normal:
    return out << "type normal";
  case TextureType::Height:
    return out << "type height";
  case TextureType::Displacement:
    return out << "type displacement";
  case TextureType::Ao:
    return out << "type ambient occlusion";
  case TextureType::Roughness:
    return out << "type roughness";
  case TextureType::Albedo:
    return out << "type albedo";
  case TextureType::Metallic:
    return out << "type metallic";
  }
}

struct Texture {
  GLuint id;
  TextureType type;
  std::string name;
  std::string path;
};
};

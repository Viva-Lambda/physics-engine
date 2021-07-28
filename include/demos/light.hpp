#pragma once

#include <demos/transformable.hpp>
#include <external.hpp>

namespace vivademos {
using vec3 = vivaphysics::v3;
using point3 = vec3;
using color = vec3;

// default values for the camera
const vivaphysics::real LYAW = -90.0f;
const vivaphysics::real LPITCH = 0.0f;
const vivaphysics::real LSPEED = 2.5f;

//
class Light {
public:
  color emitColor;

  virtual bool emitted(color &emits) const {
    emits = emitColor;
    return true;
  }
  Light(color lightColor) : emitColor(lightColor) {}
};
class DirectionalLight : public Light {
public:
  // TODO refactor components into a hash table
  Rotatable rot;

  //
  vivaphysics::onb basis;
  vivaphysics::v3 worldUp;
  vivaphysics::real movementSpeed;

  DirectionalLight(color lightColor, vec3 wup, vivaphysics::real yaw = LYAW,
                   vivaphysics::real pitch = LPITCH,
                   vivaphysics::real mspeed = LSPEED)
      : Light(lightColor), worldUp(wup), movementSpeed(mspeed) {
    euler_angles angles;
    angles.set_yaw(yaw);
    angles.set_pitch(pitch);
    angles.set_roll(0.0);
    rot = Rotatable::fromEulerAngles(angles);
    setup_basis();

    updateDirection();
  }
  void setYaw(vivaphysics::real nyaw) {
    rot.set_yaw(nyaw);
    updateDirection();
  }
  void setPitch(vivaphysics::real val) {
    rot.set_pitch(val);
    updateDirection();
  }
  void setup_basis() {
    vivaphysics::euler_angles angles = rot.to_euler();
    vivaphysics::real yaw = angles.yaw();
    vivaphysics::real pitch = angles.pitch();
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    vivaphysics::v3 f(front);
    basis.from_w(f);
  }
  void processKeyBoardRotate(ROTATE_DIRECTION direction,
                             vivaphysics::real deltaTime) {
    //
    rot.rotate_by_euler(direction, deltaTime, movementSpeed);
    updateDirection();
  }

protected:
  void updateDirection() {
    //
    vivaphysics::euler_angles angles = rot.to_euler();
    basis.from_euler(angles, worldUp);
  }
};
class PointLight : public Light {
public:
  Translatable trans;
  point3 position;
  vivaphysics::real movementSpeed;

  PointLight() : Light(vivaphysics::v3(1.0)) {}
  PointLight(const color &lightColor, const point3 &pos,
             vivaphysics::real mspeed = LSPEED)
      : Light(lightColor), trans(pos), movementSpeed(mspeed) {}

  void processKeyboard(MOVE_DIRECTION direction, vivaphysics::real deltaTime) {
    float velocity = movementSpeed * deltaTime;
    trans.move(direction, velocity);
  }
};
class SpotLight : public DirectionalLight, public PointLight {
public:
  vivaphysics::real cutOff;
  vivaphysics::real outerCutoff;
  vivaphysics::real movementSpeed;
  Transformable transform;
  //
  SpotLight(color lightColor, point3 pos, vec3 wup, vivaphysics::real y = LYAW,
            vivaphysics::real p = LPITCH,
            vivaphysics::real cutOffAngleDegree = 0.91,
            vivaphysics::real outerCut = 0.82,
            vivaphysics::real mspeed = LSPEED)

      : DirectionalLight(lightColor, wup, y, p), PointLight(lightColor, pos),
        cutOff(glm::radians(cutOffAngleDegree)), outerCutoff(outerCut),
        movementSpeed(mspeed) {
    transform.set_rotatable(rot);
    transform.set_translatable(trans);
  }
  glm::mat4 get_view_matrix() const { return transform.get_view_matrix(basis); }
  void processKeyboard(MOVE_DIRECTION direction, vivaphysics::real deltaTime) {

    float velocity = movementSpeed * deltaTime;
    transform.trans.move(direction, velocity, basis);
    // updateDirection();
  }
  void processKeyBoardRotate(ROTATE_DIRECTION direction,
                             vivaphysics::real deltaTime) {
    transform.rot.rotate_by_euler(direction, deltaTime, movementSpeed);
    updateDirection();
  }
};
}; // namespace vivademos

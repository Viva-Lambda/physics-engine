#pragma once
// camera object
#include "vivaphysics/eulerangle.hpp"
#include <demos/transformable.hpp>
#include <external.hpp>

using namespace vivademos;

namespace vivademos {

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};
struct Segment {
  glm::vec3 origin;
  glm::vec3 direction;
  float size;
};

// default values for the camera
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.00001f;
const float ZOOM = 45.0f;

// Abstract camera class
class Camera {
public:
  // TODO refactor components into a hash table
  Transformable transform;

  vivaphysics::onb basis;
  vivaphysics::v3 worldUp;
  // euler angles

  // camera options
  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  // Constructor 1
  Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f),
         float Yaw = YAW, float Pitch = PITCH,
         float Zoom = ZOOM,
         glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
         float Speed = SPEED, float Sens = SENSITIVITY);

  // Constructor 2
  Camera(float posX, float posY, float posZ, float upX,
         float upY, float upZ, float Yaw, float Pitch,
         glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
         float Speed = SPEED, float Sens = SENSITIVITY,
         float Zoom = ZOOM);
  void processKeyboardRotate(ROTATE_DIRECTION direction,
                             float deltaTime);
  virtual void processKeyboard(MOVE_DIRECTION direction,
                               float deltaTime);
  glm::mat4 getViewMatrix();
  virtual void
  processMouseMovement(float xoffset, float yoffset,
                       GLboolean pitchBound = true);
  void processMouseScroll(float yoffset);
  void setYaw(vivaphysics::real nyaw) {
    transform.rot.set_yaw(nyaw);
    updateCameraVectors();
  }
  void setPitch(vivaphysics::real npitch) {
    transform.rot.set_pitch(npitch);
    updateCameraVectors();
  }
  void setZoom(float nzoom) { zoom = nzoom; }

private:
  void updateCameraVectors();
};

// first constructor
Camera::Camera(glm::vec3 Position, glm::vec3 Up, float Yaw,
               float Pitch, float Zoom, glm::vec3 Front,
               float Speed, float Sens) {
  // create transformable component
  //
  // create translatable component
  vivaphysics::v3 pos_v(Position.x, Position.y, Position.z);
  // set rotatable
  // set angles
  euler_angles angles;

  angles.set_yaw(Yaw);
  angles.set_pitch(Pitch);
  angles.set_roll(0.0);
  auto rot_ = Rotatable::fromEulerAngles(angles);
  Translatable trans(pos_v);
  // set transformable
  transform = Transformable(trans, rot_);

  worldUp = vivaphysics::v3(Up);
  movementSpeed = Speed;
  mouseSensitivity = Sens;
  basis.from_w(Front.x, Front.y, Front.z);
  zoom = Zoom;
  updateCameraVectors();
}

// second constructor
Camera::Camera(float posX, float posY, float posZ,
               float upX, float upY, float upZ, float Yaw,
               float Pitch, glm::vec3 Front, float Speed,
               float Sens, float Zoom) {
  // create transformable component
  // set position
  vivaphysics::v3 pos_v(posX, posY, posZ);

  // set rotatable
  // set angles
  euler_angles angles;

  angles.set_yaw(Yaw);
  angles.set_pitch(Pitch);
  angles.set_roll(0.0);
  auto rot_ = Rotatable::fromEulerAngles(angles);
  Translatable trans(pos_v);
  transform = Transformable(trans, rot_);

  // set world up
  worldUp = vivaphysics::v3(upX, upY, upZ);
  movementSpeed = Speed;
  mouseSensitivity = Sens;
  zoom = Zoom;
  basis.from_w_up(vivaphysics::v3(Front), worldUp);
  updateCameraVectors();
}
void Camera::updateCameraVectors() {
  vivaphysics::euler_angles angles =
      transform.rot.to_euler();
  basis.from_euler(angles);
}
void Camera::processKeyboard(MOVE_DIRECTION direction,
                             float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  transform.trans.move(direction, velocity, basis);
}

glm::mat4 Camera::getViewMatrix() {
  glm::vec3 target =
      (transform.trans.position + basis.front()).to_glm();
  glm::vec3 upvec = basis.up().to_glm();
  glm::vec3 cameraDirection = glm::normalize(
      transform.trans.position.to_glm() - target);
  glm::vec3 right =
      glm::normalize(glm::cross(upvec, cameraDirection));
  glm::vec3 realUp = glm::normalize(
      glm::cross(cameraDirection, basis.right().to_glm()));
  //
  glm::mat4 trans(1.0f);
  trans[3][0] = -transform.trans.position.x;
  trans[3][1] = -transform.trans.position.y;
  trans[3][2] = -transform.trans.position.z;

  //
  glm::mat4 rotation(1.0f);
  rotation[0][0] = right.x;
  rotation[1][0] = right.y;
  rotation[2][0] = right.z;
  rotation[0][1] = realUp.x;
  rotation[1][1] = realUp.y;
  rotation[2][1] = realUp.z;
  rotation[0][2] = cameraDirection.x;
  rotation[1][2] = cameraDirection.y;
  rotation[2][2] = cameraDirection.z;
  return rotation * trans;
}

void Camera::processMouseMovement(float xoffset,
                                  float yoffset,
                                  GLboolean pitchBound) {
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;
  vivaphysics::euler_angles angles =
      transform.rot.to_euler();
  vivaphysics::real yaw = angles.yaw();
  vivaphysics::real pitch = angles.pitch();

  yaw += xoffset;
  pitch += yoffset;

  if (pitchBound) {
    if (pitch > 89.0f) {
      angles.set_pitch(89.0f);
    }
    if (pitch < -89.0f) {
      angles.set_pitch(-89.0f);
    }
  }

  updateCameraVectors();
}

void Camera::processKeyboardRotate(
    ROTATE_DIRECTION direction, float deltaTime) {
  transform.rot.rotate_by_euler(direction, deltaTime,
                                movementSpeed);
  updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {

  if (zoom >= 1.0f && zoom <= 45.0f) {
    zoom -= yoffset;
  }
  if (zoom <= 1.0f) {
    zoom = 1.0f;
  }
  if (zoom >= 45.0f) {
    zoom = 45.0f;
  }
}
};

#pragma once
// camera object
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
  Ray getRayToPosV4Perspective(glm::vec4 posn);
  Ray getRayToPosV4Ortho(glm::vec4 posn);
  Segment getSegmentToPosV4Perspective(glm::vec4 posn);
  Segment getSegmentToPosV4Ortho(glm::vec4 posn);
  glm::vec3 getPosToPosV4Perspective(glm::vec4 posn);
  glm::vec3 getPosToPosV4Ortho(glm::vec4 posn);
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
  pos = Position;
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

  worldUp = Up;
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
  worldUp = glm::vec3(upX, upY, upZ);
  movementSpeed = Speed;
  mouseSensitivity = Sens;
  zoom = Zoom;
  front = Front;
  updateCameraVectors();
}
void Camera::updateCameraVectors() {
  vivaphysics::EulerAngles angles =
      transform.rot.to_euler();
  real yaw = angles.yaw();
  real pitch = angles.pitch();
  // compute new front
  vivaphysics::v3 front_;
  front_.x = cos(yaw) * cos(pitch);
  front_.y = sin(pitch);
  front_.z = sin(yaw) * cos(pitch);

  //
  basis.from_w(front_x);
}
void Camera::processKeyboard(MOVE_DIRECTION direction,
                             float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  transform.trans.move(direction, velocity, basis);
}

glm::mat4 Camera::getViewMatrix() {
  glm::vec3 target = pos + front;
  glm::vec3 upvec = up;
  glm::vec3 cameraDirection = glm::normalize(pos - target);
  glm::vec3 right =
      glm::normalize(glm::cross(upvec, cameraDirection));
  glm::vec3 realUp =
      glm::normalize(glm::cross(cameraDirection, right));
  //
  glm::mat4 trans(1.0f);
  trans[3][0] = -pos.x;
  trans[3][1] = -pos.y;
  trans[3][2] = -pos.z;

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

  yaw += xoffset;
  pitch += yoffset;

  if (pitchBound) {
    if (pitch > 89.0f) {
      pitch = 89.0f;
    }
    if (pitch < -89.0f) {
      pitch = -89.0f;
    }
  }

  updateCameraVectors();
}

void Camera::processKeyboardRotate(
    Camera_Movement direction, float deltaTime) {

  deltaTime *= movementSpeed;
  switch (direction) {
  case Camera_Movement::FORWARD:
    pitch += deltaTime;
    break;
  case Camera_Movement::BACKWARD:
    pitch -= deltaTime;
    break;
  case Camera_Movement::RIGHT:
    yaw += deltaTime;
    break;
  case Camera_Movement::LEFT:
    yaw -= deltaTime;
    break;
  }
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
glm::vec3 Camera::getPosToPosV4Perspective(glm::vec4 poss) {
  glm::vec3 posPers = glm::vec3(
      poss.x / poss.w, poss.y / poss.w, poss.z / poss.w);
  return posPers - pos;
}
glm::vec3 Camera::getPosToPosV4Ortho(glm::vec4 poss) {
  glm::vec3 posPers = glm::vec3(poss.x, poss.y, poss.z);
  return posPers - pos;
}
Segment
Camera::getSegmentToPosV4Perspective(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  glm::vec3 segment = getPosToPosV4Perspective(pos);
  Segment s;
  s.origin = segment;
  s.size = glm::length(segment);
  s.direction = glm::normalize(segment);
  return s;
}
Segment Camera::getSegmentToPosV4Ortho(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  glm::vec3 segment = getPosToPosV4Ortho(pos);
  Segment s;
  s.origin = segment;
  s.direction = glm::normalize(segment);
  s.size = glm::length(segment);
  return s;
}
Ray Camera::getRayToPosV4Perspective(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  Segment s = this->getSegmentToPosV4Perspective(pos);
  Ray r;
  r.origin = s.origin;
  r.direction = s.direction;
  return r;
}
Ray Camera::getRayToPosV4Ortho(glm::vec4 pos) {
  // do perspective division to starting point
  // then subtract from the start to have an end
  Segment s = getSegmentToPosV4Ortho(pos);
  Ray r;
  r.origin = s.origin;
  r.direction = s.direction;
  return r;
}
};

#pragma once
// camera object
#include <external.hpp>

namespace vivademos {
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

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
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;
  // euler angles
  float yaw;
  float pitch;

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
  void processKeyBoardRotate(Camera_Movement direction,
                             float deltaTime);
  virtual void processKeyboard(Camera_Movement direction,
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
  void setYaw(float nyaw) {
    yaw = nyaw;
    updateCameraVectors();
  }
  void setPitch(float npitch) {
    pitch = npitch;
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
  pos = Position;
  worldUp = Up;
  pitch = Pitch;
  yaw = Yaw;
  movementSpeed = Speed;
  mouseSensitivity = Sens;
  front = Front;
  zoom = Zoom;
  updateCameraVectors();
}

// second constructor
Camera::Camera(float posX, float posY, float posZ,
               float upX, float upY, float upZ, float Yaw,
               float Pitch, glm::vec3 Front, float Speed,
               float Sens, float Zoom) {
  pos = glm::vec3(posX, posY, posZ);
  worldUp = glm::vec3(upX, upY, upZ);
  yaw = Yaw;
  pitch = Pitch;
  movementSpeed = Speed;
  mouseSensitivity = Sens;
  zoom = Zoom;
  front = Front;
  updateCameraVectors();
}
void Camera::updateCameraVectors() {
  glm::vec3 front_;
  // compute new front
  front_.x =
      cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front_.y = sin(glm::radians(pitch));
  front_.z =
      sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(front_);

  // compute right and up
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
void Camera::processKeyboard(Camera_Movement direction,
                             float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  switch (direction) {
  case FORWARD:
    pos += front * velocity;
    break;
  case BACKWARD:
    pos -= front * velocity;
    break;
  case RIGHT:
    pos += right * velocity;
    break;
  case LEFT:
    pos -= right * velocity;
    break;
  }
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
void Camera::processKeyBoardRotate(
    Camera_Movement direction, float deltaTime) {

  deltaTime *= movementSpeed;
  switch (direction) {
  case FORWARD:
    pitch += deltaTime;
    break;
  case BACKWARD:
    pitch -= deltaTime;
    break;
  case RIGHT:
    yaw += deltaTime;
    break;
  case LEFT:
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

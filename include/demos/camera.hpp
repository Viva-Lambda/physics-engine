#pragma once
// camera object
#include <demos/transformable.hpp>
#include <external.hpp>

using namespace vivademos;

namespace vivademos {

// default values for the camera
const vivaphysics::real YAW = -90.0f;
const vivaphysics::real PITCH = 0.0f;
const vivaphysics::real SPEED = 2.5f;
const vivaphysics::real SENSITIVITY = 0.00001f;
const vivaphysics::real ZOOM = 45.0f;

// Abstract camera class
class Camera {
public:
  // TODO refactor components into a hash table
  Transformable transform;

  vivaphysics::onb basis;
  vivaphysics::v3 worldUp;
  // euler angles

  // camera options
  vivaphysics::real movementSpeed;
  vivaphysics::real mouseSensitivity;
  vivaphysics::real zoom;

  vivaphysics::real near;
  vivaphysics::real far;

  // Constructor 1
  Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f),
         vivaphysics::real Yaw = YAW, vivaphysics::real Pitch = PITCH,
         vivaphysics::real Zoom = ZOOM,
         glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
         vivaphysics::real Speed = SPEED, vivaphysics::real Sens = SENSITIVITY);

  // Constructor 2
  Camera(vivaphysics::real posX, vivaphysics::real posY, vivaphysics::real posZ,
         vivaphysics::real upX, vivaphysics::real upY, vivaphysics::real upZ,
         vivaphysics::real Yaw, vivaphysics::real Pitch,
         glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
         vivaphysics::real Speed = SPEED, vivaphysics::real Sens = SENSITIVITY,
         vivaphysics::real Zoom = ZOOM);
  void processKeyboardRotate(ROTATE_DIRECTION direction,
                             vivaphysics::real deltaTime);
  virtual void processKeyboard(MOVE_DIRECTION direction,
                               vivaphysics::real deltaTime);
  glm::mat4 get_view_matrix() { return transform.get_view_matrix(basis); }
  virtual void processMouseMovement(vivaphysics::real xoffset,
                                    vivaphysics::real yoffset,
                                    GLboolean pitchBound = true);
  void processMouseScroll(vivaphysics::real yoffset);
  void setYaw(vivaphysics::real nyaw) {
    transform.rot.set_yaw(nyaw);
    updateCameraVectors();
  }
  void setPitch(vivaphysics::real npitch) {
    transform.rot.set_pitch(npitch);
    updateCameraVectors();
  }
  void setZoom(vivaphysics::real nzoom) { zoom = nzoom; }

  void set_near(vivaphysics::real n) { near = n; }
  void set_far(vivaphysics::real f) { far = f; }
  glm::mat4 perspective(unsigned int width, unsigned int height) const;

private:
  void updateCameraVectors();
};

class SimpleCamera {
public:
  // TODO refactor components into a hash table

  vivaphysics::v3 pos;
  vivaphysics::v3 front;
  vivaphysics::v3 up;
  vivaphysics::v3 right;
  vivaphysics::v3 worldUp;
  // euler angles
  vivaphysics::real yaw;
  vivaphysics::real pitch;

  // camera options
  vivaphysics::real movementSpeed;
  vivaphysics::real mouseSensitivity;
  vivaphysics::real zoom;

  vivaphysics::real near;
  vivaphysics::real far;

  // Constructor 1
  SimpleCamera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f),
               vivaphysics::real Yaw = YAW, vivaphysics::real Pitch = PITCH,
               vivaphysics::real Zoom = ZOOM,
               glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
               vivaphysics::real Speed = SPEED,
               vivaphysics::real Sens = SENSITIVITY)
      : pos(Position), worldUp(Up), yaw(Yaw), pitch(Pitch), zoom(Zoom),
        mouseSensitivity(Sens), movementSpeed(Speed), front(Front) {
    updateCameraVectors();
  }

  // Constructor 2
  SimpleCamera(vivaphysics::real posX, vivaphysics::real posY,
               vivaphysics::real posZ, vivaphysics::real upX,
               vivaphysics::real upY, vivaphysics::real upZ,
               vivaphysics::real Yaw, vivaphysics::real Pitch,
               glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
               vivaphysics::real Speed = SPEED,
               vivaphysics::real Sens = SENSITIVITY,
               vivaphysics::real Zoom = ZOOM)
      : pos(posX, posY, posZ), worldUp(upX, upY, upZ), yaw(Yaw), pitch(Pitch),
        zoom(Zoom), mouseSensitivity(Sens), movementSpeed(Speed), front(Front) {
    updateCameraVectors();
  }
  void processKeyboardRotate(ROTATE_DIRECTION direction,
                             vivaphysics::real deltaTime) {
    // vivaphysics::real velocity = movementSpeed * deltaTime;
    vivaphysics::real velocity = 0.1;
    if (direction == ROTATE_DIRECTION::FORWARD) {
      pitch += velocity;
    } else if (direction == ROTATE_DIRECTION::BACKWARD) {
      pitch -= velocity;
    } else if (direction == ROTATE_DIRECTION::LEFT) {
      yaw -= velocity;
    } else if (direction == ROTATE_DIRECTION::RIGHT) {
      yaw += velocity;
    } else {
      throw std::runtime_error("rotate by euler accepts only forward,backward, "
                               "left, right rotation directions");
    }
    updateCameraVectors();
  }
  void processKeyboard(MOVE_DIRECTION direction, vivaphysics::real deltaTime) {
    //
    vivaphysics::real dtime = movementSpeed * deltaTime;
    auto cpos = pos.to_glm();
    switch (direction) {
    case MOVE_DIRECTION::LEFT: {
      cpos -= right.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::RIGHT: {
      cpos += right.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::DOWN: {
      cpos -= up.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::UP: {
      cpos += up.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::BACKWARD: {
      cpos -= front.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::FORWARD: {
      cpos += front.to_glm() * dtime;
      break;
    }
    }
    pos = vivaphysics::v3(cpos);
  }
  glm::mat4 get_view_matrix() {
    //
    auto target = pos.to_glm() + front.to_glm();
    auto mat = glm::lookAt(pos.to_glm(), target, up.to_glm());
    return mat;
  }
  virtual void processMouseMovement(vivaphysics::real xoffset,
                                    vivaphysics::real yoffset,
                                    GLboolean pitchBound = true) {
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
  void processMouseScroll(vivaphysics::real yoffset) {

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
  void setYaw(vivaphysics::real nyaw) {
    yaw = nyaw;
    updateCameraVectors();
  }
  void setPitch(vivaphysics::real npitch) {
    pitch = npitch;
    updateCameraVectors();
  }
  void setZoom(vivaphysics::real nzoom) { zoom = nzoom; }

  void set_near(vivaphysics::real n) { near = n; }
  void set_far(vivaphysics::real f) { far = f; }
  glm::mat4 perspective(unsigned int width, unsigned int height) const {
    return glm::perspective(glm::radians(zoom),
                            static_cast<vivaphysics::real>(width) /
                                static_cast<vivaphysics::real>(height),
                            near, far);
  }

private:
  // update camera using euler angles
  void updateCameraVectors() {

    // compute new front
    glm::vec3 fr;
    fr.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    fr.y = std::sin(pitch);
    fr.z = std::sin(yaw) * std::cos(pitch);
    auto f = glm::normalize(fr);
    front = vivaphysics::v3(f);
    // recompute up and right
    auto r = glm::normalize(glm::cross(f, worldUp.to_glm()));
    right = vivaphysics::v3(r);
    auto u = glm::normalize(glm::cross(r, f));
    up = vivaphysics::v3(u);
  }
};

// first constructor
Camera::Camera(glm::vec3 Position, glm::vec3 Up, vivaphysics::real Yaw,
               vivaphysics::real Pitch, vivaphysics::real Zoom, glm::vec3 Front,
               vivaphysics::real Speed, vivaphysics::real Sens) {
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
Camera::Camera(vivaphysics::real posX, vivaphysics::real posY,
               vivaphysics::real posZ, vivaphysics::real upX,
               vivaphysics::real upY, vivaphysics::real upZ,
               vivaphysics::real Yaw, vivaphysics::real Pitch, glm::vec3 Front,
               vivaphysics::real Speed, vivaphysics::real Sens,
               vivaphysics::real Zoom) {
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
  vivaphysics::euler_angles eangles = transform.rot.to_euler();
  basis.from_euler(eangles);
}
void Camera::processKeyboard(MOVE_DIRECTION direction,
                             vivaphysics::real deltaTime) {
  vivaphysics::real velocity = movementSpeed * deltaTime;
  transform.trans.move(direction, velocity, basis);
}

glm::mat4 Camera::perspective(unsigned int width, unsigned int height) const {
  return glm::perspective(glm::radians(zoom),
                          static_cast<vivaphysics::real>(width) /
                              static_cast<vivaphysics::real>(height),
                          near, far);
}

void Camera::processMouseMovement(vivaphysics::real xoffset,
                                  vivaphysics::real yoffset,
                                  GLboolean pitchBound) {
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;
  vivaphysics::euler_angles angles = transform.rot.to_euler();
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

void Camera::processKeyboardRotate(ROTATE_DIRECTION direction,
                                   vivaphysics::real deltaTime) {
  transform.rot.rotate_by_euler(direction, deltaTime, movementSpeed);
  updateCameraVectors();
}
void Camera::processMouseScroll(vivaphysics::real yoffset) {

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
}; // namespace vivademos

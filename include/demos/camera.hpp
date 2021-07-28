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

  vivaphysics::v3 m_pos;
  vivaphysics::v3 m_dir;
  vivaphysics::v3 worldUp;

  // matrices
  glm::mat4 m_view;
  glm::mat4 m_proj;

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
      : m_pos(Position), worldUp(Up), zoom(Zoom), mouseSensitivity(Sens),
        movementSpeed(Speed), m_dir(Front) {
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
      : m_pos(posX, posY, posZ), worldUp(upX, upY, upZ), zoom(Zoom),
        mouseSensitivity(Sens), movementSpeed(Speed), m_dir(Front) {
    updateCameraVectors();
  }
  void processKeyboardRotate(ROTATE_DIRECTION direction,
                             vivaphysics::real deltaTime) {
    vivaphysics::real velocity = movementSpeed * deltaTime;

    vivaphysics::v3 xm(1, 0, 0);
    vivaphysics::v3 ym(0, 1, 0);
    vivaphysics::v3 zm(0, 0, 1);
    real angle_dir = velocity;

    auto cdir = glm::vec3(1);
    // vivaphysics::real velocity = 0.1;
    switch (direction) {
    case ROTATE_DIRECTION::FORWARD: {
      cdir = zm.to_glm();
      break;
    }
    case ROTATE_DIRECTION::BACKWARD: {
      cdir = zm.to_glm();
      angle_dir *= -1;
      break;
    }
    case ROTATE_DIRECTION::LEFT: {
      cdir = ym.to_glm();
      angle_dir *= -1;
      break;
    }
    case ROTATE_DIRECTION::RIGHT: {
      cdir = ym.to_glm();
      break;
    }
    case ROTATE_DIRECTION::ROTATE_X: {
      cdir = xm.to_glm();
      break;
    }
    case ROTATE_DIRECTION::ROTATE_Y: {
      cdir = ym.to_glm();
      break;
    }
    case ROTATE_DIRECTION::ROTATE_Z: {
      cdir = zm.to_glm();
      break;
    }
    case ROTATE_DIRECTION::U_AXIS: {
      cdir = zm.to_glm();
      break;
    }
    }
    auto axis = vivaphysics::v3(cdir);
    rotate(angle_dir, axis);

    updateCameraVectors();
  }
  void processKeyboard(MOVE_DIRECTION direction, vivaphysics::real deltaTime) {
    //
    vivaphysics::real dtime = movementSpeed * deltaTime;
    vivaphysics::v3 xm(1, 0, 0);
    vivaphysics::v3 ym(0, 1, 0);
    vivaphysics::v3 zm(0, 0, 1);
    auto cdir = glm::vec3(1);
    switch (direction) {
    case MOVE_DIRECTION::LEFT: {
      cdir = xm.to_glm() * dtime * -1.0f;
      break;
    }
    case MOVE_DIRECTION::RIGHT: {
      cdir = xm.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::DOWN: {
      cdir = ym.to_glm() * dtime * -1.0f;
      break;
    }
    case MOVE_DIRECTION::UP: {
      cdir = ym.to_glm() * dtime;
      break;
    }
    case MOVE_DIRECTION::FORWARD: {
      cdir = zm.to_glm() * dtime * -1.0f;
      break;
    }
    case MOVE_DIRECTION::BACKWARD: {
      cdir = zm.to_glm() * dtime;
      break;
    }
    }
    auto dir = vivaphysics::v3(cdir);
    move(dir);
  }
  glm::mat4 get_view_matrix() {
    //
    auto target = m_pos.to_glm() + m_dir.to_glm();
    auto mat = glm::lookAt(m_pos.to_glm(), target, worldUp.to_glm());
    return mat;
  }
  void set_view_matrix() { m_view = get_view_matrix(); }
  virtual void processMouseMovement(vivaphysics::real xoffset,
                                    vivaphysics::real yoffset,
                                    GLboolean pitchBound = true) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    real yaw = 0;
    real pitch = 0;

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

  void rotate(vivaphysics::real angle, const vivaphysics::v3 &axis) {
    auto dir =
        glm::rotate(m_dir.to_glm(), glm::radians(angle * movementSpeed), axis.to_glm());
    m_dir = vivaphysics::v3(dir);
  }
  void move(const vivaphysics::v3 &dir) {
    auto pos = m_pos.to_glm();
    pos += dir.to_glm();
    m_pos = vivaphysics::v3(pos);
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
  void set_proj(unsigned int w, unsigned int h) { m_proj = perspective(w, h); }

private:
  // update camera using euler angles
  void updateCameraVectors() {

    // compute new front
  }
}; // namespace vivademos

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

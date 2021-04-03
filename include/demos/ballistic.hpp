// ballistic demo app
#pragma once
#include <demos/mesh.hpp>
#include <demos/meshapp.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>

using namespace vivademos;

namespace vivademos {

class BallisticMeshDemo : public MeshDemoApp {
private:
  // const unsigned int max_ammo = 16;

  // AmmoRound ammo[16];
  AmmoRound ammo;
  /** holds the current shot type*/
  ShotType current_stype;

  void fire() {
    if (ammo.stype == UNUSED) {
      return;
    }

    // choose shot type
    switch (current_stype) {
    case UNUSED:
      return;
      break;
    case PISTOL:
      ammo.particle.set_mass(2.0f);
      ammo.particle.set_velocity(0, 0, 35);
      ammo.particle.set_acceleration(0, -1.0f, 0);
      ammo.particle.set_damping(0.99f);
      break;
    case ARTILLERY:
      ammo.particle.set_mass(200.0f);
      ammo.particle.set_velocity(0, 30, 40);
      ammo.particle.set_acceleration(0, -20.0f, 0);
      ammo.particle.set_damping(0.99f);
      break;
    case FIREBALL:
      ammo.particle.set_mass(1.0f);
      ammo.particle.set_velocity(0, 0, 10);
      ammo.particle.set_acceleration(0, 0.6f, 0);
      ammo.particle.set_damping(0.9f);
      break;
    case LASER:
      ammo.particle.set_mass(0.1f);
      ammo.particle.set_velocity(0, 0, 100.f);
      ammo.particle.set_acceleration(0, 0.0f,
                                     0); // no gravity
      ammo.particle.set_damping(0.99f);
      break;
    }
    ammo.particle.set_position(1.0f, 1.0f, 1.0f);
    ammo.stype = current_stype;
    // set timing
    ammo.start_time = last_time;

    // clear accumulated force
    ammo.particle.clear_accumulator();
  }

public:
  BallisticMeshDemo()
      : MeshDemoApp(), current_stype(LASER) {}
  BallisticMeshDemo(int w, int h, std::string title)
      : MeshDemoApp(w, h, title), current_stype(LASER) {}
  std::string get_title() override {
    return "Ballistic Demo Application";
  }
  void fixed_update() {
    float duration =
        static_cast<float>(MeshDemoApp::dtime());
    if (duration <= 0.0f)
      return;

    // update physics tick for each particle
    auto shot = ammo;
    if (shot.stype != UNUSED) {
      // run the physics
      shot.particle.integrate(duration);

      // check if the shot is still visible
      // not on screen
      bool cond1 = shot.particle.get_position().y < 0.0f;

      // takes too long
      bool cond2 =
          shot.start_time + 5000 < MeshDemoApp::last_time;

      // beyond visible range
      bool cond3 = shot.particle.get_position().z >
                   MeshDemoApp::far_plane_dist;
      if (cond1 || cond2 || cond3) {
        shot.stype = UNUSED;
      }
    }
  }

  void set_scene_objects() {
    ammo = mk_cube_ammo();
    lamp = SimpleShape(1, false, ShapeChoice::LAMP);
    plane = SimpleShape(1, false, ShapeChoice::PLANE);
  }
  void draw_to_depth_fbo() override {

  }
  void draw_objects() override {
    fixed_update();
    auto mat = ammo.get_model_mat();
    ammo.draw();
  }

  void process_other_keys() override {
    auto cstype = static_cast<int>(current_stype);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
      cstype += 1;
    }
    if (cstype > 4) {
      current_stype = UNUSED;
    } else {
      current_stype = static_cast<ShotType>(cstype);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
      fire();
    }
  }
};
};

// ballistic demo app
#pragma once
#include <demos/mesh.hpp>
#include <demos/meshapp.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>
#include <vivaphysics/pcontact.hpp>
#include <vivaphysics/pfgen.hpp>
#include <vivaphysics/plink.hpp>

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
    if (current_stype == ShotType::UNUSED) {
      return;
    }

    // choose shot type
    switch (current_stype) {
    case ShotType::UNUSED:
      ammo.stype = current_stype;
      return;
      break;
    case ShotType::PISTOL:
      ammo.particle.set_mass(2.0f);
      ammo.particle.set_velocity(0, 0, 35);
      ammo.particle.set_acceleration(0, -1.0f, 0);
      ammo.particle.set_damping(0.99f);
      break;
    case ShotType::ARTILLERY:
      ammo.particle.set_mass(200.0f);
      ammo.particle.set_velocity(0, 30, 40);
      ammo.particle.set_acceleration(0, -20.0f, 0);
      ammo.particle.set_damping(0.99f);
      break;
    case ShotType::FIREBALL:
      ammo.particle.set_mass(1.0f);
      ammo.particle.set_velocity(0, 0, 10);
      ammo.particle.set_acceleration(0, 0.6f, 0);
      ammo.particle.set_damping(0.9f);
      break;
    case ShotType::LASER:
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
      : MeshDemoApp(), current_stype(ShotType::LASER) {}
  BallisticMeshDemo(int w, int h, std::string title)
      : MeshDemoApp(w, h, title),
        current_stype(ShotType::LASER) {}
  std::string get_title() override {
    return "Ballistic Demo Application";
  }
  void fixed_update() {
    float duration = static_cast<float>(dtime());
    if (duration <= 0.0f)
      return;

    // update physics tick for each particle
    if (ammo.stype != ShotType::UNUSED) {
      // run the physics
      ammo.particle.integrate(duration);

      // check if the shot is still visible
      // not on screen
      bool cond1 = ammo.particle.get_position().y < 0.0f;

      // takes too long
      bool cond2 = ammo.start_time + 5000 < last_time;

      // beyond visible range
      bool cond3 =
          ammo.particle.get_position().z > far_plane_dist;
      if (cond1 || cond2 || cond3) {
        ammo.stype = ShotType::UNUSED;
      }
    }
  }

  void set_scene_objects() override {
    ammo = mk_cube_ammo();
    lamp = SimpleShape(1, false, ShapeChoice::LAMP);
    plane = SimpleShape(1, false, ShapeChoice::PLANE);
  }
  void render_scene_depth() override {
    depth_shader.useProgram();
    depth_shader.setMat4Uni("lightSpaceMatrix",
                            lightSpaceMatrix);
    glm::mat4 identityModel = glm::mat4(1.0f);

    // draw scene from light's perspective
    modelMat = ammo.get_model_mat();
    depth_shader.setMat4Uni("model", modelMat);
    ammo.draw();

    depth_shader.setMat4Uni("model", identityModel);
    plane.draw();

    // depth_shader.setMat4Uni("model", lampMat);
    // lamp.draw();
  }
  void render_scene_objects() override {
    // draw objects that need to be drawn
    obj_shader.setVec3Uni("diffColor",
                          glm::vec3(0.2, 0.7, 0.2));

    modelMat = ammo.get_model_mat();
    obj_shader.setMat4Uni("model", modelMat);
    ammo.draw();
  }

  void update() override {
    reset_frame();
    fixed_update();
    set_view();
    draw_objects();
    update_glfw();
  }

  void process_other_keys() override {
    auto cstype = static_cast<int>(current_stype);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
      cstype += 1;
    }
    if (cstype > 4) {
      current_stype = ShotType::UNUSED;
    } else {
      current_stype = static_cast<ShotType>(cstype);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
      fire();
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
      std::cout << current_stype << std::endl;
    }
  }
  void print_other_keys() override {
    std::cout << "Ballistic Control Keys" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "T: switch ammo type" << std::endl;
    std::cout << "Y: fire" << std::endl;
    std::cout << "G: show curent ammo type" << std::endl;
    std::cout << "-------------------------" << std::endl;
  }
};
};

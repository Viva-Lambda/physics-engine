// platform demo
#pragma once
#include <external.hpp>

#include <demos/apputils.hpp>
#include <demos/massapp.hpp>
#include <vivaphysics/pcontact.hpp>
#include <vivaphysics/pfgen.hpp>
#include <vivaphysics/plink.hpp>

using namespace vivademos;

namespace vivademos {

const unsigned int BASE_MASS = 1;
const unsigned int EXTRA_MASS = 10;
const unsigned int ROD_COUNT = 15;

class PlatformDemo : public MassAggregateApp {
public:
  std::vector<vivaphysics::ParticleRod> rods;
  vivaphysics::v3 mass_position;
  vivaphysics::v3 mass_display_position;

  std::string window_title = "Platform Demo";
  PlatformDemo()
      : MassAggregateApp(6), mass_position(0, 0, 0.5f) {

    // set particle positions
    particles[0]->set_position(0, 0, 1);
    particles[1]->set_position(0, 0, -1);
    particles[2]->set_position(-3, 2, 1);
    particles[3]->set_position(-3, 2, -1);
    particles[4]->set_position(4, 2, 1);
    particles[5]->set_position(4, 2, -1);

    // set particle values
    for (auto &p : particles) {
      p->set_mass(BASE_MASS);
      p->set_velocity(0, 0, 0);
      p->set_damping(0.9);
      p->set_acceleration(v3(vivaphysics::v3::GRAVITY));
      p->clear_accumulator();
    }
    // set particle rods
    auto rod_maker = [this](unsigned int pindex1,
                            unsigned int pindex2,
                            vivaphysics::real length = 2) {
      ContactParticles cp;

      cp.ps.push_back(particles[pindex1]);
      cp.ps.push_back(particles[pindex2]);
      cp.is_double = true;
      ParticleRod rod = ParticleRod();
      rod.length = length;
      rod.contact_ps = cp;
      return rod;
    };

    // first rod
    rods.push_back(rod_maker(0, 1));
    rods.push_back(rod_maker(2, 3));
    rods.push_back(rod_maker(4, 5));
    rods.push_back(rod_maker(2, 4, 7));
    rods.push_back(rod_maker(3, 5, 7));
    rods.push_back(rod_maker(0, 2, 3.606));
    rods.push_back(rod_maker(1, 3, 3.606));
    rods.push_back(rod_maker(0, 4, 4.472));
    rods.push_back(rod_maker(1, 5, 4.427));
    rods.push_back(rod_maker(0, 3, 4.123));
    rods.push_back(rod_maker(2, 5, 7.28));
    rods.push_back(rod_maker(4, 1, 4.899));
    rods.push_back(rod_maker(1, 2, 4.123));
    rods.push_back(rod_maker(3, 4, 7.28));
    rods.push_back(rod_maker(5, 0, 4.89));

    for (auto &rod : rods) {
      auto wrapper = ParticleContactWrapper(rod);
      auto pgen = ParticleContactGenerator<
          ParticleContactWrapper>();
      world.add_contact_generator(pgen, wrapper);
    }
    update_particle_platform_mass();
  }

  /**update the masses of particles using additional mass on
   * the platform*/
  void update_particle_platform_mass() {
    for (unsigned int i = 2; i < 6; i++) {
      particles[i]->set_mass(BASE_MASS);
    }
    vivaphysics::real xp = mass_position.x;
    vivaphysics::real zp = mass_position.z;
    xp = clamp<real>(xp, 0, 1);
    zp = clamp<real>(zp, 0, 1);

    //
    mass_display_position.clear();

    particles[2]->set_mass(
        BASE_MASS + EXTRA_MASS * (1 - xp) * (1 - zp));
    mass_display_position.add_scaled_vector(
        particles[2]->get_position(), (1 - xp) * (1 - zp));
    //
    if (xp > 0) {
      particles[4]->set_mass(BASE_MASS +
                             EXTRA_MASS * xp * (1 - zp));
      mass_display_position.add_scaled_vector(
          particles[4]->get_position(), xp * (1 - zp));

      if (zp > 0) {
        particles[5]->set_mass(BASE_MASS +
                               EXTRA_MASS * xp * zp);
        mass_display_position.add_scaled_vector(
            particles[5]->get_position(), xp * zp);
      }
    }
    if (zp > 0) {
      particles[3]->set_mass(BASE_MASS +
                             EXTRA_MASS * zp * (1 - xp));
      mass_display_position.add_scaled_vector(
          particles[3]->get_position(), (1 - xp) * zp);
    }
  }

  void set_scene_objects() override {}

  int init_graphics() override {
    MassAggregateApp::init_graphics();
    set_related();
    // init shaders
    return init_shaders();
  }

  void set_view() override {
    // setting model, view, projection
    projection =
        glm::perspective(glm::radians(camera.zoom),
                         (float)width / (float)height,
                         near_plane_dist, far_plane_dist);
    viewMat = camera.getViewMatrix();

    glm::mat4 lightProj, lightSpaceMat, lightView;
    lightView = glm::lookAt(light.position, glm::vec3(0.0),
                            glm::vec3(0.0, 1.0, 0.0));
    lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
                           1.0f, 7.5f);
    lightSpaceMatrix = lightProj * lightView;
  }
  void render_scene_depth() override {
    depth_shader.useProgram();
    depth_shader.setMat4Uni("lightSpaceMatrix",
                            lightSpaceMatrix);
    glm::mat4 identityModel = glm::mat4(1.0f);

    // draw scene from light's perspective
    depth_shader.setMat4Uni("model", modelMat);
    cube.draw();

    depth_shader.setMat4Uni("model", identityModel);
    plane.draw();

    // depth_shader.setMat4Uni("model", lampMat);
    // lamp.draw();
  }
  void draw_to_depth_fbo() override {
    // render to depth fbo
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    render_scene_depth();

    // lightModel = glm::translate(lightModel,
    // light.position);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void render_scene_objects() override {
    // draw objects that need to be drawn
    obj_shader.setVec3Uni("diffColor",
                          glm::vec3(0.2, 0.7, 0.2));

    obj_shader.setMat4Uni("model", modelMat);
    cube.draw();
  }

  void process_other_keys() override {}
};
};

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
  // shaders
  Shader line_shader;
  SimpleShape lines;
  vivaphysics::v3 line_color;

  std::vector<vivaphysics::ParticleRod> rods;
  vivaphysics::v3 mass_position;
  vivaphysics::v3 mass_display_position;

  std::string window_title = "Platform Demo";
  PlatformDemo()
      : MassAggregateApp(6), mass_position(0, 0, 0.5f) {
    init();
  }
  PlatformDemo(int w, int h, std::string t)
      : MassAggregateApp(6, w, h, t),
        mass_position(0, 0, 0.5f) {
    init();
  }
  void init() {

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

  virtual int init_shaders() override {
    lamp_shader = mk_pointlight_lamp_shader();
    lamp_shader.useProgram();
    lamp_shader.setVec3Uni("lightColor", light.emitColor);

    // set object shader values
    obj_shader = mk_const_color_mesh_shader();

    obj_shader.useProgram();
    float ambientCoeff = 0.1f;
    glm::vec3 attc(1.0f, 0.0f, 0.0f);
    obj_shader.setFloatUni("ambientCoeff", ambientCoeff);
    obj_shader.setVec3Uni("attC", attc);
    obj_shader.setVec3Uni("diffColor",
                          glm::vec3(0.8, 0.6, 0.3));
    // texture
    obj_shader.setIntUni("shadowMap", 0);

    // set plane shader for ground
    plane_shader = mk_const_color_mesh_shader();
    plane_shader.useProgram();
    plane_shader.setFloatUni("ambientCoeff", ambientCoeff);
    plane_shader.setVec3Uni("attC", attc);
    plane_shader.setVec3Uni("diffColor",
                            glm::vec3(0.8, 0.8, 0.6));
    // texture
    plane_shader.setIntUni("shadowMap", 0);

    // make line shader
    line_shader = mk_line_shader();
    line_shader.useProgram();
    line_shader.setVec3Uni("lightColor",
                           line_color.to_glm());

    // depth shader for shadows
    depth_shader = mk_depth_shader();
    set_view();
    return 0;
  }
  virtual void set_scene_objects() override {
    cube = SimpleShape(1, false, ShapeChoice::CUBE);
    lamp = SimpleShape(1, false, ShapeChoice::LAMP);
    plane = SimpleShape(1, false, ShapeChoice::PLANE);

    lines =
        SimpleShape(ROD_COUNT, false, ShapeChoice::LINE);
  }

  void set_view() override {
    // setting model, view, projection
    projection =
        glm::perspective(glm::radians(camera.zoom),
                         (float)width / (float)height,
                         near_plane_dist, far_plane_dist);
    viewMat = camera.getViewMatrix();

    glm::mat4 lightProj;
    glm::mat4 lightView;
    lightView = glm::lookAt(light.position, glm::vec3(0.0),
                            glm::vec3(0.0, 1.0, 0.0));
    lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
                           1.0f, 7.5f);
    lightSpaceMatrix = lightProj * lightView;
  }

  void render_line() {
    std::vector<std::array<glm::vec3, 2>> verts;
    for (unsigned int i = 0; i < ROD_COUNT; i++) {
      ParticleRod rod = rods[i];
      ContactParticles contact_particles = rod.contact_ps;
      Particles ps = contact_particles.ps;
      D_CHECK_MSG(contact_particles.is_double,
                  "cotanct particles must be double");
      auto p1pos = ps[0]->get_position();
      auto p2pos = ps[1]->get_position();
      std::array<glm::vec3, 2> vs = {p1pos, p2pos};
      verts.push_back(vs);
    }
    line_shader.useProgram();
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(
        modelMat, mass_display_position.to_glm());
    line_shader.setVec3Uni("lightColor", glm::vec3(0.0f));
    line_shader.setMat4Uni("model", modelMat);

    lines.draw_line(verts);
    modelMat = glm::mat4(1.0f);
  }
  virtual void draw_objects() override {

    /** draw scene from light's perspective */
    draw_to_depth_fbo();

    /** reset viewport after depth rendering */
    reset_frame();

    /** render scene objects to scene */
    obj_shader.useProgram();

    /** bind depth texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_texture);

    /** set object view and projection matrix */
    set_object_vp();

    /** draw scene objects */
    render_scene_objects();

    /** draw plane on which we can see the shadows */
    render_plane();

    // done with depth texture
    glBindTexture(GL_TEXTURE_2D, 0);

    /** draw lamp object */
    render_lamp();

    /** draw line object*/
    render_line();
  }
};
};

#pragma once
/**
  Ballistic demo with new architecture
 */
#include <demos/camera.hpp>
#include <demos/gamemanager.hpp>
#include <demos/gameobj.hpp>
#include <demos/light.hpp>
#include <demos/mesh.hpp>
#include <demos/shader.hpp>
#include <demos/transformable.hpp>
#include <external.hpp>

using namespace vivademos;

namespace vivademos {

enum class AmmoType { UNUSED, PISTOL, ARTILLERY, FIREBALL, LASER };

std::ostream &operator<<(std::ostream &out, const AmmoType a) {
  switch (a) {
  case AmmoType::UNUSED: {
    out << "AmmoType::UNUSED";
    break;
  }
  case AmmoType::PISTOL: {
    out << "AmmoType::PISTOL";
    break;
  }
  case AmmoType::ARTILLERY: {
    out << "AmmoType::ARTILLERY";
    break;
  }
  case AmmoType::FIREBALL: {
    out << "AmmoType::FIREBALL";
    break;
  }
  case AmmoType::LASER: {
    out << "AmmoType::LASER";
    break;
  }
  }
  return out;
}

TransformableMesh mk_platform() {
  Mesh m4 = SimpleQuatMesh(); // platform

  auto ts = Transformable();
  auto tms = TransformableMesh(m4, ts, vivaphysics::v3(1.0));
  // change quat
  tms.transform.sc.scale_check = true;
  tms.transform.sc.set_scale(vivaphysics::v3(10, 10, 0));
  glm::mat4 m = glm::mat4(1.0f);
  m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
  glm::mat3 mat(m);
  tms.transform.rot = Rotatable::fromRotationMatrix(mat);
  tms.transform.trans.set_position(vivaphysics::v3(0, -1.0, 0));
  tms.m_color = vivaphysics::v3(0.3, 0.5, 0.7);
  return tms;
}
struct GameBallistic {
  GLFWwindow *window;
  TransformableMesh platform;
  ParticleMesh pmesh;
  std::vector<Shader> shaders; //
  SimpleCamera cam;
  PointLight light;
  vivaphysics::real last_time;
  AmmoType atype = AmmoType::UNUSED;
  unsigned int current_atype = 0;

  // game window size
  unsigned int window_width;
  unsigned int window_height;

  std::string mname = "0";

  GameBallistic() {}

  struct mesh_locks {
    bool cam = false;
    bool light = false;
  }; // camera, light, object locks respectively
  mesh_locks locks;

  // change particle values based on ammo type
  void change_ammo_type() {
    //
    current_atype++;
    if (current_atype > 4)
      current_atype = 0;
    if (current_atype == 0) {
      atype = AmmoType::UNUSED;
    } else if (current_atype == 1) {
      atype = AmmoType::PISTOL;
    } else if (current_atype == 2) {
      atype = AmmoType::ARTILLERY;
    } else if (current_atype == 3) {
      atype = AmmoType::FIREBALL;
    } else {
      atype = AmmoType::LASER;
    }
  }
  void fire() {
    ParticleParams params;
    switch (atype) {
    case AmmoType::UNUSED: {
      return;
      break;
    }
    case AmmoType::PISTOL: {
      params.mass = 2.0f;
      params.velocity = vivaphysics::v3(0, 0, 35);
      params.acceleration = vivaphysics::v3(0, -1, 0);
      params.damping = 0.99;
      break;
    }
    case AmmoType::ARTILLERY: {
      params.mass = 200.0f;
      params.velocity = vivaphysics::v3(0, 30, 40);
      params.acceleration = vivaphysics::v3(0, -20.0, 0);
      params.damping = 0.99;
      break;
    }
    case AmmoType::FIREBALL: {
      params.mass = 1.0f;
      params.velocity = vivaphysics::v3(0, 0, 10);
      params.acceleration = vivaphysics::v3(0, 0.6, 0);
      params.damping = 0.9;
      break;
    }
    case AmmoType::LASER: {
      params.mass = 0.1f;
      params.velocity = vivaphysics::v3(0, 0, 100);
      params.acceleration = vivaphysics::v3(0, 0.0, 0); // no gravity
      params.damping = 0.99;
      break;
    }

      // set position
      pmesh.particle.set_position(1.0, 1.0, 1.0);

      // set starting time to particle
      pmesh.start_time = last_time;

      // clear any accumulated force over particle
      pmesh.particle.clear_accumulator();
    }
  }
  void set_window(GLFWwindow *wind) {
    window = wind;
    int w, h;
    glfwGetWindowSize(wind, &w, &h);
    window_width = static_cast<unsigned int>(w);
    window_height = static_cast<unsigned int>(h);
  }
  void process_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
    if (!locks.cam) {
      moveCamera();
    }
    if (!locks.light) {
      moveLight();
    }

    if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)) {
      print_keys();
    }
    process_particle();
    process_toggles();
  }
  void process_particle() {
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) &&
        glfwGetKey(window, GLFW_KEY_KP_7)) {
      fire();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) &&
        glfwGetKey(window, GLFW_KEY_KP_8)) {
      change_ammo_type();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) &&
        glfwGetKey(window, GLFW_KEY_KP_9)) {
      std::cout << atype << std::endl;
    }
  }
  void process_toggles() {
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) {
      toggle_camera_movement();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
      toggle_light_movement();
    }
  }
  void toggle_camera_movement() {
    if (locks.cam) {
      locks.cam = false;
    } else {
      locks.cam = true;
    }
  }
  void toggle_light_movement() {
    if (locks.light) {
      locks.light = false;
    } else {
      locks.light = true;
    }
  }
  vivaphysics::real dtime() {
    auto current = static_cast<vivaphysics::real>(glfwGetTime());
    auto d = current - last_time;
    last_time = current;
    return d;
  }
  /**move camera using a model matrix*/
  void moveCamera() {
    auto delta = 0.1f;
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      cam.processKeyboard(MOVE_DIRECTION::FORWARD, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      cam.processKeyboard(MOVE_DIRECTION::BACKWARD, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      cam.processKeyboard(MOVE_DIRECTION::LEFT, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      cam.processKeyboard(MOVE_DIRECTION::RIGHT, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
      //
      cam.processKeyboardRotate(ROTATE_DIRECTION::RIGHT, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
      cam.processKeyboardRotate(ROTATE_DIRECTION::LEFT, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
      cam.processKeyboardRotate(ROTATE_DIRECTION::FORWARD, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
      cam.processKeyboardRotate(ROTATE_DIRECTION::BACKWARD, delta);
    }
  }
  void moveLight() {
    auto delta = 0.1f;

    if ((glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)) {
      light.processKeyboard(MOVE_DIRECTION::UP, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)) {
      light.processKeyboard(MOVE_DIRECTION::DOWN, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)) {
      light.processKeyboard(MOVE_DIRECTION::RIGHT, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)) {
      light.processKeyboard(MOVE_DIRECTION::LEFT, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)) {
      light.processKeyboard(MOVE_DIRECTION::FORWARD, delta);
    }
    if ((glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)) {
      light.processKeyboard(MOVE_DIRECTION::BACKWARD, delta);
    }
  }
  void print_toggle_movement_keys() const {
    std::cout << "Toggle Movement:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "key pad 7 camera" << std::endl;
    std::cout << "key pad 8 light" << std::endl;
    std::cout << "key pad 9 object" << std::endl;
    std::cout << "-------------------------" << std::endl;
  }
  void print_move_rotate_camera_keys() const {
    std::cout << "Move Rotate Camera Keys:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "UP_ARROW move forward" << std::endl;
    std::cout << "LEFT_ARROW move left" << std::endl;
    std::cout << "DOWN_ARROW move backward" << std::endl;
    std::cout << "RIGHT_ARROW move right" << std::endl;
    std::cout << "CTRL+LEFT rotate left" << std::endl;
    std::cout << "CTRL+RIGHT rotate right" << std::endl;
    std::cout << "CTRL+DOWN rotate downward" << std::endl;
    std::cout << "CTRL+UP rotate upward" << std::endl;
    std::cout << "-------------------------" << std::endl;
  }
  void print_move_light_keys() const {
    std::cout << "Move Light Keys:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "key pad 1 move upward" << std::endl;
    std::cout << "key pad 2 move downward" << std::endl;
    std::cout << "key pad 3 move right" << std::endl;
    std::cout << "key pad 4 move left" << std::endl;
    std::cout << "key pad 5 move forward" << std::endl;
    std::cout << "key pad 6 move backward" << std::endl;
    std::cout << "-------------------------" << std::endl;
  }
  void print_particle_keys() {
    std::cout << "Particle Controls:" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "CTRL + key pad 7: fire" << std::endl;
    std::cout << "CTRL + key pad 8: change ammo type" << std::endl;
    std::cout << "CTRL + key pad 9: print current ammo type" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
  }
  void print_keys() {
    std::cout << "Print Keys:" << std::endl;
    std::cout << "SPACE + V" << std::endl;
    std::cout << "-------------------------" << std::endl;
    print_toggle_movement_keys();
    print_move_rotate_camera_keys();
    print_move_light_keys();
    print_particle_keys();
  }
  bool should_close() const { return glfwWindowShouldClose(window); }
};
template <> struct GameManager<GameBallistic> {
  // call at every frame
  static void process_input(GameBallistic &game) { game.process_input(); }

  // call at every frame
  static bool should_close(GameBallistic &game) { return game.should_close(); }

  // call at every frame
  static void draw(GameBallistic &game) {

    // clear the current frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update uniform values
    auto projection =
        game.cam.perspective(game.window_width, game.window_height);
    // get camera view matrix;
    glm::mat4 view = game.cam.get_view_matrix();

    // set model matrix
    {
      // draw light
      Shader light_shader = game.shaders[1];
      // game.light.trans =
      // Translatable(vivaphysics::v3(glm::vec3(0.0f,3.0f,0.0)));
      auto lightModel = game.light.trans.translate();
      light_shader.useProgram();
      light_shader.setMat4Uni("model", lightModel);
      light_shader.setMat4Uni("view", view);
      light_shader.setMat4Uni("projection", projection);
      Mesh m5 = SimpleTriangleMesh();
      Transformable ts = Transformable(game.light.trans);
      TransformableMesh tmesh = TransformableMesh(m5, ts);
      tmesh.draw(light_shader);
      gerr();
    }

    Shader object_shader = game.shaders[0];
    object_shader.useProgram();
    object_shader.setMat4Uni("view", view);
    object_shader.setMat4Uni("projection", projection);
    // setup light values
    object_shader.setVec3Uni("lightPos", game.light.trans.position.to_glm());
    gerr();

    object_shader.setVec3Uni("lightColor", game.light.emitColor.to_glm());
    gerr();

    gerr();
    // redraw meshes
    {
      // draw platform
      glm::mat4 nmodel = game.platform.model();
      // set object model view projection values
      object_shader.setMat4Uni("model", nmodel);
      gerr();
      object_shader.setVec3Uni("diffColor", game.platform.color());
      game.platform.draw(object_shader);
    }
    {
      // draw particle mesh
      glm::mat4 nmodel = game.pmesh.model();
      // set object model view projection values
      object_shader.setMat4Uni("model", nmodel);
      gerr();
      object_shader.setVec3Uni("diffColor", game.pmesh.color());
      game.pmesh.draw(object_shader);
    }
  }

  // call at every frame for physics update
  static void update(GameBallistic &game) {

    // update particle
    auto duration = static_cast<float>(game.dtime());
    if (duration <= 0.0)
      return;

    if (game.atype != AmmoType::UNUSED) {
      // update particle
      game.pmesh.particle.integrate(duration);
      // check if the shot is still visible
      // not on screen
      bool cond1 = game.pmesh.particle.get_position().y() < 0.0f;

      // takes too long
      // bool cond2 = game.pmesh.start_time + 5000 < game.last_time;

      // beyond visible range
      bool cond3 = game.pmesh.particle.get_position().z() > game.cam.far;
      if (cond1 || cond3) {
        game.atype = AmmoType::UNUSED;
      }
    }
    //
    glfwSwapBuffers(game.window);
    glfwPollEvents();
  }

  // executed once per level
  static void unload(GameBallistic &game) {
    game.mname = "4";
    return;
  }
  static void load(GameBallistic &game) {

    game.mname = "2";
    // handle ressource management from file system

    return;
  }
  // execute once per level
  static void start(GameBallistic &game) {
    // set up mesh data
    game.mname = "1";

    // set platform
    auto platform = mk_platform();
    game.platform = platform;

    // set particle mesh
    auto cube = SimpleCubeMesh();
    game.pmesh = ParticleMesh(cube, vivaphysics::v3(0.7, 0.4, 0.6));

    // set up shader data

    // set up object shader
    Shader obj_shader = mk_simple_mesh_shader2();
    game.shaders.push_back(obj_shader);

    // set up light shader
    Shader light_shader = mk_pointlight_lamp_shader();
    light_shader.useProgram();
    light_shader.setVec3Uni("lightColor", glm::vec3(1.0f));
    gerr();
    game.shaders.push_back(light_shader);

    // set up camera
    game.cam = SimpleCamera(glm::vec3(0.0f, 0.0f, 10.0f));

    game.cam.set_near(0.1);
    game.cam.set_far(1000.0);

    // set up light
    auto lcolor = glm::vec3(1.0, 1.0, 1.0f);
    auto lpos = glm::vec3(0.2f, 1.0f, 0.5f);
    game.light = PointLight(lcolor, lpos);

    // set up last time
    game.last_time = static_cast<vivaphysics::real>(glfwGetTime());

    return;
  }
};

} // namespace vivademos

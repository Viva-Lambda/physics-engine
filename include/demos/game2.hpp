#pragma once
// basic skeleton for showing demos of our physics engine
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

std::vector<TransformableMesh> mk_game2_mesh_obj() {
  Mesh m0 = SimpleCubeMesh();
  Mesh m1 = SimpleCubeMesh();
  Mesh m2 = SimpleCubeMesh();
  Mesh m3 = SimpleTriangleMesh();
  Mesh m4 = SimpleQuatMesh(); // platform

  std::vector<Mesh> ms;
  ms.push_back(m0);
  ms.push_back(m1);
  ms.push_back(m2);
  ms.push_back(m3);
  ms.push_back(m4);

  std::vector<Transformable> ts;

  glm::vec3 startpos(0.0f);
  unsigned int i = 0;
  for (i = 0; i < 5; i++) {
    vivaphysics::real p = static_cast<vivaphysics::real>(i * 1);
    startpos.z += static_cast<float>(p);

    Rotatable r = Rotatable::fromEulerAngles(0, p, 0);

    Translatable tr = Translatable(vivaphysics::v3(startpos));

    Transformable t = Transformable(tr, r);

    ts.push_back(t);
  }

  std::vector<TransformableMesh> tms;

  for (i = 0; i < 5; i++) {
    TransformableMesh t(
        ms[i], ts[i], vivaphysics::v3(static_cast<vivaphysics::real>(i) / 5.0));
    tms.push_back(t);
  }
  // change quat
  tms[4].transform.sc.scale_check = true;
  tms[4].transform.sc.set_scale(vivaphysics::v3(10, 10, 0));
  glm::mat4 m = glm::mat4(1.0f);
  m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
  glm::mat3 mat(m);
  tms[4].transform.rot = Rotatable::fromRotationMatrix(mat);
  tms[4].transform.trans.set_position(vivaphysics::v3(0, -1.0, 0));
  tms[4].m_color = vivaphysics::v3(0.3, 0.5, 0.3);
  return tms;
}
struct Game2 {
  GLFWwindow *window;
  std::vector<TransformableMesh> shapes;
  std::vector<Shader> shaders; //
  SimpleCamera cam;
  PointLight light;
  vivaphysics::real last_time;

  // game window size
  unsigned int window_width;
  unsigned int window_height;

  std::string mname = "0";

  Game2() {}

  struct mesh_locks {
    bool cam = false;
    bool light = false;
  }; // camera, light, object locks respectively
  mesh_locks locks;

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
    process_toggles();
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
    if ((glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)) {
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
  void print_keys() {
    std::cout << "Print Keys:" << std::endl;
    std::cout << "SPACE + V" << std::endl;
    std::cout << "-------------------------" << std::endl;
    print_toggle_movement_keys();
    print_move_rotate_camera_keys();
    print_move_light_keys();
  }
  bool should_close() const { return glfwWindowShouldClose(window); }
};
template <> struct GameManager<Game2> {
  // call at every frame
  static void process_input(Game2 &game) { game.process_input(); }

  // call at every frame
  static bool should_close(Game2 &game) { return game.should_close(); }

  // call at every frame
  static void draw(Game2 &game) {

    // clear the current frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update uniform values
    auto projection =
        game.cam.perspective(game.window_width, game.window_height);
    // get camera view matrix;
    glm::mat4 view = game.cam.get_view_matrix();

    // set model matrix
    glm::mat4 model = glm::mat4(1.0f);
    Shader object_shader = game.shaders[0];
    gerr();
    glm::vec3 startpos(0.0f);
    glm::vec3 color = glm::vec3(1);

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

    // redraw meshes
    for (unsigned int i = 0; i < game.shapes.size(); i++) {
      TransformableMesh m = game.shapes[i];
      gerr();

      glm::mat4 nmodel = m.model();

      object_shader.useProgram();
      // setup light values
      object_shader.setVec3Uni("lightPos", game.light.trans.position.to_glm());
      object_shader.setVec3Uni("lightColor", game.light.emitColor.to_glm());
      object_shader.setVec3Uni("attC", glm::vec3(1.0f, 0.1f, 0.01f));
      object_shader.setFloatUni("ambientCoeff", 0.1f);

      // set object model view projection values
      object_shader.setMat4Uni("model", nmodel);
      gerr();

      object_shader.setMat4Uni("view", view);
      object_shader.setVec3Uni("diffColor", m.color());
      object_shader.setMat4Uni("projection", projection);

      m.draw(object_shader);
    }
  }

  // call at every frame for physics update
  static void update(Game2 &game) {

    game.mname = "3";
    //
    glfwSwapBuffers(game.window);
    glfwPollEvents();
  }

  // executed once per level
  static void unload(Game2 &game) {
    game.mname = "4";
    return;
  }
  static void load(Game2 &game) {

    game.mname = "2";
    // handle ressource management from file system

    return;
  }
  // execute once per level
  static void start(Game2 &game) {
    // set up mesh data
    game.mname = "1";
    auto tms = mk_game2_mesh_obj();
    game.shapes.clear();
    for (const auto &tm : tms) {
      game.shapes.push_back(tm);
    }

    // set up shader data

    // set up object shader
    Shader obj_shader = mk_simple_mesh_shader2();
    obj_shader.useProgram();
    obj_shader.setVec3Uni("diffColor", glm::vec3(0.1, 0.1, 0.6));
    gerr();
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
    game.light =
        PointLight(glm::vec3(0.6, 0.6, 1.0f), glm::vec3(0.2f, 1.0f, 0.5f));

    // set up last time
    game.last_time = static_cast<vivaphysics::real>(glfwGetTime());

    return;
  }
};

} // namespace vivademos

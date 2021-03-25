#pragma once
// show a mesh on window
#include <demos/app.hpp>
#include <demos/camera.hpp>
#include <demos/light.hpp>
#include <demos/mesh.hpp>
#include <demos/shader.hpp>
#include <external.hpp>

using namespace vivademos;
namespace vivademos {

class MeshDemoApp : public DemoApp {
protected:
  // window size related
  Mesh mesh;
  Mesh lamp;
  Shader obj_shader;
  Shader lamp_shader;

  // SpotLight light = SpotLight(glm::vec3(1.0f),
  //                            glm::vec3(0.2f, 1.0f, 0.5f),
  //                            glm::vec3(0, 1, 0));
  PointLight light =
      PointLight(glm::vec3(0.6, 0.6, 1.0f),
                 glm::vec3(0.2f, 1.0f, 0.5f));

  Camera camera;
  // constraints
  bool is_camera_locked = false;
  bool is_light_locked = false;
  bool is_object_locked = false;

  double last_time;
  glm::vec3 transVec = glm::vec3(1.0);
  glm::mat4 modelMat = glm::mat4(1.0f);
  glm::mat4 lampMat = glm::mat4(1.0f);

public:
  MeshDemoApp() {}
  MeshDemoApp(int w, int h, std::string title)
      : DemoApp(w, h, title),
        camera(Camera(glm::vec3(0.0f, 0.0f, 10.0f))),
        last_time(glfwGetTime()) {}

  /**
    Basically we are specifying the default key map as
    enums associated to functions. The functions capture
    their significant argument by reference and change
    their value. `&` for capturing them by reference
    `mutable` for changing their value inside the lambda.
   */
  void default_map() override {}

  void toggle_camera_movement() {
    if (is_camera_locked) {
      is_camera_locked = false;
    } else {
      is_camera_locked = true;
    }
  }

  void toggle_light_movement() {
    if (is_light_locked) {
      is_light_locked = false;
    } else {
      is_light_locked = true;
    }
  }

  void toggle_object_movement() {
    if (is_object_locked) {
      is_object_locked = false;
    } else {
      is_object_locked = true;
    }
  }

  std::string get_title() override { return "mesh viewer"; }
  int init_graphics() override {
    DemoApp::init_graphics();
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    mesh = mk_cube();
    lamp = mk_cube();

    // set lamp shader values
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
                          glm::vec3(0.3, 0.3, 0.3));
    set_view();
    return 0;
  }
  void set_view() override {
    // setting model, view, projection

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.zoom),
        (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 viewMat = camera.getViewMatrix();
    glm::vec3 viewPos = camera.pos;
    // render cube object

    obj_shader.useProgram();
    obj_shader.setMat4Uni("view", viewMat);
    obj_shader.setMat4Uni("model", modelMat);
    obj_shader.setMat4Uni("projection", projection);
    // obj_shader.setVec3Uni("viewPos", viewPos);
    obj_shader.setVec3Uni("lightPos", light.position);
    obj_shader.setFloatUni("lightIntensity", 1.0f);

    // render light
    // lamp_shader.useProgram();
    // lamp_shader.setMat4Uni("view", viewMat);
    // lamp_shader.setMat4Uni("model", lampMat);
    // lamp_shader.setMat4Uni("projection", projection);
  }

  /** display application content*/
  int display() override {
    default_map();
    print_keys();
    while (!glfwWindowShouldClose(window)) {
      update();
    }
    destroy_graphics();
    return 0;
  }
  void update() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mesh.draw(obj_shader);
    // lamp.draw(lamp_shader);
    process_input();
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  void resize(unsigned int w, unsigned int h) override {
    width = w;
    height = h;
    set_view();
  }
  void query_key(int key) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      key_map[key]();
    }
  }
  void key(int key) override {
    auto it = key_map.find(key);
    D_CHECK_MSG(it != key_map.end(), "key not in key maps");
    query_key(key);
  }
  /** process certain keys given in key map */
  void process_input() override {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
    process_toggles();
    if (!is_camera_locked) {
      moveCamera();
    }
    if (!is_object_locked) {
      moveObject();
    }
    if (!is_light_locked) {
      moveLight();
    }
    set_view();

    if ((glfwGetKey(window, GLFW_KEY_SPACE) ==
         GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)) {
      print_keys();
    }
  }
  double dtime() {
    auto current = glfwGetTime();
    auto d = current - last_time;
    last_time = current;
    return d;
  }
  void moveCamera() {
    //
    auto deltaTime = dtime();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      camera.processKeyboard(Camera_Movement::FORWARD,
                             deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      camera.processKeyboard(Camera_Movement::LEFT,
                             deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      camera.processKeyboard(Camera_Movement::BACKWARD,
                             deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      camera.processKeyboard(Camera_Movement::RIGHT,
                             deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
      camera.processKeyboardRotate(Camera_Movement::LEFT,
                                   0.7f);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
      camera.processKeyboardRotate(Camera_Movement::RIGHT,
                                   0.7f);
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
      camera.processKeyboardRotate(Camera_Movement::FORWARD,
                                   0.7f);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
      camera.processKeyboardRotate(
          Camera_Movement::BACKWARD, 0.7f);
    }
  }
  void moveLight() {
    // move light
    auto deltaTime = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
      light.position.y += deltaTime;
      lampMat = glm::translate(lampMat, light.position);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
      light.position.y -= deltaTime;
      lampMat = glm::translate(lampMat, light.position);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) {
      light.position.x += deltaTime;
      lampMat = glm::translate(lampMat, light.position);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
      light.position.x -= deltaTime;
      lampMat = glm::translate(lampMat, light.position);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
      light.position.z -= deltaTime; // the axis are inverse
      lampMat = glm::translate(lampMat, light.position);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
      light.position.z += deltaTime;

      lampMat = glm::translate(lampMat, light.position);
    }
  }

  void moveObject() {
    //
    auto deltaTime = dtime();
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      transVec.x = deltaTime;
      transVec.y = 0.0f;
      transVec.z = 0.0f;
      modelMat = glm::translate(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
      transVec.x = -deltaTime;
      transVec.y = 0.0f;
      transVec.z = 0.0f;

      modelMat = glm::translate(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
      transVec.y = deltaTime;
      transVec.x = 0.0f;
      transVec.z = 0.0f;

      modelMat = glm::translate(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
      transVec.y = -deltaTime;
      transVec.x = 0.0f;
      transVec.z = 0.0f;

      modelMat = glm::translate(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
      transVec.z = deltaTime;
      transVec.y = 0.0f;
      transVec.x = 0.0f;

      modelMat = glm::translate(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
      transVec.z = -deltaTime;
      transVec.y = 0.0f;
      transVec.x = 0.0f;

      modelMat = glm::translate(modelMat, transVec);
    }
  }
  void process_toggles() {
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) {
      toggle_camera_movement();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
      toggle_light_movement();
    }
    if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) {
      toggle_object_movement();
    }
  }
  void print_keys() {
    std::cout << "Print Keys:" << std::endl;
    std::cout << "SPACE + V" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Toggle Movement:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "key pad 7 camera" << std::endl;
    std::cout << "key pad 8 light" << std::endl;
    std::cout << "key pad 9 object" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Move Rotate Camera Keys:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "W/Z move forward" << std::endl;
    std::cout << "A/Q move left" << std::endl;
    std::cout << "S move backward" << std::endl;
    std::cout << "D move right" << std::endl;
    std::cout << "H rotate left" << std::endl;
    std::cout << "J rotate right" << std::endl;
    std::cout << "K rotate downward" << std::endl;
    std::cout << "L rotate upward" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Move Light Keys:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "key pad 1 move upward" << std::endl;
    std::cout << "key pad 2 move downward" << std::endl;
    std::cout << "key pad 3 move right" << std::endl;
    std::cout << "key pad 4 move left" << std::endl;
    std::cout << "key pad 5 move forward" << std::endl;
    std::cout << "key pad 6 move backward" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Move Object Keys:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "E move left" << std::endl;
    std::cout << "R move right" << std::endl;
    std::cout << "T move up" << std::endl;
    std::cout << "Y move down" << std::endl;
    std::cout << "I move forward" << std::endl;
    std::cout << "O move backward" << std::endl;
    std::cout << "-------------------------" << std::endl;
  }

  void render_text(int x, int y, std::string txt,
                   void *font = nullptr) override {}
};
};

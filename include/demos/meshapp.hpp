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
  SimpleShape cube;
  SimpleShape lamp;
  SimpleShape plane;

  //
  Shader obj_shader;
  Shader depth_shader;
  Shader lamp_shader;
  Shader plane_shader;

  // depth map related
  const unsigned int SHADOW_WIDTH = 1024;
  const unsigned int SHADOW_HEIGHT = 1024;
  GLuint depth_map_fbo;
  GLuint depth_texture;

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
  // camera related mvp matrices
  glm::vec3 transVec = glm::vec3(1.0);
  glm::mat4 modelMat = glm::mat4(1.0f);
  glm::mat4 lampMat = glm::mat4(1.0f);
  glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
  //
  glm::mat4 viewMat = glm::mat4(1.0);
  glm::mat4 projection = glm::mat4(1.0);
  float near_plane_dist = 0.1f;
  float far_plane_dist = 100.0f;

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

  std::string get_title() override {
    return "cube mesh viewer";
  }
  int init_shaders() {
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

    // depth shader for shadows
    depth_shader = mk_depth_shader();
    set_view();
    return 0;
  }

  virtual void set_scene_objects() {
    cube = SimpleShape(1, false, ShapeChoice::CUBE);
    lamp = SimpleShape(1, false, ShapeChoice::LAMP);
    plane = SimpleShape(1, false, ShapeChoice::PLANE);
  }

  virtual void set_depth_fbo() {
    glGenFramebuffers(1, &depth_map_fbo);
    glGenTextures(1, &depth_texture);

    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
                     borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, depth_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  virtual void set_related() {
    set_scene_objects();
    set_depth_fbo();
  }

  int init_graphics() override {
    DemoApp::init_graphics();
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
    // render cube object

    glm::mat4 lightProj, lightSpaceMat, lightView;
    lightView = glm::lookAt(light.position, glm::vec3(0.0),
                            glm::vec3(0.0, 1.0, 0.0));
    lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
                           1.0f, 7.5f);
    lightSpaceMatrix = lightProj * lightView;
  }

  /** display application content*/
  int display() override {
    print_keys();
    while (!glfwWindowShouldClose(window)) {
      update();
    }
    destroy_graphics();
    return 0;
  }
  /** \brief render scene from light's perspective*/
  virtual void render_scene_depth() {
    depth_shader.useProgram();
    depth_shader.setMat4Uni("lightSpaceMatrix",
                            lightSpaceMatrix);
    glm::mat4 identityModel = glm::mat4(1.0f);

    // draw scene from light's perspective
    depth_shader.setMat4Uni("model", modelMat);
    cube.draw();

    depth_shader.setMat4Uni("model", identityModel);
    plane.draw();

    depth_shader.setMat4Uni("model", lampMat);
    lamp.draw();
  }
  virtual void draw_to_depth_fbo() {
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
  virtual void set_object_vp() {
    obj_shader.useProgram();
    obj_shader.setMat4Uni("view", viewMat);
    obj_shader.setMat4Uni("projection", projection);

    // set light space matrix for shadow rendering
    obj_shader.setMat4Uni("lightSpaceMatrix",
                          lightSpaceMatrix);

    // set light position and other info for lightening
    // computation
    obj_shader.setVec3Uni("lightPos", light.position);
    obj_shader.setFloatUni("lightIntensity", 1.0f);
  }

  virtual void render_scene_objects() {
    // draw objects that need to be drawn
    obj_shader.setVec3Uni("diffColor",
                          glm::vec3(0.2, 0.7, 0.2));

    obj_shader.setMat4Uni("model", modelMat);
    cube.draw();

    // draw the plane
    glm::mat4 identityModel = glm::mat4(1.0f);

    obj_shader.setMat4Uni("model", identityModel);
    obj_shader.setVec3Uni("diffColor",
                          glm::vec3(0.9, 0.8, 0.6));
    plane.draw();

    //
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  virtual void render_lamp() {
    lamp_shader.useProgram();

    lampMat = glm::mat4(1.0f);
    lampMat = glm::scale(lampMat, glm::vec3(0.5f));
    lampMat = glm::translate(lampMat, light.position);

    // draw lamp
    lamp_shader.useProgram();
    // mvp for lamp
    lamp_shader.setMat4Uni("view", viewMat);
    lamp_shader.setMat4Uni("model", lampMat);
    lamp_shader.setMat4Uni("projection", projection);

    // lamp color
    lamp_shader.setVec3Uni("lightColor", glm::vec3(1.0f));

    //
    lamp.draw();
  }
  virtual void draw_objects() {
    draw_to_depth_fbo();

    // render object

    // reset viewport
    reset_frame();

    // render objects
    obj_shader.useProgram();

    // bind depth texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_texture);

    // set mvp
    set_object_vp();

    render_scene_objects();

    render_lamp();

    //
  }
  void clear_frame() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  void reset_frame() { glViewport(0, 0, width, height); clear_frame();}
  void update() override {
    reset_frame();
    draw_objects();
    set_view();
    update_glfw();
  }
  void update_glfw() {
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
  virtual void process_other_keys() {}
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
    // other child object keys
    process_other_keys();

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
  virtual void moveCamera() {
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
  virtual void moveLight() {
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

  virtual void set_model_mat(const glm::mat4 &mmat,
                             const glm::vec3 &trans) {

    modelMat = glm::translate(mmat, trans);
  }

  virtual void moveObject() {
    //
    auto deltaTime = dtime();
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      transVec.x = deltaTime;
      transVec.y = 0.0f;
      transVec.z = 0.0f;
      set_model_mat(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
      transVec.x = -deltaTime;
      transVec.y = 0.0f;
      transVec.z = 0.0f;

      set_model_mat(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
      transVec.y = deltaTime;
      transVec.x = 0.0f;
      transVec.z = 0.0f;

      set_model_mat(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
      transVec.y = -deltaTime;
      transVec.x = 0.0f;
      transVec.z = 0.0f;

      set_model_mat(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
      transVec.z = deltaTime;
      transVec.y = 0.0f;
      transVec.x = 0.0f;

      set_model_mat(modelMat, transVec);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
      transVec.z = -deltaTime;
      transVec.y = 0.0f;
      transVec.x = 0.0f;

      set_model_mat(modelMat, transVec);
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
  virtual void print_toggle_movement_keys() const {
    std::cout << "Toggle Movement:" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "key pad 7 camera" << std::endl;
    std::cout << "key pad 8 light" << std::endl;
    std::cout << "key pad 9 object" << std::endl;
    std::cout << "-------------------------" << std::endl;
  }
  virtual void print_move_rotate_camera_keys() const {
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
  }
  virtual void print_move_light_keys() const {
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
  virtual void print_move_obj_keys() {
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
  void print_keys() {
    std::cout << "Print Keys:" << std::endl;
    std::cout << "SPACE + V" << std::endl;
    std::cout << "-------------------------" << std::endl;
    print_toggle_movement_keys();
    print_move_rotate_camera_keys();
    print_move_light_keys();
    print_move_obj_keys();
  }

  void render_text(int x, int y, std::string txt,
                   void *font = nullptr) override {}
};
};

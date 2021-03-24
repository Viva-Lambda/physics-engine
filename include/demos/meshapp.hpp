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
  int height = 480;
  int width = 640;
  std::string window_title = "My Demo App";
  GLFWwindow *window;
  std::map<int, std::function<void(void)>> key_map;
  Mesh mesh = mk_cube();
  Shader shader = mk_const_color_mesh_shader();
  SpotLight light = SpotLight(glm::vec3(1.0f),
                              glm::vec3(0.2f, 1.0f, 0.5f),
                              glm::vec3(0, 1, 0));
  Camera camera;

public:
  MeshDemoApp() {}
  MeshDemoApp(int w, int h, std::string title)
      : width(w), height(h), window_title(title),
        camera(Camera(glm::vec3(0.0f, 0.0f, 2.3f))) {}

  void default_map() override {
    auto wind = window;
    key_map = {
        std::make_pair<int, std::function<void(void)>>(
            GLFW_KEY_ESCAPE, [wind]() {
              glfwSetWindowShouldClose(wind, true);
            })};
  }

  std::string get_title() override { return "mesh viewer"; }
  int init_graphics() override {
    DemoApp::init_graphics();
    shader.useProgram();
    float ambientCoeff = 0.1f;
    glm::vec3 attc(1.0f, 0.0f, 0.0f);
    shader.setFloatUni("ambientCoeff", ambientCoeff);
    shader.setVec3Uni("attC", attc);
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
    glm::mat4 cubeModel(1.0f);

    shader.useProgram();
    shader.setMat4Uni("view", viewMat);
    shader.setMat4Uni("model", cubeModel);
    shader.setMat4Uni("projection", projection);
    shader.setVec3Uni("viewPos", viewPos);
    shader.setVec3Uni("lightPos", light.position);
    shader.setFloatUni("lightIntensity", 1.0f);
  }

  /** display application content*/
  int display() override {
    while (!glfwWindowShouldClose(window)) {
      update();
    }
    destroy_graphics();
    return 0;
  }
  void update() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mesh.draw(shader);
    process_input();
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  void resize(unsigned int w, unsigned int h) override {
    width = w;
    height = h;
    set_view();
  }
  void key(int key) override {
    auto it = key_map.find(key);
    D_CHECK_MSG(it != key_map.end(), "key not in key maps");
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      key_map[key]();
    }
  }
  /** process certain keys given in key map */
  void process_input() override { key(GLFW_KEY_ESCAPE); }
  void render_text(int x, int y, std::string txt,
                   void *font = nullptr) override {}
};
};

#pragma once
// basic skeleton for showing demos of our physics engine
#include <demos/apputils.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>

using namespace vivaphysics;

namespace vivademos {

class DemoApp {
protected:
  // window size related
  int height = 480;
  int width = 640;
  std::string window_title = "My Demo App";
  GLFWwindow *window;
  std::map<int, std::function<void(void)>> key_map;

public:
  DemoApp() {}
  DemoApp(int w, int h, std::string title)
      : height(h), width(w), window_title(title) {}

  virtual void default_map() {
    auto wind = window;
    key_map = {
        std::make_pair<int, std::function<void(void)>>(
            GLFW_KEY_ESCAPE, [wind]() {
              glfwSetWindowShouldClose(wind, true);
            })};
  }

  virtual std::string get_title() { return window_title; }
  virtual int init_graphics() {
    initializeGLFWMajorMinor(4, 4);
    window = glfwCreateWindow(
        width, height, window_title.c_str(), NULL, NULL);
    if (window == NULL) {
      std::cout << "failed to create glfw window"
                << std::endl;
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);

    // window resize
    glfwSetFramebufferSizeCallback(
        window, framebuffer_size_callback);

    // load glad
    if (gladLoadGLLoader(
            (GLADloadproc)(glfwGetProcAddress)) == 0) {
      std::cout << "Failed to start glad" << std::endl;
      glfwTerminate();
      return -1;
    }

    // set default viewport
    glViewport(0, 0, width,
               height); // viewport equal to window

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    return 0;
  }
  virtual void set_view() {}

  /** clean up application related data*/
  virtual int destroy_graphics() {
    glfwTerminate();
    return 0;
  }
  /** display application content*/
  virtual int display() {
    default_map();
    while (!glfwWindowShouldClose(window)) {
      update();
    }
    destroy_graphics();
    return 0;
  }
  virtual void update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    process_input();
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  virtual void resize(unsigned int w, unsigned int h) {
    width = w;
    height = h;
    set_view();
  }
  virtual void key(int key) {
    auto it = key_map.find(key);
    D_CHECK_MSG(it != key_map.end(), "key not in key maps");
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      key_map[key]();
    }
  }
  /** process certain keys given in key map */
  virtual void process_input() { key(GLFW_KEY_ESCAPE); }
  virtual void mouse_drag(int x, int y) {}
  virtual void render_text(int x, int y, std::string txt,
                           void *font = nullptr) {}
};
};

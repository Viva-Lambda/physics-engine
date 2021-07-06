#pragma once
// basic skeleton for showing demos of our physics engine
#include <demos/gamemanager.hpp>
#include <external.hpp>

namespace vivademos {

struct Game {
  GLFWwindow *window;
  void set_window(GLFWwindow *wind) { window = wind; }
  virtual void process_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
  }
  bool should_close() const { return glfwWindowShouldClose(window); }
};
template <> struct GameManager<Game> {
  static void process_input(Game &game) { game.process_input(); }
  static bool should_close(Game &game) { return game.should_close(); }
  static void draw(Game &game) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  static void update(Game &game) {
    glfwSwapBuffers(game.window);
    glfwPollEvents();
  }
  static void unload(Game &game) { return; }
  static void load(Game &game) { return; }
  static void start(Game &game) { return; }
};
}; // namespace vivademos

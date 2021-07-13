#pragma once
// main engine component
#include <demos/apputils.hpp>
#include <demos/game.hpp>
#include <demos/gameloop.hpp>
#include <external.hpp>

using namespace vivademos;
namespace vivademos {
struct MainEngine {
  // window related data
  unsigned int window_width = 800;
  unsigned int window_height = 600;
  std::string window_title = "my game window";

  GLFWwindow *window;

  virtual bool init_graphics() {
    initializeGLFWMajorMinor(4, 4);
    window = glfwCreateWindow(window_width, window_height, window_title.c_str(),
                              NULL, NULL);
    if (window == NULL) {
      std::cout << "failed to create glfw window" << std::endl;
      glfwTerminate();
      return false;
    }
    glfwMakeContextCurrent(window);

    // window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // load glad
    if (gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)) == 0) {
      std::cout << "Failed to start glad" << std::endl;
      glfwTerminate();
      return false;
    }

    // set default viewport
    glViewport(0, 0, window_width,
               window_height); // viewport equal to window

    glClearColor(0.3f, 0.5f, 0.6f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    return true;
  }

  template <typename T> void start_scene(T &game, GameLoop<T> &loop) {
    // set game window
    game.set_window(window);

    // load game assets
    GameManager<T>::load(game);
    std::cout << game.mname << std::endl;

    // start game loop
    loop = GameLoop<T>();
    loop.start(game);
  }
  template <typename OldLevel, typename NewLevel>
  void load_next_scene(GameLoop<OldLevel> &old_loop, NewLevel &ngame,
                       GameLoop<NewLevel> &ngame_loop) {
    // stop the game loop
    old_loop.stop();

    // unload the ressources
    OldLevel prevLevel = old_loop.game();
    GameManager<OldLevel>::unload(prevLevel);

    // start the new scene
    start_scene(ngame, ngame_loop);
  }

  template <typename T> void start_main(T &game, GameLoop<T> &loop) {
    init_graphics();
    //
    start_scene<T>(game, loop);
  }

  void end() { glfwTerminate(); }
};
} // namespace vivademos

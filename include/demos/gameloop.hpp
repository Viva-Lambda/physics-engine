#pragma once
// game loop function for running the game
#include <demos/game.hpp>
#include <demos/gamemanager.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>

using namespace vivademos;
namespace vivademos {

template <class T> struct GameLoop {
  vivaphysics::real FPS = 60.0;
  vivaphysics::real MPF = 1000 / FPS;
  std::chrono::time_point<std::chrono::system_clock> prevTime =
      std::chrono::system_clock::now();
  std::chrono::duration<vivaphysics::real, std::milli> lagTime;
  std::chrono::time_point<std::chrono::system_clock> currentTime;
  std::chrono::duration<vivaphysics::real, std::milli> elapsedTime;

  //
  GameLoop() {}

  bool loop_running = false;

  T loop_game;

  void start_loop() {
    //
    // set time variables
    prevTime = std::chrono::system_clock::now();
    lagTime = std::chrono::duration<vivaphysics::real, std::milli>(0);

    // set running flag
    loop_running = true;

    // run loop
    run();
  }
  void start(T &game) {
    loop_game = game;

    // start the game
    GameManager<T>::start(game);

    // start the game loop
    start_loop();
  }
  void stop() { loop_running = false; }
  void run() {
    if (loop_running == true) {
      //
      // compute duration
      currentTime = std::chrono::system_clock::now();
      elapsedTime = currentTime - prevTime;
      prevTime = currentTime;
      lagTime += elapsedTime;

      // update game
      while ((lagTime.count() >= MPF) && loop_running) {
        //
        // game input update
        GameManager<T>::process_input(loop_game);
        // game physics update
        GameManager<T>::update(loop_game);
        lagTime -= std::chrono::duration<vivaphysics::real, std::milli>(MPF);
      }

      // draw game
      GameManager<T>::draw(loop_game);

      // check if the window should close
      // stop game loop then
      loop_running = GameManager<T>::should_close(loop_game) ? false : true;

    } else {
      // game loop stopped let's clean up
      GameManager<T>::unload(loop_game);
    }
  }
  bool is_running() { return loop_running; }
};

} // namespace vivademos

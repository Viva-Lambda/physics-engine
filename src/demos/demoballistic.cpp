// demo application window
#include <demos/gameballistic.hpp>
#include <demos/gameloop.hpp>
#include <demos/gamemanager.hpp>
#include <demos/mengine.hpp>
// #include <demos/ballistic.hpp>
using namespace vivademos;

int main() {
  // start engine
  MainEngine engine;

  //
  GameBallistic level2;
  GameLoop<GameBallistic> loop;
  engine.start_main<GameBallistic>(level2, loop);
  while (loop.is_active()) {
    // update game until the loop is stopped
    loop.run();
  }

  // we immediately unload the ressources for the current scene
  // since we don't have any other scenes
  level2 = loop.game();
  GameManager<GameBallistic>::unload(level2);

  // check game state [paused, success, fail, window close etc]

  // load next scene depending on the game state

  // MyNextLevel level2;
  // GameLoop<MyOtherLevel> loop2;
  // engine.load_next_scene<Game, MyOtherLevel>(loop, level1, loop2);

  // terminate program
  engine.end();
  return 0;
}

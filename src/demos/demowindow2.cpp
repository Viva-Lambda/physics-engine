// demo application window
#include <demos/game.hpp>
#include <demos/gameloop.hpp>
#include <demos/gamemanager.hpp>
#include <demos/mengine.hpp>
// #include <demos/ballistic.hpp>
using namespace vivademos;
int main() {
  // start engine
  MainEngine engine;

  //
  Game level1;
  GameLoop<Game> loop;
  engine.start_main<Game>(level1, loop);
  while (loop.is_active()) {
    // update game until the loop is stopped
    loop.run();
  }

  // we immediately unload the ressources for the current scene
  // since we don't have any other scenes
  level1 = loop.game();
  GameManager<Game>::unload(level1);

  // check game state [paused, success, fail, window close etc]

  // load next scene depending on the game state

  // MyNextLevel level2;
  // GameLoop<MyOtherLevel> loop2;
  // engine.load_next_scene<Game, MyOtherLevel>(loop, level1, loop2);

  // terminate program
  engine.end();
  return 0;
}

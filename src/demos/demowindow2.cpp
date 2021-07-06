// demo application window
#include <demos/game.hpp>
#include <demos/gameloop.hpp>
#include <demos/gamemanager.hpp>
#include <demos/mengine.hpp>
// #include <demos/ballistic.hpp>
using namespace vivademos;
int main() {
  Game game;
  MainEngine engine;
  GameLoop<Game> loop;
  engine.start_main<Game>(game, loop);
  while (loop.is_running()) {
    // update game until the loop is stopped
    loop.run();
  }
  // unload the current scene
  GameManager<Game>::unload(game);

  // load next scene

  // terminate program
  engine.end();
  return 0;
}

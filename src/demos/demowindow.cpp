// demo application window
#include <demos/app.hpp>
#include <demos/shader.hpp>
// #include <demos/ballistic.hpp>
using namespace vivademos;

int main() {
  auto demo = DemoApp(800, 600, "Basic Demo App Test");
  demo.init_graphics();
  demo.display();
  return 0;
}

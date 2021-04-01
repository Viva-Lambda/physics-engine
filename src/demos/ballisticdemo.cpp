// ballistic objects demo
#include <demos/ballistic.hpp>
// #include <demos/ballistic.hpp>
using namespace vivademos;

int main() {
  auto demo = BallisticMeshDemo(
      800, 600, "Ballistic Mesh Demo App Test");
  demo.init_graphics();
  demo.display();
  return 0;
}

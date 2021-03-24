// mesh demo application
#include <demos/meshapp.hpp>
using namespace vivademos;

int main() {
  auto demo =
      MeshDemoApp(800, 600, "Basic Mesh Demo App Test");
  demo.init_graphics();
  demo.display();
  return 0;
}

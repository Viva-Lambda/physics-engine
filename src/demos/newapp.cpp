#include <demos/app.hpp>
#include <demos/mesh.hpp>
// #include <demos/ballistic.hpp>
using namespace vivademos;
int main() {
  auto demo = PhyApp(800, 600, "Basic Demo App Test");
  bool success = demo.init();
  if (success) {
    demo.run();
  }
  bool cleaned_up = demo.clean_up();
  if (cleaned_up) {
    return 0;
  } else {
    return -1;
  }
}

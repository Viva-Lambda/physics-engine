// platform demo
#include <demos/platformapp.hpp>
using namespace vivademos;

int main() {
  auto demo = PlatformDemo(800, 600, "platform demo");
  demo.init_graphics();
  demo.display();
  return 0;
}

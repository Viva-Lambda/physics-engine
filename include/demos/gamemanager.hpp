#pragma once
// basic skeleton for showing demos of our physics engine

namespace vivademos {
template <class T> struct GameManager {
  static void load(T &game) {}
  static void unload(T &game) {}
  static void start(T &game) {}
  static void process_input(T &game) {}
  static bool should_close(T &game) {}
  static void update(T &game) {}
  static void draw(T &game) {}
};

} // namespace vivademos

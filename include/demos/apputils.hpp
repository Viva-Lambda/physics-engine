#pragma once
// utilty functions for demo application
#include <external.hpp>
void framebuffer_size_callback(GLFWwindow *window,
                               int newWidth,
                               int newHeight) {
  glViewport(0, 0, newWidth, newHeight);
}
void glfwErrorCallBack(int id, const char *desc) {
  std::cout << desc << std::endl;
}
void initializeGLFWMajorMinor(unsigned int maj,
                              unsigned int min) {
  // initialize glfw version with correct profiling etc
  glfwSetErrorCallback(glfwErrorCallBack);
  if (glfwInit() == 0) {
    std::cout << "glfw not initialized correctly"
              << std::endl;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, maj);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min);
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);
}

#pragma once
// application with glut window
#include <demos/timeutils.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>

using namespace vivaphysics;

namespace vivademos {
class DemoGlutApp {
protected:
  int height = 480;
  int width = 640;
  std::string window_title = "My Demo App";
  virtual std::string get_title() const {
    return window_title;
  }
  virtual void init_graphics() {
    glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    set_view();
  }
  virtual void set_view() {
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60.0, // fovy,
        static_cast<float>(width) /
            static_cast<float>(height), // aspect ratio
        1.0,  // near plane distance
        500.0 // far plane distance
        );
  }
  virtual void destroy_graphics() {}
  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    glVertex2i(1, 1);
    glVertex2i(639, 319);
    glEnd();
  }
  virtual void update() {
    //
    glutPostRedisplay();
  }
  virtual void key(unsigned char k) {}
  virtual void resize(int w, int h) {
    //
    h = h <= 0 ? 1 : h;
    w = w <= 0 ? 1 : w;
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    set_view();
  }
  virtual void mouse(int btn, int state, int x, int y) {}
  virtual void mouse_drag(int x, int y) {}
  virtual void render_text(float x, float y,
                           std::string text,
                           void *font = nullptr) {
    //
    glDisable(GL_DEPTH_TEST);

    //
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0,                        // left
            static_cast<float>(width),  // right
            0.0,                        // bottom
            static_cast<float>(height), // top
            -1.0,                       // near plane value
            1.0                         // far plane value
            );

    // set model view
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // ensure we have a font to display the text with
    if (!font) {
      font = GLUT_BITMAP_HELVETICA_12;
    }

    glRasterPos2f(x, y);
    // loop over characters
    for (unsigned int i = 0; i < text.size(); i++) {
      const char s = text[i];
      if (s == '\n') {
        y -= -12.0f;
        glRasterPos2f(x, y);
      }
      glutBitmapCharacter(font, s);
    }

    // set everything back
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
  }
};
};

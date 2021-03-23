// ballistic demo app
#pragma once
#include <demos/app.hpp>
#include <external.hpp>
#include <vivaphysics/core.h>
#include <vivaphysics/particle.hpp>

using namespace vivademos;

namespace vivademos {
class BallisticDemo : public DemoApp {
private:
  enum ShotType {
    UNUSED = 0,
    PISTOL = 1,
    ARTILLERY = 2,
    FIREBALL = 3,
    LASER = 4
  };
  struct AmmoRound {
    vivaphysics::Particle particle;
    ShotType stype;
    unsigned int start_time;
    void render() {
      vivaphysics::Vector3 pos;
      particle.get_position(pos);

      glColor3f(0, 0, 0);
      glPushMatrix();
      glTranslatef(pos.x, pos.y, pos.z);
      glutSolidSphere(0.3f, // radius
                      5,    //
                      4);
      glPopMatrix();
      glColor3f(0.75, 0.75, 0.75);
      glPushMatrix();
      glTranslatef(pos.x, 0, pos.z);
      glScalef(1.0f, 1.0f, 1.0f);
      glutSolidSphere(0.6f, // radius
                      5,    //
                      4);
      glPopMatrix();
    }
  };
  const unsigned int max_ammo = 16;
  AmmoRound ammo[16];

  /** holds the current shot type*/
  ShotType current_stype;

  /** dispatches the throwable*/
  void fire() {
    for (unsigned int i = 0; i < max_ammo; i++) {
      if (ammo[i].stype == UNUSED) {
        break;
      }
      if (i >= max_ammo)
        return;
    }
    // choose shot type
    AmmoRound *shot;
    switch (current_stype) {
    case PISTOL:
      shot->particle.set_mass(2.0f);
      shot->particle.set_velocity(0, 0, 35);
      shot->particle.set_acceleration(0, -1.0f, 0);
      shot->particle.set_damping(0.99f);
      break;
    case ARTILLERY:
      shot->particle.set_mass(200.0f);
      shot->particle.set_velocity(0, 30, 40);
      shot->particle.set_acceleration(0, -20.0f, 0);
      shot->particle.set_damping(0.99f);
      break;
    case FIREBALL:
      shot->particle.set_mass(1.0f);
      shot->particle.set_velocity(0, 0, 10);
      shot->particle.set_acceleration(0, 0.6f, 0);
      shot->particle.set_damping(0.9f);
      break;
    case LASER:
      shot->particle.set_mass(0.1f);
      shot->particle.set_velocity(0, 0, 100.f);
      shot->particle.set_acceleration(0, 0.0f,
                                      0); // no gravity
      shot->particle.set_damping(0.99f);
      break;
    }
    shot->particle.set_position(0.0f, 1.5f, 0.0f);
    shot->stype = current_stype;
    // set timing
    shot->start_time = TimeData::get().last_frame_timestamp;

    // clear accumulated force
    shot->particle.clear_accumulator();
  }

public:
  BallisticDemo() : current_stype(LASER) {
    for (unsigned int i = 0; i < max_ammo; i++) {
      ammo[i].stype = UNUSED;
    }
  }
  std::string get_title() const override {
    return "Ballistic Demo Application";
  }
  void update() override {
    float duration = static_cast<float>(
        TimeData::get().last_frame_duration * 0.001f);
    if (duration <= 0.0f)
      return;

    // update physics tick for each particle
    for (unsigned int i = 0; i < max_ammo; i++) {
      auto shot = ammo[i];
      if (shot.stype != UNUSED) {
        // run the physics
        shot.particle.integrate(duration);

        // check if the shot is still visible
        // not on screen
        bool cond1 = shot.get_position().y() < 0.0f;

        // takes too long
        bool cond2 = shot.start_time + 5000 <
                     TimeData::get().last_frame_timestamp;

        // beyond visible range
        bool cond3 = shot.get_position().z() > 200.f;
        if (cond1 || cond2 || cond3) {
          shot.stype = UNUSED;
        }
      }
    }
    DemoGlutApp.update();
  }
  void display() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glLookAt(-25.0, 8.0, 5.0, 0.0, 5.0, 22, 0, 1, 0);

    // draw a sphere
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0, 1.5f, 0.0f);
    glutSolidSphere(0.1f, 5, 5);
    glTranslatef(0.0, -1.5, 0.0);
    glColor3f(0.75, 0.75, 0.75);
    glScalef(1.0, 1, 1);
    glutSolidSphere(0.1f, 5, 5);
    glPopMatrix();

    // draw scaling lines
    glColor3f(0.75, 0.75, 0.75);
    glBegin(GL_LINES);
    for (unsigned int i = 0; i < 200; i += 10) {
      glVertex3f(-5.0f, 0, i);
      glVertex3f(5.0f, 0, i);
    }
    glEnd();
    for (unsigned int i = 0; i < max_ammo; i++) {
      auto shot = ammo[i];
      if (shot.stype != UNUSED) {
        // run the physics
        shot.render();
      }
    }
    //
    glColor3f(0, 0, 0);
    render_text(
        10, 34,
        std::string("Click: Fire\n1-4: Select Ammo"));
    switch (current_stype) {
    case PISTOL:
      render_text(10, 10,
                  std::string("Current Ammo: Pistol"));
      break;
    case ARTILLERY:
      render_text(10, 10,
                  std::string("Current Ammo: Artillery"));

      break;
    case FIREBALL:
      render_text(10, 10,
                  std::string("Current Ammo: Fireball"));

      break;
    case LASER:

      render_text(10, 10,
                  std::string("Current Ammo: Laser"));

      break;
    }
  }
  void mouse(int btn, int state, int x, int y) override {
    if (state == GLUT_DOWN) {
      fire();
    }
  }
  void key(unsigned char k) override {
    switch (k) {
    case '1':
      current_stype = PISTOL;
      break;
    case '2':
      current_stype = ARTILLERY;
      break;
    case '3':
      current_stype = FIREBALL;
      break;
    case '4':
      current_stype = LASER;
      break;
    }
  }
};
};

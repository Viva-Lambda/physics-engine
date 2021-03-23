#pragma once
// time related utility functions
#include <external.hpp>

namespace vivademos {

struct TimeData {
  /** current frame number which increments constantly*/
  unsigned int frame_nb;

  /** timestamp of the last frame*/
  unsigned int last_frame_timestamp;

  /** duration of the last frame*/
  unsigned int last_frame_duration;

  unsigned long last_frame_clockstamp;
  unsigned long last_frame_clockticks;
  bool is_paused;

  // values to be computed
  double average_frame_duration;

  float fps;

  static TimeData &get();

  void update();
  static void init();
  static void deinit();
  /**get system time*/
  static unsigned get_time() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
  }
  static unsigned get_clock() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
  }

private:
  TimeData();
  TimeData(const TimeData &t);
  TimeData &operator=(const TimeData &t);
};

/**global time frame*/
static TimeData *time_data = nullptr;

TimeData &TimeData::get() { return (TimeData &)*time_data; }

void TimeData::update() {
  if (!time_data) {
    return;
  }

  if (time_data->is_paused) {
    time_data->frame_nb++;
  }
  // update timing
  unsigned now = TimeData::get_time();
  time_data->last_frame_duration =
      now - time_data->last_frame_timestamp;
  time_data->last_frame_timestamp = now;

  // update tick
  unsigned long clock = TimeData::get_clock();
  time_data->last_frame_clockticks =
      clock - time_data->last_frame_clockstamp;
  time_data->last_frame_clockstamp = clock;

  // update average frame rate if we can
  if (time_data->frame_nb > 1) {
    if (time_data->average_frame_duration <= 0) {
      time_data->average_frame_duration =
          static_cast<double>(
              time_data->last_frame_duration);
    } else {
      //
      time_data->average_frame_duration *= 0.99;
      time_data->average_frame_duration +=
          0.01 * static_cast<double>(
                     time_data->last_frame_duration);
      time_data->fps = static_cast<float>(
          1000.0f / time_data->average_frame_duration);
    }
  }
}
void TimeData::init() {
  if (!time_data)
    time_data = new TimeData();
  time_data->frame_nb = 0;
  time_data->last_frame_timestamp = TimeData::get_time();
  time_data->last_frame_duration = 0;
  time_data->last_frame_clockstamp = TimeData::get_clock();
  time_data->last_frame_clockticks = 0;
  time_data->is_paused = false;
  time_data->average_frame_duration = 0;
  time_data->fps = 0;
}
void TimeData::deinit() {
  delete time_data;
  time_data = nullptr;
}
};

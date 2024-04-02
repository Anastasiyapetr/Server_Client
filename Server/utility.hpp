#pragma once

#include <cstdint>

#define SOCKET_PATH "/tmp/my_socket"
#define MAX_BACKLOG 5
#define BUFFER_SIZE 1024
#define CHANNELS_COUNT 4

enum class clientStatus : std::uint8_t {
  connect = 0,
  work = 1,
  disconnect = 2
};

enum class channelState : std::uint8_t {
  error_state = 0,
  idle_state = 1,
  measure_state = 2,
  busy_state = 3,
};

enum class RangeMode { range0, range1, range2, range3 };

struct Range {
public:
  Range(RangeMode mode = RangeMode::range3) : _mode(mode) {
    switch (mode) {
    case RangeMode::range0:
      _min = 1e-7;
      _max = 1e-3;
      break;
    case RangeMode::range1:
      _min = 1e-3;
      _max = 1.0;
      break;
    case RangeMode::range2:
      _min = 1.0;
      _max = 1000.0;
      break;
    case RangeMode::range3:
      _min = 1000;
      _max = 1e6;
      break;
    }
  }
  RangeMode _mode;
  double _min;
  double _max;
};

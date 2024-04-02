#include "HandlerCommand.hpp"
#include "../../SERVER/include/Server.hpp"

void Command::AddParameter(const std::string &first,
                           const std::string &second) {
  _params = std::make_pair(first, second);
}

std::string Command::getName() { return _name; }

void Command::setRange(Range ran, std::string &channel) {
  Server::channelArray[channel].range = ran;
}

Range Command::getRange(std::string &channel) {
  return Server::channelArray[channel].range;
}

std::string StartMeasure::Execute(channelState *state) {
  if (*state == channelState::idle_state) {
    std::cout << "///////Command Start Measure on " << _params.first
              << std::endl;
    *state = channelState::measure_state;
    return "ok";
  } else {
    return "fail";
  }
}

std::string SetRange::Execute(channelState *state) {
  if (*state == channelState::idle_state ||
      *state == channelState::measure_state) {
    std::cout << "///////Command Set Range///////" << std::endl;
    int mode;
    std::from_chars(_params.second.data() + std::string("range").size(),
                    _params.second.data() + std::string("range").size() + 1,
                    mode);
    Range ran = Range(static_cast<RangeMode>(mode));
    setRange(ran, _params.first);
    *state = channelState::idle_state;
    return "ok, " + _params.second;
  } else {
    return "false";
  }
}

std::string StopMeasure::Execute(channelState *state) {
  if (*state == channelState::idle_state ||
      *state == channelState::measure_state) {
    std::cout << "///////Command Stop Measure on " << _params.first
              << std::endl;
    *state = channelState::idle_state;
    return "ok";
  } else {
    return "fail";
  }
}

std::string GetStatus::Execute(channelState *state) {
  std::cout << "///////Command Get Status///////////////" << std::endl;
  std::string result{};

  if (*state == channelState::error_state) {
    result = "error_state";
  } else if (*state == channelState::busy_state) {
    result = "busy_state";
  } else if (*state == channelState::idle_state) {
    result = "idle_state";
  } else if (*state == channelState::measure_state) {
    result = "measure_state";
  }
  return "ok, " + result;
}

std::string GetResult::Execute(channelState *state) {
  if (*state == channelState::measure_state) {
    std::cout << "///////Command Get Result on " << _params.first << std::endl;
    Range _currentRange = getRange(_params.first);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(_currentRange._min,
                                          _currentRange._max);

    double randomNumber = distr(gen);
    return "ok, " + std::to_string(randomNumber);
  } else {
    return "fail";
  }
}
#pragma once

#include "../../utility.hpp"
#include <charconv>
#include <iostream>
#include <random>
#include <string>

class Command {
public:
  Command(const std::string &name) : _name(name){};
  virtual std::string Execute(channelState *state) = 0;
  virtual ~Command(){};
  void AddParameter(const std::string &first, const std::string &second = "");
  std::string getName();
  static void setRange(Range ran, std::string &channel);
  static Range getRange(std::string &channel);

protected:
  std::string _name;
  std::pair<std::string, std::string> _params;
};

class StartMeasure : public Command {
public:
  StartMeasure() : Command("start_measure"){};
  std::string Execute(channelState *state) override;
};

class SetRange : public Command {
public:
  SetRange() : Command("set_range"){};
  std::string Execute(channelState *state) override;
};

class StopMeasure : public Command {
public:
  StopMeasure() : Command("stop_measure"){};
  std::string Execute(channelState *state) override;
};

class GetStatus : public Command {
public:
  GetStatus() : Command("get_status"){};
  std::string Execute(channelState *state) override;
};

class GetResult : public Command {
public:
  GetResult() : Command("get_result"){};
  std::string Execute(channelState *state) override;
};
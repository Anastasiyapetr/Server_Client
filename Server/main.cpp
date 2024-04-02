#include "Server.hpp"

int main() {
  Server server;

  std::vector<std::unique_ptr<Command>> Commands;
  Commands.push_back(std::make_unique<StartMeasure>());
  Commands.push_back(std::make_unique<SetRange>());
  Commands.push_back(std::make_unique<StopMeasure>());
  Commands.push_back(std::make_unique<GetStatus>());
  Commands.push_back(std::make_unique<GetResult>());

  for (size_t num = 1; num < CHANNELS_COUNT; ++num) {
    std::string name = "channel" + std::to_string(num);
    Server::channelArray[name] = ConfigChannel();
  }

  Server::AddCommand(std::move(Commands));
  server.Start();

  return 0;
}

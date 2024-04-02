#pragma once

#include "HandlerCommand.hpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>

struct ConfigChannel {
  ConfigChannel() : range(), state(channelState::idle_state){};
  Range range;
  channelState state;
};

class Server {
private:
  struct Client {
    Client(int socket)
        : _clientSocket(socket), _currentStatus(clientStatus::connect){};
    int _clientSocket;
    std::thread clientThread;
    std::string _currentCommand;
    clientStatus _currentStatus;
    std::string _currentChannel;
    std::mutex clientMutex;

    void InitializeClientThread();
    void HandleClient();
    std::string ParsingChannel();
    void SplitByChar(std::vector<std::string> &bufVec, const std::string &str,
                     const char &sep);
    std::string ParsingCommand();
    void GenerateBusyState();
  };

public:
  enum class status : uint8_t {
    ok = 0,
    err_socket_init = 1,
    err_socket_bind = 2,
    err_socket_listening = 3,
    close = 4
  };

  Server();
  ~Server();
  void Start();
  Server::status CreateSocket();
  Server::status BindSocket();
  Server::status ListenSocket();
  static void AddCommand(std::vector<std::unique_ptr<Command>> &&Commands) {
    for (auto &cmd : Commands) {
      Server::_commands.push_back(std::move(cmd));
    }
  }
  static inline std::vector<std::unique_ptr<Command>> _commands{};
  static inline std::map<std::string, ConfigChannel> channelArray{};

private:
  void AcceptLoop();
  void HandleLoop();
  void JoinThreads();
  void GenerateErrorState();

  status _status;
  int serverSocket;
  std::map<int, std::unique_ptr<Client>> clientList;

  std::thread acceptThread;
  std::thread handleThread;
  std::mutex serverMutex;
};
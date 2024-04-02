#include "Server.hpp"

Server::Server() : _status(status::close), serverSocket(-1) {
  srand(time(nullptr));
};

Server::~Server() {
  JoinThreads();
  _status = status::close;
  if (serverSocket != -1) {
    close(serverSocket);
  }
}

void Server::Start() {
  if (CreateSocket() == status::ok) {
    if (BindSocket() == status::ok) {
      if (ListenSocket() == status::ok) {
        std::cout << "Server is listening for connections..." << std::endl;
        _status = status::ok;
        acceptThread = std::thread([this]() { AcceptLoop(); });
        handleThread = std::thread([this]() { HandleLoop(); });
      }
    }
  }
}

Server::status Server::CreateSocket() {
  serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Failed to create socket" << std::endl;
    return status::err_socket_init;
  }
  return status::ok;
}

Server::status Server::BindSocket() {
  sockaddr_un address;
  memset(&address, 0, sizeof(struct sockaddr_un));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);
  unlink(SOCKET_PATH);
  if (bind(serverSocket, (struct sockaddr *)&address,
           sizeof(struct sockaddr_un)) == -1) {
    std::cerr << "Failed to bind socket" << std::endl;
    return status::err_socket_bind;
  }
  return status::ok;
}

Server::status Server::ListenSocket() {
  if (listen(serverSocket, MAX_BACKLOG) == -1) {
    std::cerr << "Failed to listen for connections" << std::endl;
    return status::err_socket_listening;
  }
  return status::ok;
}

void Server::AcceptLoop() {
  while (_status == status::ok) {
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
      std::cerr << "Failed to accept connection" << std::endl;
      continue;
    }
    serverMutex.lock();
    clientList.emplace(clientSocket, std::make_unique<Client>(clientSocket));
    serverMutex.unlock();
    std::cout << "accepted new client" << std::endl;
  }
}

void Server::HandleLoop() {
  while (_status == status::ok) {
    serverMutex.lock();
    for (auto &cl : clientList) {
      GenerateErrorState();
      if (cl.second) {
        if (cl.second->_currentStatus == clientStatus::disconnect) {
          if (cl.second->clientThread.joinable()) {
            cl.second->clientThread.join();
          }
          clientList.erase(cl.first);
          break;
        }
        if (cl.second->_currentStatus == clientStatus::connect) {
          cl.second->_currentStatus = clientStatus::work;
          cl.second->clientMutex.lock();
          cl.second->InitializeClientThread();
          cl.second->clientMutex.unlock();
        }
      }
    }
    serverMutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void Server::GenerateErrorState() {
  int randomNumber = rand() % 100;
  int errorProbability = 30;
  if (randomNumber <= errorProbability) {
    for (auto &ch : channelArray) {
      ch.second.state = channelState::error_state;
    }
  } else {
    for (auto &ch : channelArray) {
      if (ch.second.state == channelState::error_state) {
        ch.second.state = channelState::idle_state;
      }
    }
  }
}
void Server::JoinThreads() {
  if (acceptThread.joinable()) {
    acceptThread.join();
  }

  if (handleThread.joinable()) {
    handleThread.join();
  }
}

void Server::Client::InitializeClientThread() {
  this->clientThread = std::thread([this]() { HandleClient(); });
}

void Server::Client::HandleClient() {

  ssize_t bytesRead;
  while (_currentStatus == clientStatus::work) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    bytesRead = recv(_clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
      std::cerr << "Error receiving data from client" << std::endl;
      _currentStatus = clientStatus::disconnect;
    } else if (bytesRead == 0) {
      std::cerr << "Client disconnected" << std::endl;
      _currentStatus = clientStatus::disconnect;
    } else {
      std::string receiveCommand(buffer, bytesRead);
      std::vector<std::string> cmds;
      SplitByChar(cmds, receiveCommand, '\n');
      for (auto &cmd : cmds) {
        std::cout << "Received message from client: " << cmd << std::endl;
        _currentCommand = cmd;
        _currentChannel = ParsingChannel();
        std::string answer = ParsingCommand();
        GenerateBusyState();
        answer.append("\n");
        send(_clientSocket, answer.c_str(), answer.size(), 0);
      }
    }
  }
  close(_clientSocket);
}

void Server::Client::GenerateBusyState() {
  int randomNumber = rand() % 100;
  int errorProbability = 10;
  if (randomNumber <= errorProbability) {
    Server::channelArray[_currentChannel].state = channelState::busy_state;
  }
}

std::string Server::Client::ParsingChannel() {
  const std::string start = "channel";
  auto PosStart = _currentCommand.find(start);
  std::string ChannelNum;
  for (; PosStart < _currentCommand.size(); ++PosStart) {
    if (_currentCommand[PosStart] == ' ')
      break;
    ChannelNum += _currentCommand[PosStart];
  }
  return ChannelNum;
}

void Server::Client::SplitByChar(std::vector<std::string> &bufVec,
                                 const std::string &str, const char &sep) {
  std::stringstream ss(str);
  std::string token;
  bufVec.clear();
  while (std::getline(ss, token, sep)) {
    bufVec.push_back(token);
  }
}

std::string Server::Client::ParsingCommand() {
  std::string range{};
  std::string answer{};
  auto spacePos = _currentCommand.find(" ");
  auto commaPos = _currentCommand.find(",");
  if (commaPos == std::string::npos) {
    commaPos = _currentCommand.size();
  } else {
    auto endPos = _currentCommand.size() - 1;
    range = _currentCommand.substr(commaPos + 2, endPos - commaPos - 1);
  }
  std::string name = _currentCommand.substr(0, spacePos);
  std::string channel =
      _currentCommand.substr(spacePos + 1, commaPos - spacePos - 1);
  for (auto &cmd : Server::_commands) {
    if (cmd->getName() == name) {
      cmd->AddParameter(channel, range);
      channelState *_currentState =
          &(Server::channelArray[_currentChannel].state);
      answer = cmd->Execute(_currentState);
      break;
    }
  }
  return answer;
}

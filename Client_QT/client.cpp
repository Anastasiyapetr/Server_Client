#include "client.h"

Client::Client(QObject *parent)
    : QObject(parent), socket(new QLocalSocket(this)) {
  connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
  connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
  connect(socket, SIGNAL(disconnected()), SLOT(slotDisconnected()));
  connect(&interface, &MultimeterInterface::sendMessageClicked, this,
          &Client::sendMessage);
  connect(&interface, &MultimeterInterface::disconnectFromServer, this,
          &Client::disconnectFromServer);
  interface.show();
}

Client::~Client() {
  socket->close();
  delete socket;
}

void Client::connectToServer() {
  socket->connectToServer(SOCKET_PATH);
  if (!socket->waitForConnected()) {
    qDebug() << "Error: " << socket->errorString();
  }
}

void Client::sendMessage(const QString &message) {
  if (socket->state() == QLocalSocket::ConnectedState) {
    qDebug() << "Send message to server " << message;
    socket->write(message.toUtf8());
  } else {
    static int attempt = 0;
    attempt++;
    if (attempt >= 5) {
      qDebug() << "Error: Not connected to server.";
      attempt = 0;
      return;
    }
    connectToServer();
    sendMessage(message);
  }
}

void Client::slotReadyRead() {
  QByteArray responseData = socket->readAll();
  QString answer = responseData;
  std::vector<QString> array;
  SplitByChar(array, answer, QChar('\n'));
  for (auto &el : array) {
    qDebug() << "Response from server:" << el;
    if (el.contains("error_state")) {
      socket->disconnectFromServer();
    }
    interface.ReceiveAnswer(el);
  }
}

void Client::SplitByChar(std::vector<QString> &result, QString &str,
                         QChar sep) {
  QString current;
  for (const QChar &ch : str) {
    if (ch == sep) {
      result.push_back(current);
      current.clear();
    } else {
      current += ch;
    }
  }
  if (!current.isEmpty()) {
    result.push_back(current);
  }
}

void Client::disconnectFromServer() { socket->disconnectFromServer(); }

void Client::slotConnected() {
  qDebug() << "Success connection to server";
  interface.changeColorIndicate("background-color: green;");
}

void Client::slotDisconnected() {
  qDebug() << "Disconnected from server";
  interface.changeColorIndicate("background-color: red;");
}

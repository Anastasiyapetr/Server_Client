#ifndef CLIENT_H
#define CLIENT_H

#include "multimeterinterface.h"
#include <QDebug>
#include <QLocalSocket>
#include <QObject>
#include <QWidget>

#define SOCKET_PATH "/tmp/my_socket"

class Client : public QObject {
  Q_OBJECT

public:
  Client(QObject *parent = nullptr);
  ~Client();
  void connectToServer();
  void sendMessage(const QString &message);
  void disconnectFromServer();

private slots:
  void slotReadyRead();
  void slotConnected();
  void slotDisconnected();

private:
  QLocalSocket *socket;
  MultimeterInterface interface;
  void SplitByChar(std::vector<QString> &result, QString &str, QChar sep);
};
#endif // CLIENT_H

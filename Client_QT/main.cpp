#include "client.h"
#include <QApplication>
#include <chrono>
#include <thread>
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Client client;
  return a.exec();
}

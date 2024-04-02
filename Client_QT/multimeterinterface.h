#ifndef MULTIMETERINTERFACE_H
#define MULTIMETERINTERFACE_H

#include <QAbstractButton>
#include <QComboBox>
#include <QMainWindow>
#include <QPushButton>
#include <map>

#define CHANNELS_COUNT 4
#define RANGES_COUNT 3

QT_BEGIN_NAMESPACE
namespace Ui {
class MultimeterInterface;
}
QT_END_NAMESPACE

class MultimeterInterface : public QMainWindow {
  Q_OBJECT
public:
  MultimeterInterface(QWidget *parent = nullptr);
  ~MultimeterInterface();
  void ReceiveAnswer(QString &answer);
  void changeColorIndicate(const QString &color);
signals:
  void sendMessageClicked(const QString &message);
  void disconnectFromServer();
public slots:
  void ButtonClicked(QAbstractButton *btn);
  void SendData();
  void ClearData();

private:
  void DefaultSettings();
  void setButtons();
  void setComboBox(QComboBox *Combobox, const size_t from, const size_t to,
                   const QString &name);
  void ChangeButtonColor(QAbstractButton *btn);
  void SetButtonColor(QAbstractButton *btn, const QColor color);

  std::map<QString, QPushButton *> cmd;
  QFrame *indicator;
  Ui::MultimeterInterface *ui;
  QComboBox *channels;
  QComboBox *ranges;
  QString result;
  QString currentCommand;
};
#endif // MULTIMETERINTERFACE_H

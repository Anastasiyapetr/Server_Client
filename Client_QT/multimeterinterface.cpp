#include "multimeterinterface.h"
#include "./ui_multimeterinterface.h"

MultimeterInterface::MultimeterInterface(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MultimeterInterface) {
  ui->setupUi(this);
  connect(ui->Commands, &QButtonGroup::buttonClicked, this,
          &::MultimeterInterface::ButtonClicked);
  connect(ui->send, &QPushButton::clicked, this,
          &::MultimeterInterface::SendData);
  connect(ui->clear, &QPushButton::clicked, this,
          &::MultimeterInterface::ClearData);
  DefaultSettings();
}

MultimeterInterface::~MultimeterInterface() {
  delete channels;
  delete ranges;
  delete ui;
  delete indicator;
}

void MultimeterInterface::DefaultSettings() {
  setButtons();

  channels = new QComboBox(this);
  channels->setGeometry(400, 200, 193, 40);
  setComboBox(channels, 1, CHANNELS_COUNT, "channel ");

  ranges = new QComboBox(this);
  ranges->setGeometry(400, 360, 193, 40);
  setComboBox(ranges, 0, RANGES_COUNT, "range ");

  indicator = new QFrame(this);
  indicator->setGeometry(380, 435, 10, 10);
  setAutoFillBackground(true);
  indicator->setStyleSheet("background-color: red;");
}

void MultimeterInterface::changeColorIndicate(const QString &color) {
  indicator->setStyleSheet(color);
}

void MultimeterInterface::setButtons() {
  cmd.emplace("start_measure", ui->start_measure);
  cmd.emplace("set_range", ui->set_range);
  cmd.emplace("stop_measure", ui->stop_measure);
  cmd.emplace("get_status", ui->get_status);
  cmd.emplace("get_result", ui->get_result);
}

void MultimeterInterface::setComboBox(QComboBox *Combobox, const size_t from,
                                      const size_t to, const QString &name) {
  QStringList items;
  items << "(select)";
  for (size_t i = from; i <= to; ++i) {
    QString tmp{name};
    std::string num = std::to_string(i);
    tmp.append(num.data());
    items << tmp;
  }
  Combobox->addItems(items);
}

void MultimeterInterface::ButtonClicked(QAbstractButton *btn) {
  ChangeButtonColor(btn);
  for (auto &button : cmd) {
    if (button.second == btn) {
      currentCommand = button.first;
      break;
    }
  }
};

void MultimeterInterface::ChangeButtonColor(QAbstractButton *btn) {
  QPalette pal;
  for (auto &button : cmd) {
    if (button.second == btn) {
      continue;
    }
    pal = button.second->palette();
    QColor activeColor = pal.color(QPalette::Active, QPalette::Button);
    if (activeColor == Qt::gray) {
      SetButtonColor(button.second, Qt::white);
    }
  }
  if (btn != nullptr)
    SetButtonColor(btn, Qt::gray);
}

void MultimeterInterface::SetButtonColor(QAbstractButton *btn,
                                         const QColor color) {
  QPalette pallete;
  pallete.setColor(QPalette::Button, color);
  btn->setAutoFillBackground(true);
  btn->setPalette(pallete);
  btn->update();
}

void MultimeterInterface::ClearData() {
  ChangeButtonColor(nullptr);
  channels->setCurrentIndex(0);
  ranges->setCurrentIndex(0);
  ui->result->clear();
  emit disconnectFromServer();
}

void MultimeterInterface::SendData() {
  QString tmp, chan, ran;
  if (channels->currentIndex() == 0) {
    tmp = "Выберите канал";
  } else if (ranges->currentIndex() == 0 && currentCommand == "set_range") {
    tmp = "Выберите диапазон канала измерения";
  } else {
    ui->result->clear();

    chan = channels->currentText();
    auto space = chan.indexOf(" ");
    chan.erase(chan.begin() + space, chan.begin() + space + 1);

    if (currentCommand == "set_range") {
      ran = ranges->currentText();
      space = ran.indexOf(" ");
      ran.erase(ran.begin() + space, ran.begin() + space + 1);
    }
    if (ran.size() != 0) {
      result = currentCommand + " " + chan + ", " + ran + "\n";
    } else {
      result = currentCommand + " " + chan + "\n";
    }
    emit sendMessageClicked(result);
  }
}

void MultimeterInterface::ReceiveAnswer(QString &answer) {
  ui->result->clear();
  answer.prepend("Результат: ");
  ui->result->setText(answer);
}

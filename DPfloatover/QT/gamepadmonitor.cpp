#include "gamepadmonitor.h"
#include <QDebug>
#include <QtGamepad/QGamepad>

GamepadMonitor::GamepadMonitor(QObject *parent)
    : QObject(parent),
      m_gamepad(0),
      button_up(false),
      button_down(false),
      button_left(false),
      button_right(false),
      button_L1(false),
      button_R1(false),
      gamepad_xforce(0),
      gamepad_yforce(0),
      gamepad_zmoment(0) {}

void GamepadMonitor::initializeGamepad() {
  auto gamepads = QGamepadManager::instance()->connectedGamepads();
  if (gamepads.isEmpty()) {
    return;
  }

  m_gamepad = new QGamepad(*gamepads.begin(), this);

  // ouput id of each device
  int device = m_gamepad->deviceId();
  qDebug() << device;

  connect(m_gamepad, &QGamepad::buttonLeftChanged, this,
          [=](bool pressed) { button_left = pressed; });

  connect(m_gamepad, &QGamepad::buttonRightChanged, this,
          [=](bool pressed) { button_right = pressed; });

  connect(m_gamepad, &QGamepad::buttonUpChanged, this,
          [=](bool pressed) { button_up = pressed; });

  connect(m_gamepad, &QGamepad::buttonDownChanged, this,
          [=](bool pressed) { button_down = pressed; });

  // connect(m_gamepad, &QGamepad::axisLeftXChanged, this,
  //         [](double value) { qDebug() << "Left X" << value; });
  // connect(m_gamepad, &QGamepad::axisLeftYChanged, this,
  //         [](double value) { qDebug() << "Left Y" << value; });

  //    connect(m_gamepad, &QGamepad::axisRightXChanged, this, [](double value){
  //        qDebug() << "Right X" << value;
  //    });
  //    connect(m_gamepad, &QGamepad::axisRightYChanged, this, [](double value){
  //        qDebug() << "Right Y" << value;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonAChanged, this, [](bool pressed){
  //        qDebug() << "Button A" << pressed;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonBChanged, this, [](bool pressed){
  //        qDebug() << "Button B" << pressed;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonXChanged, this, [](bool pressed){
  //        qDebug() << "Button X" << pressed;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonYChanged, this, [](bool pressed){
  //        qDebug() << "Button Y" << pressed;
  //    });
  connect(m_gamepad, &QGamepad::buttonL1Changed, this,
          [=](bool pressed) { button_L1 = pressed; });
  connect(m_gamepad, &QGamepad::buttonR1Changed, this,
          [=](bool pressed) { button_R1 = pressed; });
  //    connect(m_gamepad, &QGamepad::buttonL2Changed, this, [](double value){
  //        qDebug() << "Button L2: " << value;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonR2Changed, this, [](double value){
  //        qDebug() << "Button R2: " << value;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonSelectChanged, this, [](bool
  //    pressed){
  //        qDebug() << "Button Select" << pressed;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonStartChanged, this, [](bool
  //    pressed){
  //        qDebug() << "Button Start" << pressed;
  //    });
  //    connect(m_gamepad, &QGamepad::buttonGuideChanged, this, [](bool
  //    pressed){
  //        qDebug() << "Button Guide" << pressed;
  //    });
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(computegamepadforce()));
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(ouputpressedinfo()));
  // Interval 0 means to refresh as fast as possible
  dataTimer.start(VIEWERREFRESH);
}

int GamepadMonitor::computegamepadxforce() {
  int temp_positiveforce = 0;
  int temp_negativeforce = 0;
  if (button_up) temp_positiveforce = 1;
  if (button_down) temp_negativeforce = -1;
  return temp_negativeforce + temp_positiveforce;
}

int GamepadMonitor::computegamepadyforce() {
  int temp_positiveforce = 0;
  int temp_negativeforce = 0;
  if (button_right) temp_positiveforce = 1;
  if (button_left) temp_negativeforce = -1;
  return temp_negativeforce + temp_positiveforce;
}

int GamepadMonitor::computegamepadzmoment() {
  int temp_positiveforce = 0;
  int temp_negativeforce = 0;
  if (button_R1) temp_positiveforce = 1;
  if (button_L1) temp_negativeforce = -1;
  return temp_negativeforce + temp_positiveforce;
}

void GamepadMonitor::ouputpressedinfo() {
  qDebug() << "X force" << gamepad_xforce;
  qDebug() << "Y force" << gamepad_yforce;
  qDebug() << "Z moment" << gamepad_zmoment;
}

void GamepadMonitor::computegamepadforce() {
  gamepad_xforce = computegamepadxforce();
  gamepad_yforce = computegamepadyforce();
  gamepad_zmoment = computegamepadzmoment();
}

bool GamepadMonitor::getButtonDown() const { return button_down; }
bool GamepadMonitor::getButtonL1() const { return button_L1; }
bool GamepadMonitor::getButtonLeft() const { return button_left; }
bool GamepadMonitor::getButtonR1() const { return button_R1; }
bool GamepadMonitor::getButtonRight() const { return button_right; }
bool GamepadMonitor::getButtonUp() const { return button_up; }
int GamepadMonitor::getGamepadXforce() const { return gamepad_xforce; }
int GamepadMonitor::getGamepadYforce() const { return gamepad_yforce; }
int GamepadMonitor::getGamepadZmoment() const { return gamepad_zmoment; }
GamepadMonitor::~GamepadMonitor() { delete m_gamepad; }

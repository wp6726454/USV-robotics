/*
***********************************************************************
* gamepadmonitor.h:
* header file to recieve signal event from gamepad controller
* autonomous system.
*
*  by Hu.ZH(Mr. SJTU)
***********************************************************************
*/

#ifndef GAMEPADMONITOR_H
#define GAMEPADMONITOR_H

#include "constants.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE
class QGamepad;
QT_END_NAMESPACE

class GamepadMonitor : public QObject {
  Q_OBJECT
public:
  explicit GamepadMonitor(QObject *parent = nullptr);
  ~GamepadMonitor();
  bool getButtonUp() const;
  bool getButtonDown() const;
  bool getButtonLeft() const;
  bool getButtonRight() const;
  bool getButtonL1() const;
  bool getButtonR1() const;
  int getGamepadXforce() const;
  int getGamepadYforce() const;
  int getGamepadZmoment() const;
  void initializeGamepad();

private:
  QGamepad *m_gamepad;
  QTimer dataTimer;
  bool button_up;
  bool button_down;
  bool button_left;
  bool button_right;
  bool button_L1;
  bool button_R1;
  int gamepad_xforce;
  int gamepad_yforce;
  int gamepad_zmoment;

  int computegamepadxforce();
  int computegamepadyforce();
  int computegamepadzmoment();

private slots:
  void ouputpressedinfo();
  void computegamepadforce();
};

#endif // GAMEPADMONITOR_H

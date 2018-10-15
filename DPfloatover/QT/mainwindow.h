#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <fstream>
#include "display2ddialog.h"
#include "globalvar.h"
#include "thrusterdiag.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_action2D_triggered();

  void on_actionThruster_P_triggered();

  void on_PB_start_clicked();

  void on_PB_connection_clicked();

  void on_PB_test_clicked();

  void on_PB_suspend_clicked();

  void readfilebyline();

 private:
  Ui::MainWindow *ui;
  Display2DDialog *myDisplay2DDialog;
  ThrusterDiag *myThrusterDiag;
  QStringListModel *_model;
  Qt::WindowFlags flags = nullptr;

  void GetCurrentPath();
  void initializeLoglist();
  void updatelog();
};

#endif  // MAINWINDOW_H

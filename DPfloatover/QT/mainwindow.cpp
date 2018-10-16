#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->setWindowState(Qt::WindowMaximized);
  GetCurrentPath();
  initializeLoglist();
  updatelog();
  //  this->setStyleSheet("background-color: black;");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_action2D_triggered() {
  //  Display2DDialog mDialog;
  //  mDialog.setModal(true);
  //  mDialog.exec();
  myDisplay2DDialog = new Display2DDialog(this);
  myDisplay2DDialog->show();
}

void MainWindow::on_actionThruster_P_triggered() {
  myThrusterDiag = new ThrusterDiag(this);
  myThrusterDiag->show();
}

void MainWindow::on_PB_connection_clicked() {
  //  globalvar::_loop.initializelooop();
  //  globalvar::_loop.start_connnection_t();
  globalvar::_threadloop.initializelooop();
  globalvar::_threadloop.start_connnection_t();
  globalvar::_threadloop.updategamepad_t();
}

void MainWindow::on_PB_start_clicked() {
  //  globalvar::_loop.receiveallclients_t();

  // start a thread for motion caputre
  globalvar::_threadloop.updatemotioncapture_t();
  // start multithreads for each socket client
  globalvar::_threadloop.controller_t();
  // start a thread for send/receive using Profinet
  globalvar::_threadloop.send2allclients_pn_t();
  // start a thread for sqlite database
  globalvar::_threadloop.save2database_t();
}

void MainWindow::on_PB_test_clicked() {
  std::thread t1(updatetest, std::ref(globalvar::_dataviewer));
  t1.detach();
}

void MainWindow::on_PB_suspend_clicked() {
  // globalvar::_loop.closelooop();
  globalvar::_threadloop.closelooop();
}

void MainWindow::GetCurrentPath() {
  char *PWD;
  PWD = getenv("PWD");
  globalvar::pwd.append(PWD);
}

void MainWindow::initializeLoglist() {
  // create model
  _model = new QStringListModel(this);
  // Make data
  QStringList loglist;
  loglist << "Program started!";
  // populate our model
  _model->setStringList(loglist);
  // Glue model and view together
  ui->listView_log->setModel(_model);
}

void MainWindow::readfilebyline() {
  //  QString _path = globalvar::pwd + "/data/log";
  QString _path = QString::fromUtf8(logsavepath.c_str());
  QFile inputfile(_path);
  if (inputfile.open(QIODevice::ReadOnly)) {
    QTextStream in(&inputfile);
    QStringList loglist;
    while (!in.atEnd()) {
      QString line = in.readLine();
      loglist << line;
    }
    _model->setStringList(loglist);
  }
}

void MainWindow::updatelog() {
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(readfilebyline()));
  timer->start(1000);
}

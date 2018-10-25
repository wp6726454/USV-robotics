#ifndef DISPLAY2DDIALOG_H
#define DISPLAY2DDIALOG_H
#include <QDialog>
#include <QTimer>
#include <cmath>
#include <unordered_map>
#include "constants.h"
#include "globalvar.h"
#include "qcustomplot.h"  // the header file of QCustomPlot.
#include "realtimedata.h"

const uint32_t arraylength = 592;  // length of array for 2D display
// length of array for trajectory display
const uint32_t trajectorylength = 2000;
const uint32_t arraylength_6DoF = 296;  // length of array for 6DoF display

namespace Ui {
class Display2DDialog;
}

class Display2DDialog : public QDialog {
  Q_OBJECT

 public:
  explicit Display2DDialog(QWidget *parent = nullptr);
  ~Display2DDialog();

  void setupDemo();
  void setupVesselRealtimeData();

 private slots:
  void motion6DOFdataSlot();
  void simplerealtimeDataSlot();
  void vesselshapeDataSlot();

 private:
  Ui::Display2DDialog *ui;
  QString demoName;
  QTimer dataTimer;
  QCPItemTracer *itemDemoPhaseTracer;

  // data for 2d display
  std::unordered_map<int, QVector<double>> planarmotion_x;
  std::unordered_map<int, QVector<double>> planarmotion_y;
  std::unordered_map<int, QVector<double>> trajectory_x;
  std::unordered_map<int, QVector<double>> trajectory_y;
  // data for 6DoF time series
  std::unordered_map<int, QVector<double>> motion_clients;
  QVector<double> motion6Dof_xaxis_data;
  QVector<double> motion6Dof_yaxis_data;

  std::vector<QCPAxisRect *> dofmotionplot;

  void initializeAllUI();
  void convertvessel(double origin_x, double origin_y, double t_orient,
                     QVector<double> &t_datax, QVector<double> &t_datay);
  void updatetrajectoryvector(double origin_x, double origin_y,
                              QVector<double> &t_trajectory_x,
                              QVector<double> &t_trajectory_y);

  void initialize6DOFmotion(QCustomPlot *customPlot);
  void initializePlanarMotion(QCustomPlot *customPlot);
  void initializePlanarMotionData();
  bool eventFilter(QObject *target, QEvent *event);
};

#endif  // DISPLAY2DDIALOG_H

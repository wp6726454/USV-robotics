/*
***********************************************************************
* thrusterdiag.h:
* header file for real-time display of thruster data information
*
*  by Hu.ZH(Mr. SJTU)
***********************************************************************
*/

#ifndef THRUSTERDIAG_H
#define THRUSTERDIAG_H

#include <QDialog>
#include <unordered_map>
#include "constants.h"
#include "globalvar.h"
#include "qcustomplot.h"  // the header file of QCustomPlot.
#include "realtimedata.h"
// length of array for dashboard background
const uint32_t arraylength_dgsparse = 50;
const uint32_t arraylength_dgdense = 300;
const uint32_t step_angle_text = 30;  // step of angle and text
const uint32_t num_angle_text = 12;   // thus, number of angles and text
const uint32_t num_slider_text = 5;   // number of slider text
const uint32_t num_pointer = 2;
const double dash_radius = 1.2;

namespace Ui {
class ThrusterDiag;
}

class ThrusterDiag : public QDialog {
  Q_OBJECT

 public:
  explicit ThrusterDiag(QWidget *parent = nullptr);
  ~ThrusterDiag();

 private slots:

  void realtimeDashboard();

 private:
  Ui::ThrusterDiag *ui;
  QString demoName;
  QVector<double> dbgsparse_x;  // data for dashboard background
  QVector<double> dbgsparse_y;
  QVector<double> dbgdense_x;
  QVector<double> dbgdense_y;

  QVector<QCPCurveData> dataSpiral_outer;
  QVector<QCPCurveData> dataSpiral_inner;
  QVector<double> text_posx;
  QVector<double> text_posy;

  QTimer dataTimer;

  int bowthrustersize = 1000;
  double qcustomplot_max = 5;

  /* int means the index of each vessel */
  // in the QCustomplot coordinate
  std::unordered_map<int, QVector<double>> thruster_center_X;
  std::unordered_map<int, QVector<double>> thruster_center_Y;
  std::unordered_map<int, QVector<QCPItemText *>> center_texts;
  std::unordered_map<int, QVector<QSlider *>> horizontalslider;
  std::unordered_map<int, QVector<int>> dashtext_factor;
  std::unordered_map<int, QVector<int>> slidertext_factor;
  std::unordered_map<int, QVector<int>> realtime_rudderangle;
  // in the QT global coordinate
  std::unordered_map<int, QVector<int>> slider_position_x;
  std::unordered_map<int, QVector<int>> slider_position_y;
  // in the QCustomplot coordinate
  std::unordered_map<int, QVector<double>> slidertext_position_x;
  std::unordered_map<int, QVector<double>> slidertext_position_y;

  /* int means the index of thruster for each vessel */
  // in the QCustomplot coordinate
  std::unordered_map<int, QVector<double>> realtime_rev2angle;
  std::unordered_map<int, QVector<double>> thruster1_pointer_X;
  std::unordered_map<int, QVector<double>> thruster1_pointer_Y;
  std::unordered_map<int, QVector<double>> thruster2_pointer_X;
  std::unordered_map<int, QVector<double>> thruster2_pointer_Y;
  std::unordered_map<int, QVector<double>> thruster3_pointer_X;
  std::unordered_map<int, QVector<double>> thruster3_pointer_Y;

  QVector<double> tranlation_vessel_x;
  QVector<double> tranlation_vessel_y;

  void setupclockshape(QCustomPlot *customPlot, int vessel_index);

  void initializeAllUI();  // specify the position and size of each widget
  void initializevesselshape(QCustomPlot *customPlot,
                             double translationx /*= 0*/,
                             double translationy /*= -4.7*/);
  void initializedenseTick(QCustomPlot *customPlot, int graph_index);
  void initializesparseTick(QCustomPlot *customPlot, int graph_index);
  void initializePointer(QCustomPlot *customPlot, int graph_index,
                         int index_vessel, int index_thruster);
  void initializeTextDash(QCustomPlot *customPlot, int index_vessel,
                          int index_thruster, double center_x, double center_y,
                          int _index_bow);
  void initializeCircle(QCustomPlot *customPlot, double center_x,
                        double center_y);
  void initializeonethruster(QCustomPlot *customPlot, int vessel_index,
                             int thruster_index, int _index_bow = 0);
  void initializeoneslider(QCustomPlot *customPlot, int vessel_index,
                           int slider_index);
  void initializeoneslidertext(QCustomPlot *customPlot, int vessel_index,
                               int slider_index, double center_x,
                               double center_y);
  // generate the coordinate data for dashboard background
  void generateDashBGDense(double min_radius, double max_radius,
                           unsigned int step_angle, double center_x,
                           double center_y);
  void generateDashBGSparse(double min_radius, double max_radius,
                            unsigned int step_angle, double center_x,
                            double center_y);
  void generateCurveData(double radius_inner, double radius_outer,
                         double center_x, double center_y);
  void generatetext_factor();
  void generatetranslationdata();
  void generatePointerData(int vessel_index, int thruster_index);
  void setuprealtimeRevolution(const Eigen::Vector3i &_rotation);
  void setuprealtimeRevolution(const Eigen::Vector3i &_rotation_first,
                               const Eigen::Vector3i &_rotation_second);
  void setuprealtimeRevolution(const Eigen::Vector3i &_rotation_first,
                               const Eigen::Vector3i &_rotation_second,
                               const Eigen::Vector3i &_rotation_third);
  void setuprealtimerudder(const Eigen::Vector3i &_alpha_deg);
  void setuprealtimerudder(const Eigen::Vector3i &_alpha_deg_first,
                           const Eigen::Vector3i &_alpha_deg_second);
  void setuprealtimerudder(const Eigen::Vector3i &_alpha_deg_first,
                           const Eigen::Vector3i &_alpha_deg_second,
                           const Eigen::Vector3i &_alpha_deg_third);

  // real time convert realtimedata to viewer data
  void convertrealtime2viewer() {}
  // convert polar coordinate to Cartesian coordinates
  void convertPolar2Cartesian(double center_x, double center_y,
                              const QVector<double> &polar_r,
                              const QVector<double> &polar_theta,
                              QVector<double> &Car_x, QVector<double> &Car_y);
  void generatethrusterAndSliderPosition();
  void initializePointerAndSliderData();
};

#endif  // THRUSTERDIAG_H

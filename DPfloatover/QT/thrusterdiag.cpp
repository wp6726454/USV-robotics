#include "thrusterdiag.h"
#include "ui_thrusterdiag.h"

ThrusterDiag::ThrusterDiag(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ThrusterDiag),
      dbgsparse_x(arraylength_dgsparse, 0),
      dbgsparse_y(arraylength_dgsparse, 0),
      dbgdense_x(arraylength_dgdense, 0),
      dbgdense_y(arraylength_dgdense, 0),
      dataSpiral_outer(arraylength_dgsparse),
      dataSpiral_inner(arraylength_dgsparse),
      text_posx(num_angle_text, 0),
      text_posy(num_angle_text, 0) {
  ui->setupUi(this);
  initializeAllUI();
}

ThrusterDiag::~ThrusterDiag() { delete ui; }

void ThrusterDiag::initializeAllUI() {
  // setup all colors used
  QPalette pal_background = palette();
  pal_background.setColor(QPalette::Background, QColor(68, 68, 68, 255));

  // set the whole dialog
  this->setWindowState(Qt::WindowMaximized);
  this->setWindowFlags(Qt::Window);
  this->setAutoFillBackground(true);
  this->setPalette(pal_background);
  this->setStyleSheet("QLabel {color : white; }");  // set color for all Qlabels

  ui->customPlot_thruster->setGeometry(-220, 0, bowthrustersize,
                                       bowthrustersize);
  ui->customPlot_thruster2->setGeometry(550, 0, bowthrustersize,
                                        bowthrustersize);
  ui->customPlot_thruster3->setGeometry(1200, 0, bowthrustersize,
                                        bowthrustersize);

  generatetranslationdata();
  generatetext_factor();
  generatethrusterAndSliderPosition();
  initializePointerAndSliderData();
  if (MAXCONNECTION > 0) setupclockshape(ui->customPlot_thruster, 0);
  if (MAXCONNECTION > 1) setupclockshape(ui->customPlot_thruster2, 1);
  if (MAXCONNECTION > 2) setupclockshape(ui->customPlot_thruster3, 2);

  // setup title of dialog
  demoName = "Vessel Thruster";
  setWindowTitle(demoName);

  // start realtime viewer
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDashboard()));
  // Interval 0 means to refresh as fast as possible
  dataTimer.start(VIEWERREFRESH);
}

void ThrusterDiag::setupclockshape(QCustomPlot *customPlot, int vessel_index) {
  // setup background
  QBrush boxBrush(QColor(255, 255, 255, 255));
  customPlot->setBackground(boxBrush);  // setup color of background
  customPlot->setNotAntialiasedElements(QCP::aeAll);

  // initialize all thruster dashboard
  initializevesselshape(customPlot, tranlation_vessel_x[vessel_index],
                        tranlation_vessel_y[vessel_index]);

  if (vessel_index == 2) {
    for (int i = 0; i != 6; ++i)
      initializeonethruster(customPlot, vessel_index, i);
    for (int i = 0; i != 4; ++i)
      initializeoneslider(customPlot, vessel_index, i);
  } else {
    for (int i = 0; i != 2; ++i)
      initializeonethruster(customPlot, vessel_index, i, 1);  // bow thruster
    for (int i = 2; i != 4; ++i)
      initializeonethruster(customPlot, vessel_index, i);  // azimuth thruster
    for (int i = 0; i != 2; ++i)
      initializeoneslider(customPlot, vessel_index, i);
  }

  // setup x,y axis
  customPlot->xAxis->setVisible(false);
  customPlot->yAxis->setVisible(false);
  customPlot->xAxis->setRange(-qcustomplot_max, qcustomplot_max);
  customPlot->yAxis->setRange(-qcustomplot_max, qcustomplot_max);
  customPlot->yAxis->setScaleRatio(customPlot->xAxis, 1.0);  // axis equal
}

void ThrusterDiag::initializeonethruster(QCustomPlot *customPlot,
                                         int vessel_index, int thruster_index,
                                         int _index_bow) {
  double center_x = thruster_center_X[vessel_index][thruster_index] +
                    tranlation_vessel_x[vessel_index];
  double center_y = thruster_center_Y[vessel_index][thruster_index];

  // generate data
  generateDashBGSparse(0.86 * dash_radius, 0.96 * dash_radius, step_angle_text,
                       center_x, center_y);
  generateDashBGDense(0.90 * dash_radius, 0.96 * dash_radius, 5, center_x,
                      center_y);
  generateCurveData(0.6 * dash_radius, dash_radius, center_x, center_y);
  generatePointerData(vessel_index, thruster_index);

  // setup dense and sparse tick
  initializesparseTick(customPlot, 3 * thruster_index + 1);
  initializedenseTick(customPlot, 3 * thruster_index + 2);
  initializePointer(customPlot, 3 * thruster_index + 3, vessel_index,
                    thruster_index);

  initializeCircle(customPlot, center_x, center_y);
  initializeTextDash(customPlot, vessel_index, thruster_index, center_x,
                     center_y, _index_bow);
}

void ThrusterDiag::generateDashBGSparse(double min_radius, double max_radius,
                                        unsigned int step_angle,
                                        double center_x, double center_y) {
  // ensure that step_angle can be divided by 360
  unsigned totalnum_radius = (unsigned)(arraylength_dgsparse / num_angle_text);
  double radius_step = (max_radius - min_radius) / totalnum_radius;

  // initialize Qvectors
  QVector<double> polar_r(arraylength_dgsparse, 0);
  QVector<double> polar_theta(arraylength_dgsparse, 0);
  QVector<double> text_polarr(num_angle_text, 0.9 * min_radius);
  QVector<double> text_polartheta(num_angle_text, 0);

  // handle the data for dashboard sparse background in the polar coordinate
  for (unsigned i = 0; i != num_angle_text; ++i) {
    text_polartheta[i] = i * step_angle;
    for (unsigned j = 0; j != totalnum_radius; ++j) {
      polar_theta[totalnum_radius * i + j] = step_angle * i;
      polar_r[totalnum_radius * i + j] = radius_step * j + min_radius;
    }
  }

  // convert polar coordinate to Cartesian coordiante
  convertPolar2Cartesian(center_x, center_y, polar_r, polar_theta, dbgsparse_x,
                         dbgsparse_y);
  convertPolar2Cartesian(center_x, center_y, text_polarr, text_polartheta,
                         text_posx, text_posy);
}

void ThrusterDiag::generateDashBGDense(double min_radius, double max_radius,
                                       unsigned int step_angle, double center_x,
                                       double center_y) {
  QVector<double> polar_r(arraylength_dgdense, 0);
  QVector<double> polar_theta(arraylength_dgdense, 0);
  // ensure that step_angle can be divided by 360
  unsigned totalnum_angle = (unsigned)(360 / step_angle);
  unsigned totalnum_radius = (unsigned)(arraylength_dgdense / totalnum_angle);
  double radius_step = (max_radius - min_radius) / totalnum_radius;
  for (unsigned i = 0; i != totalnum_angle; ++i)
    for (unsigned j = 0; j != totalnum_radius; ++j) {
      polar_theta[totalnum_radius * i + j] = step_angle * i;
      polar_r[totalnum_radius * i + j] = radius_step * j + min_radius;
    }
  convertPolar2Cartesian(center_x, center_y, polar_r, polar_theta, dbgdense_x,
                         dbgdense_y);
}

void ThrusterDiag::generateCurveData(double radius_inner, double radius_outer,
                                     double center_x, double center_y) {
  for (unsigned i = 0; i != arraylength_dgsparse; ++i) {
    double theta = i / (double)(arraylength_dgsparse - 1) * 2 * M_PI;
    dataSpiral_outer[i] =
        QCPCurveData(i, center_x + radius_outer * std::cos(theta),
                     center_y + radius_outer * std::sin(theta));
    dataSpiral_inner[i] =
        QCPCurveData(i, center_x + radius_inner * std::cos(theta),
                     center_y + radius_inner * std::sin(theta));
  }
}

void ThrusterDiag::convertPolar2Cartesian(double center_x, double center_y,
                                          const QVector<double> &polar_r,
                                          const QVector<double> &polar_theta,
                                          QVector<double> &Car_x,
                                          QVector<double> &Car_y) {
  double theta_radian = 0;
  for (int i = 0; i != polar_r.size(); ++i) {
    theta_radian = polar_theta[i] * M_PI / 180;
    Car_x[i] = polar_r[i] * std::cos(theta_radian) + center_x;
    Car_y[i] = polar_r[i] * std::sin(theta_radian) + center_y;
  }
}

void ThrusterDiag::realtimeDashboard() {
  Eigen::Vector3i _alpha_deg_first =
      globalvar::_threadloop.getrealtimealphadeg_first();
  Eigen::Vector3i _alpha_deg_second =
      globalvar::_threadloop.getrealtimealphadeg_second();
  Eigen::Vector3i _alpha_deg_third =
      globalvar::_threadloop.getrealtimealphadeg_third();
  Eigen::Vector3i _rotation_first =
      globalvar::_threadloop.getrealtimerotation_first();
  Eigen::Vector3i _rotation_second =
      globalvar::_threadloop.getrealtimerotation_second();
  Eigen::Vector3i _rotation_third =
      globalvar::_threadloop.getrealtimerotation_third();
  if (MAXCONNECTION > 0) {
    setuprealtimeRevolution(_rotation_first);
    setuprealtimerudder(_alpha_deg_first);
    for (int thruster_index = 0; thruster_index != 4; ++thruster_index) {
      generatePointerData(0, thruster_index);
      ui->customPlot_thruster->graph(3 * thruster_index + 3)
          ->setData(thruster1_pointer_X[thruster_index],
                    thruster1_pointer_Y[thruster_index]);

      center_texts[0][thruster_index]->setText(
          QString::number((int)(dashtext_factor[0][thruster_index] *
                                realtime_rev2angle[0][thruster_index])));
    }
    for (int rudder_index = 0; rudder_index != 2; ++rudder_index) {
      horizontalslider[0][rudder_index]->setValue(
          realtime_rudderangle[0][rudder_index]);
    }
    ui->customPlot_thruster->replot();
  }

  if (MAXCONNECTION > 1) {
    setuprealtimeRevolution(_rotation_first, _rotation_second);
    setuprealtimerudder(_alpha_deg_first, _alpha_deg_second);
    for (int thruster_index = 0; thruster_index != 4; ++thruster_index) {
      generatePointerData(1, thruster_index);

      ui->customPlot_thruster2->graph(3 * thruster_index + 3)
          ->setData(thruster2_pointer_X[thruster_index],
                    thruster2_pointer_Y[thruster_index]);
      center_texts[1][thruster_index]->setText(
          QString::number((int)(dashtext_factor[1][thruster_index] *
                                realtime_rev2angle[1][thruster_index])));
    }
    for (int rudder_index = 0; rudder_index != 2; ++rudder_index) {
      horizontalslider[1][rudder_index]->setValue(
          realtime_rudderangle[1][rudder_index]);
    }
    ui->customPlot_thruster2->replot();
  }

  if (MAXCONNECTION > 2) {
    setuprealtimeRevolution(_rotation_first, _rotation_second, _rotation_third);
    setuprealtimerudder(_alpha_deg_first, _alpha_deg_second, _alpha_deg_third);
    for (int thruster_index = 0; thruster_index != 6; ++thruster_index) {
      generatePointerData(2, thruster_index);
      ui->customPlot_thruster3->graph(3 * thruster_index + 3)
          ->setData(thruster3_pointer_X[thruster_index],
                    thruster3_pointer_Y[thruster_index]);
      center_texts[2][thruster_index]->setText(
          QString::number((int)(dashtext_factor[2][thruster_index] *
                                realtime_rev2angle[2][thruster_index])));
    }
    for (int rudder_index = 0; rudder_index != 4; ++rudder_index)
      horizontalslider[2][rudder_index]->setValue(
          realtime_rudderangle[2][rudder_index]);
    ui->customPlot_thruster3->replot();
  }
}

void ThrusterDiag::initializevesselshape(QCustomPlot *customPlot,
                                         double translationx /*= 0*/,
                                         double translationy /*= -4.7*/) {
  /* initialize the 2D vessel shape, whose graph index will be 0 */
  customPlot->addGraph();
  // convert position and orientation of vessel to global coordinate
  int arraylength = 592;
  QVector<double> vesselshape_x(arraylength, 0);
  QVector<double> vesselshape_y(arraylength, 0);

  double t_orient = 90 * M_PI / 180;
  double radius = 0.78;
  double c_value = radius * std::cos(t_orient);
  double s_value = radius * std::sin(t_orient);
  for (int i = 0; i != arraylength; ++i) {
    vesselshape_x[i] = c_value * globalvar::vesselshape_x[i] -
                       s_value * globalvar::vesselshape_y[i] + translationx;
    vesselshape_y[i] = s_value * globalvar::vesselshape_x[i] +
                       c_value * globalvar::vesselshape_y[i] + translationy;
  }

  customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
  QCPScatterStyle myQCPScatterStyle1(QCPScatterStyle::ssDisc,
                                     QColor(238, 156, 167, 255), 2);
  customPlot->graph(0)->setScatterStyle(myQCPScatterStyle1);
  customPlot->graph(0)->setData(vesselshape_x, vesselshape_y);
  customPlot->graph(0)->rescaleAxes(true);
}

void ThrusterDiag::initializedenseTick(QCustomPlot *customPlot,
                                       int graph_index) {
  // setup dense tick, whose graph index will be 3*i+2
  customPlot->addGraph();

  customPlot->graph(graph_index)->setLineStyle(QCPGraph::lsNone);
  QCPScatterStyle myQCPScatterStyle(QCPScatterStyle::ssDisc,
                                    QColor(68, 68, 68, 255), 1);
  customPlot->graph(graph_index)->setScatterStyle(myQCPScatterStyle);
  customPlot->graph(graph_index)->setData(dbgdense_x, dbgdense_y);
  customPlot->graph(graph_index)->rescaleAxes(true);
}

void ThrusterDiag::initializesparseTick(QCustomPlot *customPlot,
                                        int graph_index) {
  // setup dense and sparse tick, whose graph index will be 3*i+1
  customPlot->addGraph();

  customPlot->graph(graph_index)->setLineStyle(QCPGraph::lsNone);
  QCPScatterStyle myQCPScatterStyle(QCPScatterStyle::ssDisc,
                                    QColor(68, 68, 68, 255), 2);
  customPlot->graph(graph_index)->setScatterStyle(myQCPScatterStyle);
  customPlot->graph(graph_index)->setData(dbgsparse_x, dbgsparse_y);
  customPlot->graph(graph_index)->rescaleAxes(true);
}

void ThrusterDiag::initializePointer(QCustomPlot *customPlot, int graph_index,
                                     int vessel_index, int thruster_index) {
  // setup pointer, whose graph index will be 3*i+3
  customPlot->addGraph();
  customPlot->graph(graph_index)->setPen(QPen(Qt::red, 3));
  switch (vessel_index) {
    case 0: {
      customPlot->graph(graph_index)
          ->setData(thruster1_pointer_X[thruster_index],
                    thruster1_pointer_Y[thruster_index]);
      break;
    }
    case 1: {
      customPlot->graph(graph_index)
          ->setData(thruster2_pointer_X[thruster_index],
                    thruster2_pointer_Y[thruster_index]);
      break;
    }
    case 2: {
      customPlot->graph(graph_index)
          ->setData(thruster3_pointer_X[thruster_index],
                    thruster3_pointer_Y[thruster_index]);
      break;
    }
    default:
      break;
  }
  customPlot->graph(graph_index)->rescaleAxes(true);
}

void ThrusterDiag::initializeTextDash(QCustomPlot *customPlot, int index_vessel,
                                      int index_thruster, double center_x,
                                      double center_y, int _index_bow) {
  // setup text label in the circle
  QVector<QCPItemText *> dbgtext_sparse;
  for (unsigned i = 0; i != num_angle_text; ++i) {
    dbgtext_sparse.push_back(new QCPItemText(customPlot));
    dbgtext_sparse.back()->position->setCoords(text_posx[i], text_posy[i]);
    if (_index_bow && (i * step_angle_text > 180)) {
      dbgtext_sparse.back()->setText(
          QString::number((int)(-(num_angle_text - i) * step_angle_text) *
                          dashtext_factor[index_vessel][index_thruster]));
    } else {
      dbgtext_sparse.back()->setText(QString::number(
          i * step_angle_text * dashtext_factor[index_vessel][index_thruster]));
    }

    dbgtext_sparse.back()->setFont(
        QFont("SansSerif", 6 * dash_radius, QFont::Bold));
    dbgtext_sparse.back()->setColor(Qt::black);
  }

  // setup text label in the center
  QCPItemText *center_text = new QCPItemText(customPlot);
  center_text->position->setCoords(center_x, 0.1 * dash_radius + center_y);
  center_text->setText(QString::number(0));
  center_text->setFont(QFont("SansSerif", 10 * dash_radius, QFont::Bold));
  center_text->setColor(Qt::black);

  center_texts[index_vessel] << center_text;

  // setup text label in the bottom
  QCPItemText *bottom_text = new QCPItemText(customPlot);
  bottom_text->position->setCoords(center_x, -0.2 * dash_radius + center_y);
  bottom_text->setText("r/min");
  bottom_text->setFont(QFont("SansSerif", 7.5 * dash_radius));
  bottom_text->setColor(QColor(119, 136, 153, 255));
}

void ThrusterDiag::initializeCircle(QCustomPlot *customPlot, double center_x,
                                    double center_y) {
  // create curve objects
  QCPCurve *fermatSpiral_outer =
      new QCPCurve(customPlot->xAxis, customPlot->yAxis);
  QCPCurve *fermatSpiral_inner =
      new QCPCurve(customPlot->xAxis, customPlot->yAxis);

  fermatSpiral_outer->data()->set(dataSpiral_outer, true);
  fermatSpiral_inner->data()->set(dataSpiral_inner, true);
  fermatSpiral_outer->setPen(QPen(QColor(68, 68, 68, 255), 2 * dash_radius));

  double ratio = bowthrustersize / 2 / qcustomplot_max / 1.03;
  QRadialGradient radialGrad(QPointF(bowthrustersize / 2 + center_x * ratio,
                                     bowthrustersize / 2 - center_y * ratio),
                             80 * dash_radius);
  radialGrad.setColorAt(1, QColor(0, 12, 64, 255));
  radialGrad.setColorAt(0.8, QColor(0, 12, 64, 100));
  radialGrad.setColorAt(0.5, QColor(240, 240, 240, 100));
  fermatSpiral_inner->setPen(QPen(QColor(0, 12, 64, 255), 0.5));
  fermatSpiral_inner->setBrush(QBrush(radialGrad));
}

void ThrusterDiag::generatethrusterAndSliderPosition() {
  // initialize thruster center
  thruster_center_X[0] = QVector<double>{0, 0, 1.5, -1.5};
  thruster_center_Y[0] = QVector<double>{3, 0.5, -3.2, -3.2};
  thruster_center_X[1] = QVector<double>{0, 0, 1.5, -1.5};
  thruster_center_Y[1] = QVector<double>{3, 0.5, -3.2, -3.2};
  if (MAXCONNECTION == 3) {
    thruster_center_X[2] = QVector<double>{0, 0, 2.4, -2.4, 1.5, -1.5};
    thruster_center_Y[2] = QVector<double>{3, 0.5, -0.8, -0.8, -3.2, -3.2};
  }

  // initialize position of slider
  int ymove = 65;
  slider_position_x[0] = QVector<int>{20, 340};
  slider_position_y[0] =
      QVector<int>{bowthrustersize - ymove, bowthrustersize - ymove};
  slider_position_x[1] = QVector<int>{655, 950};
  slider_position_y[1] =
      QVector<int>{bowthrustersize - ymove, bowthrustersize - ymove};
  if (MAXCONNECTION == 3) {
    slider_position_x[2] = QVector<int>{1220, 1710, 1310, 1620};
    slider_position_y[2] = QVector<int>{400, 400, bowthrustersize - ymove,
                                        bowthrustersize - ymove};
  }

  // initialize position of slider text
  slidertext_position_x[0] = QVector<double>{-2.6, 0.6};
  slidertext_position_y[0] = QVector<double>{-4.9, -4.9};
  slidertext_position_x[1] = QVector<double>{-4, -1.0};
  slidertext_position_y[1] = QVector<double>{-4.9, -4.9};
  if (MAXCONNECTION == 3) {
    slidertext_position_x[2] = QVector<double>{0.15, -4.9, -0.8, -3.9};
    slidertext_position_y[2] = QVector<double>{0.6, 0.6, -4.9, -4.9};
  }
}

void ThrusterDiag::generatetext_factor() {
  // text factor of dashboard
  dashtext_factor[0] = QVector<int>{25, 25, 2, 2};
  dashtext_factor[1] = QVector<int>{25, 25, 2, 2};
  if (MAXCONNECTION == 3) {
    dashtext_factor[2] = QVector<int>{20, 20, 2, 2, 3, 3};
  }

  // text factor of slider
  slidertext_factor[0] = QVector<int>{90, 90};
  slidertext_factor[1] = QVector<int>{90, 90};
  if (MAXCONNECTION == 3) {
    slidertext_factor[2] = QVector<int>{1, 1, 2, 2};
  }
}

void ThrusterDiag::initializePointerAndSliderData() {
  // initialize revolution2angle
  realtime_rev2angle[0] = QVector<double>(4, 0);
  realtime_rev2angle[1] = QVector<double>(4, 0);
  realtime_rev2angle[2] = QVector<double>(6, 0);

  // initialize each
  for (int i = 0; i != 4; ++i) {
    thruster1_pointer_X[i] = QVector<double>(num_pointer, 0);
    thruster1_pointer_Y[i] = QVector<double>(num_pointer, 0);
    thruster2_pointer_X[i] = QVector<double>(num_pointer, 0);
    thruster2_pointer_Y[i] = QVector<double>(num_pointer, 0);
  }
  if (MAXCONNECTION == 3) {
    for (int i = 0; i != 6; ++i) {
      thruster3_pointer_X[i] = QVector<double>(num_pointer, 0);
      thruster3_pointer_Y[i] = QVector<double>(num_pointer, 0);
    }
  }

  // initialize rudder angles
  realtime_rudderangle[0] = QVector<int>(2, 50);
  realtime_rudderangle[1] = QVector<int>(2, 50);
  realtime_rudderangle[2] = QVector<int>(4, 50);
}

void ThrusterDiag::initializeoneslider(QCustomPlot *customPlot,
                                       int vessel_index, int slider_index) {
  QSlider *_horizontalSlider = new QSlider(Qt::Horizontal, this);
  _horizontalSlider->setGeometry(slider_position_x[vessel_index][slider_index],
                                 slider_position_y[vessel_index][slider_index],
                                 200, 30);
  _horizontalSlider->setFocusPolicy(Qt::StrongFocus);
  _horizontalSlider->setTickPosition(QSlider::TicksBothSides);
  _horizontalSlider->setTickInterval(25);
  _horizontalSlider->setSingleStep(1);
  _horizontalSlider->setValue(realtime_rudderangle[vessel_index][slider_index]);
  horizontalslider[vessel_index] << _horizontalSlider;

  // the text of slider
  double center_x = slidertext_position_x[vessel_index][slider_index];
  double center_y = slidertext_position_y[vessel_index][slider_index];
  initializeoneslidertext(customPlot, vessel_index, slider_index, center_x,
                          center_y);
}

void ThrusterDiag::initializeoneslidertext(QCustomPlot *customPlot,
                                           int vessel_index, int slider_index,
                                           double center_x, double center_y) {
  // setup text label in the circle
  QVector<QCPItemText *> slidertext;
  int _minv = num_slider_text / 2;
  for (unsigned i = 0; i != num_slider_text; ++i) {
    slidertext.push_back(new QCPItemText(customPlot));
    slidertext.back()->position->setCoords(center_x + i * 0.5, center_y);
    int temp = (-_minv + i) * slidertext_factor[vessel_index][slider_index];
    slidertext.back()->setText(QString::number(temp));
    slidertext.back()->setFont(
        QFont("SansSerif", 7 * dash_radius, QFont::Bold));
    slidertext.back()->setColor(Qt::black);
  }
}

/* function for setup realtime rotation speed of motor */
void ThrusterDiag::setuprealtimeRevolution(const Eigen::Vector3i &_rotation) {
  for (int i = 0; i != 2; ++i)
    realtime_rev2angle[0][i] = _rotation(0) / dashtext_factor[0][i];
  for (int i = 1; i != 3; ++i)
    realtime_rev2angle[0][i + 1] =
        abs(_rotation(i)) / dashtext_factor[0][i + 1];
}
void ThrusterDiag::setuprealtimeRevolution(
    const Eigen::Vector3i &_rotation_first,
    const Eigen::Vector3i &_rotation_second) {
  for (int i = 0; i != 2; ++i) {
    realtime_rev2angle[0][i] = _rotation_first(0) / dashtext_factor[0][i];
    realtime_rev2angle[1][i] = _rotation_second(0) / dashtext_factor[1][i];
  }
  for (int i = 1; i != 3; ++i) {
    realtime_rev2angle[0][i + 1] =
        _rotation_first(i) / dashtext_factor[0][i + 1];
    realtime_rev2angle[1][i + 1] =
        _rotation_second(i) / dashtext_factor[1][i + 1];
  }
}
void ThrusterDiag::setuprealtimeRevolution(
    const Eigen::Vector3i &_rotation_first,
    const Eigen::Vector3i &_rotation_second,
    const Eigen::Vector3i &_rotation_third) {
  for (int i = 0; i != 2; ++i) {
    realtime_rev2angle[0][i] = _rotation_first(0) / dashtext_factor[0][i];
    realtime_rev2angle[1][i] = _rotation_second(0) / dashtext_factor[1][i];
  }
  for (int i = 1; i != 3; ++i) {
    realtime_rev2angle[0][i + 1] =
        _rotation_first(i) / dashtext_factor[0][i + 1];
    realtime_rev2angle[1][i + 1] =
        _rotation_second(i) / dashtext_factor[1][i + 1];
  }

  // update the revolution of the third vessel
  realtime_rev2angle[2][0] = _rotation_third(0) / dashtext_factor[2][0];
  for (int i = 0; i != 3; ++i)
    realtime_rev2angle[2][i + 1] =
        _rotation_third(i) / dashtext_factor[2][i + 1];
}

/* function for setup realtime rudder angle and azimuth angle */
void ThrusterDiag::setuprealtimerudder(const Eigen::Vector3i &_alpha_deg) {
  for (int i = 0; i != 2; ++i)
    realtime_rudderangle[0][i] =
        50 + (int)(_alpha_deg(i + 1) * 100 / (num_slider_text - 1) /
                   slidertext_factor[0][i]);
}
void ThrusterDiag::setuprealtimerudder(
    const Eigen::Vector3i &_alpha_deg_first,
    const Eigen::Vector3i &_alpha_deg_second) {
  for (int i = 0; i != 2; ++i) {
    realtime_rudderangle[0][i] =
        50 + (int)(_alpha_deg_first(i + 1) * 100 / (num_slider_text - 1) /
                   slidertext_factor[0][i]);
    realtime_rudderangle[1][i] =
        50 + (int)(_alpha_deg_second(i + 1) * 100 / (num_slider_text - 1) /
                   slidertext_factor[1][i]);
  }
}
void ThrusterDiag::setuprealtimerudder(
    const Eigen::Vector3i &_alpha_deg_first,
    const Eigen::Vector3i &_alpha_deg_second,
    const Eigen::Vector3i &_alpha_deg_third) {
  for (int i = 0; i != 2; ++i) {
    realtime_rudderangle[0][i] =
        50 + (int)(_alpha_deg_first(i + 1) * 100 / (num_slider_text - 1) /
                   slidertext_factor[0][i]);
    realtime_rudderangle[1][i] =
        50 + (int)(_alpha_deg_second(i + 1) * 100 / (num_slider_text - 1) /
                   slidertext_factor[1][i]);
  }
  // update the rudder angle of third vessel
  for (int i = 0; i != 2; ++i)
    realtime_rudderangle[2][i] =
        50 + (int)(_alpha_deg_third(i + 1) * 100 / (num_slider_text - 1) /
                   slidertext_factor[2][i]);
}

void ThrusterDiag::generatePointerData(int vessel_index, int thruster_index) {
  double center_x = thruster_center_X[vessel_index][thruster_index] +
                    tranlation_vessel_x[vessel_index];
  double center_y = thruster_center_Y[vessel_index][thruster_index];

  double angle = realtime_rev2angle[vessel_index][thruster_index] * M_PI / 180;
  double c_value = dash_radius * std::cos(angle);
  double s_value = dash_radius * std::sin(angle);
  switch (vessel_index) {
    case 0: {
      thruster1_pointer_X[thruster_index][0] = 0.2 * c_value + center_x;
      thruster1_pointer_Y[thruster_index][0] = 0.2 * s_value + center_y;
      thruster1_pointer_X[thruster_index][1] = 0.7 * c_value + center_x;
      thruster1_pointer_Y[thruster_index][1] = 0.7 * s_value + center_y;
      break;
    }
    case 1: {
      thruster2_pointer_X[thruster_index][0] = 0.2 * c_value + center_x;
      thruster2_pointer_Y[thruster_index][0] = 0.2 * s_value + center_y;
      thruster2_pointer_X[thruster_index][1] = 0.7 * c_value + center_x;
      thruster2_pointer_Y[thruster_index][1] = 0.7 * s_value + center_y;
      break;
    }
    case 2: {
      thruster3_pointer_X[thruster_index][0] = 0.2 * c_value + center_x;
      thruster3_pointer_Y[thruster_index][0] = 0.2 * s_value + center_y;
      thruster3_pointer_X[thruster_index][1] = 0.7 * c_value + center_x;
      thruster3_pointer_Y[thruster_index][1] = 0.7 * s_value + center_y;
      break;
    }
    default:
      break;
  }
}

void ThrusterDiag::generatetranslationdata() {
  tranlation_vessel_x = {0, -1.5, -1.4};
  tranlation_vessel_y = {-4.4, -4.4, -4.4};
}

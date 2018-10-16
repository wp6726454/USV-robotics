/*
***********************************************************************
* constants.h:
* header file to define the global parameters, dependent on each
* autonomous system.
* This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(Mr. SJTU)
***********************************************************************
*/
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include <string>

#define CLIENT_DATA_SIZE 9  // define length of float array for client data
// maximum number of clients connected, "0" means unlimited number of clients
#define MAXCONNECTION 1
#define PORT "9035"  // port we're listening on

// max number of bytes we can get at once for socket server
#define MAXDATASIZE 100
#define M_PI 3.14159265358979323846

// number to decide QT or terminal, 1 indicates using Qt and save a file, 0
// indicates using terminal
#define FILEORNOT 1

// refresh period of each variable
#define VIEWERREFRESH 20  // ms

const double sample_time = 0.1;            // second
const int sample_mtime = 100;              // ms
const unsigned int sample_utime = 100000;  // us
const std::string dbsavepath =
    "/home/skloe/Coding/CPP1X/USV/DPfloatover/QT/build/data/mytest.db";
const std::string logsavepath =
    "/home/skloe/Coding/CPP1X/USV/DPfloatover/QT/build/data/log";
const std::string xmlpath =
    "/home/skloe/Coding/CPP1X/USV/DPfloatover/QT/build/xml/test.xml";
/* model properties of vessel 1 (K class-I)
*********************************************************************** */
struct vessel_first {
  // Mass and damping matrix
  const double Mass[9];
  const double Damping[9];
  // pid controller
  const double P_x;
  const double P_y;
  const double P_theta;
  const double I_x;
  const double I_y;
  const double I_theta;
  const double D_x;
  const double D_y;
  const double D_theta;
  const double allowed_error_x;
  const double allowed_error_y;
  const double allowed_error_orientation;
  // manual controller
  const double maxpositive_x_thruster;   // N
  const double maxnegative_x_thruster;   // N
  const double maxpositive_y_thruster;   // N
  const double maxnegative_y_thruster;   // N
  const double maxpositive_Mz_thruster;  // N*m
  const double maxnegative_Mz_thruster;  // N*m
  // dimension of each variables
  const int m;
  const int n;
  const int numvar;
  const int num_constraints;  // the number of constraints

  // we use quadratic function to give best fit for measured data
  // relationship between rotation and thrust of bow thruster
  const double Kbar_positive;        // alpha=90
  const double Kbar_negative;        // alpha=-90
  const int max_delta_rotation_bow;  // rpm
  const int max_rotation_bow;        // rpm
  const double max_thrust_bow_positive;
  const double max_thrust_bow_negative;

  // relationship between rotation and thrust of azimuth thruster
  const double K_left;
  const double K_right;
  const int max_delta_rotation_azimuth;  // rpm
  const int max_rotation_azimuth;        // rpm
  const double max_thrust_azimuth_left;
  const double max_thrust_azimuth_right;
  const double max_delta_alpha_azimuth;  // rad
  const double max_alpha_azimuth_left;   // rad
  const double min_alpha_azimuth_left;   // rad
  const double max_alpha_azimuth_right;  // rad
  const double min_alpha_azimuth_right;  // rad

  // position of each propeller
  const double bow_x;
  const double bow_y;
  const double left_x;
  const double left_y;
  const double right_x;
  const double right_y;
};

/* model properties of vessel 2 (K class-II)
*********************************************************************** */
struct vessel_second {
  // Mass and damping matrix
  const double Mass[9];
  const double Damping[9];
  // pid controller
  const double P_x;
  const double P_y;
  const double P_theta;
  const double I_x;
  const double I_y;
  const double I_theta;
  const double D_x;
  const double D_y;
  const double D_theta;
  const double allowed_error_x;
  const double allowed_error_y;
  const double allowed_error_orientation;

  // manual controller
  const double maxpositive_x_thruster;   // N
  const double maxnegative_x_thruster;   // N
  const double maxpositive_y_thruster;   // N
  const double maxnegative_y_thruster;   // N
  const double maxpositive_Mz_thruster;  // N*m
  const double maxnegative_Mz_thruster;  // N*m
  // dimension of each variables
  const int m;
  const int n;
  const int numvar;
  const int num_constraints;  // the number of constraints

  // we use quadratic function to give best fit for measured data
  // relationship between rotation and thrust of bow thruster
  const double Kbar_positive;        // alpha=90
  const double Kbar_negative;        // alpha=-90
  const int max_delta_rotation_bow;  // rpm
  const int max_rotation_bow;        // rpm
  const double max_thrust_bow_positive;
  const double max_thrust_bow_negative;

  // relationship between rotation and thrust of azimuth thruster
  const double K_left;
  const double K_right;
  const int max_delta_rotation_azimuth;  // rpm
  const int max_rotation_azimuth;        // rpm
  const double max_thrust_azimuth_left;
  const double max_thrust_azimuth_right;
  const double max_delta_alpha_azimuth;  // rad
  const double max_alpha_azimuth_left;   // rad
  const double min_alpha_azimuth_left;   // rad
  const double max_alpha_azimuth_right;  // rad
  const double min_alpha_azimuth_right;  // rad

  // position of each propeller
  const double bow_x;
  const double bow_y;
  const double left_x;
  const double left_y;
  const double right_x;
  const double right_y;
};

/* model properties of vessel 3 (x class)
*********************************************************************** */
struct vessel_third {
  // Mass and damping matrix
  const double Mass[9];
  const double Damping[9];
  // pid controller
  const double P_x;
  const double P_y;
  const double P_theta;
  const double I_x;
  const double I_y;
  const double I_theta;
  const double D_x;
  const double D_y;
  const double D_theta;
  const double allowed_error_x;
  const double allowed_error_y;
  const double allowed_error_orientation;

  // manual controller
  const double maxpositive_x_thruster;   // N
  const double maxnegative_x_thruster;   // N
  const double maxpositive_y_thruster;   // N
  const double maxnegative_y_thruster;   // N
  const double maxpositive_Mz_thruster;  // N*m
  const double maxnegative_Mz_thruster;  // N*m
  // dimension of each variables
  const int m;
  const int n;
  const int numvar;
  const int num_constraints;  // the number of constraints

  // we use quadratic function to give best fit for measured data
  // relationship between rotation and thrust of bow thruster
  const double Kbar_positive;        // alpha=90
  const double Kbar_negative;        // alpha=-90
  const int max_delta_rotation_bow;  // rpm
  const int max_rotation_bow;        // rpm
  const double max_thrust_bow_positive;
  const double max_thrust_bow_negative;

  // relationship between rotation and thrust of azimuth thruster
  const double K_left;
  const double K_right;
  const int max_delta_rotation_azimuth;  // rpm
  const int max_rotation_azimuth;        // rpm
  const double max_thrust_azimuth_left;
  const double max_thrust_azimuth_right;
  const double max_delta_alpha_azimuth;  // rad
  const double max_alpha_azimuth_left;   // rad
  const double min_alpha_azimuth_left;   // rad
  const double max_alpha_azimuth_right;  // rad
  const double min_alpha_azimuth_right;  // rad

  // position of each propeller
  const double bow_x;
  const double bow_y;
  const double left_x;
  const double left_y;
  const double right_x;
  const double right_y;
};
#endif /*_CONSTANTS_H_*/

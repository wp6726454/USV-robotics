/*
***********************************************************************
* databasecpp.h:
* sqlite modern cpp wrapper
*.This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr. SKLOE@SJTU)
***********************************************************************
*/

#ifndef _DATABASECPP_H_
#define _DATABASECPP_H_

#include <sqlite_modern_cpp.h>
#include <Eigen/Core>
#include "constants.h"
#include "realtimedata.h"

class databasecpp {
 public:
  explicit databasecpp(const std::string &savepath = "dbfile.db")
      : db(savepath), clientset(MAXCONNECTION, "") {}
  ~databasecpp() {}
  // maseter table: contains general information about all clients
  void create_mastertable() {
    db << "CREATE TABLE IF NOT EXISTS MASTER("
          "CLIENT_ID    INT    PRIMARY KEY    NOT NULL,"
          "TABLE_NAME   TEXT                          ,"
          "DATETIME     TEXT                         );";
  }

  void update_mastertable(int _clientid) {
    // update clientset
    std::string s_id = std::to_string(_clientid);
    clientset[_clientid] = "client" + s_id;
    // create string for sqlite
    std::string str(
        "INSERT INTO MASTER (CLIENT_ID,TABLE_NAME,DATETIME) VALUES(");
    str += s_id;
    str += ", 'client";
    str += s_id;
    str += "', DATETIME('now'));";
    db << str;
  }
  // create each client table
  void create_client_table(int _clientid) {
    std::string str = "CREATE TABLE " + clientset[_clientid] +
                      "(ID          INTEGER PRIMARY KEY AUTOINCREMENT,"
                      " STATUS      BOOLEAN    NOT NULL,"
                      " DATETIME    TEXT       NOT NULL,"
                      " SURGE       DOUBLE, "
                      " SWAY        DOUBLE, "
                      " HEAVE       DOUBLE, "
                      " ROLL        DOUBLE, "
                      " PITCH       DOUBLE, "
                      " YAW         DOUBLE, " /* measurement*/
                      " x           DOUBLE, "
                      " y           DOUBLE, "
                      " theta       DOUBLE, "
                      " u           DOUBLE, "
                      " v           DOUBLE, "
                      " r           DOUBLE, " /* state*/
                      " tauX        DOUBLE, "
                      " tauY        DOUBLE, "
                      " tauMz       DOUBLE, " /* tau: desired force */
                      " estX        DOUBLE, "
                      " estY        DOUBLE, "
                      " estMz       DOUBLE, " /* BalphaU: estimated force */
                      " alpha_bow   INT, "
                      " alpha_left  INT, "
                      " alpha_right INT, " /* the angle of each propeller*/
                      " rpm_bow     INT, "
                      " rpm_left    INT, "
                      " rpm_right   INT); "; /* the speed of each propeller*/
    db << str;
  }

  // insert a row into some client table
  void update_client_table(bool t_status,
                           const realtimevessel_first &_realtimevessel_first) {
    std::string str = "INSERT INTO " + clientset[0];
    // t_status=0 means corrected data
    std::string str_end("");
    if (t_status) {
      str_end = "(STATUS, DATETIME) VALUES( 1 , julianday('now'));";
    } else {
      str_end =
          "(STATUS, DATETIME, SURGE, SWAY, HEAVE, ROLL, PITCH, "
          "YAW, x, y, theta, u, v, r, tauX, tauY, tauMz, estX, estY, "
          "estMz, alpha_bow, alpha_left, alpha_right, rpm_bow, rpm_left, "
          "rpm_right) VALUES( 0 , julianday('now')";
      convert_Eigendouble2string(_realtimevessel_first, str_end);
      str_end += ");";
    }
    str += str_end;
    db << str;
  }
  // insert a row into some client table
  void update_client_table(
      bool t_status, const realtimevessel_second &_realtimevessel_second) {
    std::string str = "INSERT INTO " + clientset[1];
    // t_status=0 means corrected data
    std::string str_end("");
    if (t_status) {
      str_end = "(STATUS, DATETIME) VALUES( 1 , julianday('now'));";
    } else {
      str_end =
          "(STATUS, DATETIME, SURGE, SWAY, HEAVE, ROLL, PITCH, "
          "YAW, x, y, theta, u, v, r, tauX, tauY, tauMz, estX, estY, "
          "estMz, alpha_bow, alpha_left, alpha_right, rpm_bow, rpm_left, "
          "rpm_right) VALUES( 0 , julianday('now')";
      convert_Eigendouble2string(_realtimevessel_second, str_end);
      str_end += ");";
    }
    str += str_end;
    db << str;
  }

 private:
  sqlite::database db;
  // the set of clients' name
  // 0 --> the first vessel
  // 1 --> the second vessel
  // 2 --> the third vessel
  std::vector<std::string> clientset;

  // convert real time data to sql string (the first vessel)
  void convert_Eigendouble2string(
      const realtimevessel_first &_realtimevessel_first, std::string &_str) {
    // measured 6 DoF motion
    for (int i = 0; i != 6; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_first.Position(i));
    }
    // real time state
    for (int i = 0; i != 6; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_first.State(i));
    }
    // desired force
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_first.tau(i));
    }
    // estimated force
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_first.BalphaU(i));
    }
    // angle of each propeller
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_first.alpha_deg(i));
    }
    // speed of each propeller
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_first.rotation(i));
    }
  }

  // convert real time data to sql string (the second vessel)
  void convert_Eigendouble2string(
      const realtimevessel_second &_realtimevessel_second, std::string &_str) {
    // measured 6 DoF motion
    for (int i = 0; i != 6; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_second.Position(i));
    }
    // real time state
    for (int i = 0; i != 6; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_second.State(i));
    }
    // desired force
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_second.tau(i));
    }
    // estimated force
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_second.BalphaU(i));
    }
    // angle of each propeller
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_second.alpha_deg(i));
    }
    // speed of each propeller
    for (int i = 0; i != 3; ++i) {
      _str += ", ";
      _str += std::to_string(_realtimevessel_second.rotation(i));
    }
  }
};

#endif /*_DATABASECPP_H_*/

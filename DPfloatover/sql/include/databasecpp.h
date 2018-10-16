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
                      " tayMz       DOUBLE);";
    " u          DOUBLE, "
    " v          DOUBLE, "
    " r          DOUBLE);";
    db << str;
  }
  // insert a row into some client table
  void update_client_table(int _clientid, bool t_status,
                           const Eigen::VectorXd &_motiondata) {
    std::string str = "INSERT INTO " + clientset[_clientid];
    // t_status=0 means corrected data
    std::string str_end("");
    if (t_status) {
      str_end = "(STATUS, DATETIME) VALUES( 1 , julianday('now'));";
    } else {
      str_end =
          "(STATUS, DATETIME, SURGE, SWAY, HEAVE, ROLL, PITCH, "
          "YAW, Ux, Uy, Utheta) VALUES( 0 , julianday('now')";
      convert_Eigendouble2string(_motiondata, str_end);
      str_end += ");";
    }
    str += str_end;
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
          "YAW, Ux, Uy, Utheta) VALUES( 0 , julianday('now')";
      convert_Eigendouble2string(_motiondata, str_end);
      str_end += ");";
    }
    str += str_end;
    db << str;
  }
  // insert a row into some client table
  void update_client_table(
      bool t_status, const realtimevessel_second &_realtimevessel_second) {
    std::string str = "INSERT INTO " + clientset[_clientid];
    // t_status=0 means corrected data
    std::string str_end("");
    if (t_status) {
      str_end = "(STATUS, DATETIME) VALUES( 1 , julianday('now'));";
    } else {
      str_end =
          "(STATUS, DATETIME, SURGE, SWAY, HEAVE, ROLL, PITCH, "
          "YAW, Ux, Uy, Utheta) VALUES( 0 , julianday('now')";
      convert_Eigendouble2string(_motiondata, str_end);
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

  void convert_Eigendouble2string(const Eigen::VectorXd &_motiondata,
                                  std::string &_str) {
    for (int i = 0; i != CLIENT_DATA_SIZE; ++i) {
      _str += ", ";
      _str += std::to_string(_motiondata(i));
    }
  }
};

#endif /*_DATABASECPP_H_*/

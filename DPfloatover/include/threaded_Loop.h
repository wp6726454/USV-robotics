/*
***********************************************************************
* thread_Loop.hpp: thread-based concurrent socket server
* function to run the whole loop on server (including receive from clients,
* crccheck, unpack, 6D motion capture, LQE, LQR, thruster allocation, pack, send
* to clients, save2sqlite, viewer).
* This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _THREADLOOP_HPP_
#define _THREADLOOP_HPP_

#include "../controller/pidcontroller/include/controller.h"
#include "../joystick/include/gamepadmonitor.h"
#include "../motioncapture/include/motioncapture.h"
#include "../network/include/crccheck.h"
#include "../network/include/datapack.h"
#include "../network/include/tcpserver_t.h"
// #include "../sql/include/database.h"
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <unordered_map>
#include "../sql/include/databasecpp.h"
#include "constants.h"
#include "timecounter.hpp"

class threadloop {
 public:
  threadloop()
      : connection_status(0),
        mydb(dbsavepath),
        _controller_first(_vessel_first, _realtimevessel_first),
        _controller_second(_vessel_second, _realtimevessel_second),
        _controller_third(_vessel_third, _realtimevessel_third) {}
  ~threadloop() {}

  void initializelooop() {
    // open system file descriptors
    if (FILEORNOT) {
      myfile = fopen(logsavepath.c_str(), "a+");
      // initialize socket server
      myserver.initializesocketserver(myfile);
    } else {
      // initialize socket server
      myserver.initializesocketserver(NULL);
    }
    // sqlite to create master table
    mydb.create_mastertable();
  }
  // start socket server and wait for clients
  void start_connnection_t() {
    std::thread _thread(&threadloop::start_connection, this);
    _threadid_connection = _thread.native_handle();
    _thread.detach();
  }
  // loop for send and recive data using socket
  void receiveallclients_t() {
    if (MAXCONNECTION > 0) {
      // thread for the first vessel
      std::thread _threadfirst(&threadloop::ReceiveClient_first, this, 0);
      if (FILEORNOT) {  // join for terminal, detach for QT
        _tmclients[0] = _threadfirst.native_handle();
        _threadfirst.detach();
      } else
        _threadfirst.join();
    }
    if (MAXCONNECTION > 1) {
      // thread for the second vessel
      std::thread _threadsecond(&threadloop::ReceiveClient_second, this, 1);
      if (FILEORNOT) {  // join for terminal, detach for QT
        _tmclients[1] = _threadsecond.native_handle();
        _threadsecond.detach();
      } else
        _threadsecond.join();
    }
    if (MAXCONNECTION > 2) {
      // thread for the third vessel
      std::thread _threadthird(&threadloop::ReceiveClient_third, this, 2);
      if (FILEORNOT) {  // join for terminal, detach for QT
        _tmclients[2] = _threadthird.native_handle();
        _threadthird.detach();
      } else
        _threadthird.join();
    }
  }
  // gampad
  void updategamepad_t() {
    std::thread _threadgamepad(&threadloop::updategamepad, this);
    if (FILEORNOT) {  // join for terminal, detach for QT
      _threadid_gamepad = _threadgamepad.native_handle();
      _threadgamepad.detach();
    } else
      _threadgamepad.join();
  }
  // motion capture
  void updatemotioncapture_t() {
    std::thread _threadmotioncapure(&threadloop::updatemotion, this);
    if (FILEORNOT) {  // join for terminal, detach for QT
      _threadid_motion = _threadmotioncapure.native_handle();
      _threadmotioncapure.detach();
    } else
      _threadmotioncapure.join();
  }

  // save data into sqlite database
  void save2database_t() {
    std::thread _thread(&threadloop::save2database, this);
    _threadid_database = _thread.native_handle();
    _thread.detach();
  }

  void closelooop() {
    if (FILEORNOT) {
      pthread_cancel(_threadid_connection);
      pthread_cancel(_threadid_gamepad);
      pthread_cancel(_threadid_motion);
      for (int i = 0; i != MAXCONNECTION; ++i) stopthread(i);
      pthread_cancel(_threadid_database);

      fclose(myfile);
    }
  }

  int get_connection_status() { return connection_status; }

  Vector6d getrealtime6dmotion_first() {
    return _realtimevessel_first.Position;
  }
  Vector6d getrealtime6dmotion_second() {
    return _realtimevessel_second.Position;
  }
  Vector6d getrealtime6dmotion_third() {
    return _realtimevessel_third.Position;
  }
  Eigen::Vector3i getrealtimealphadeg_first() {
    return _realtimevessel_first.alpha_deg;
  }
  Eigen::Vector3i getrealtimealphadeg_second() {
    return _realtimevessel_second.alpha_deg;
  }
  Eigen::Vector3i getrealtimealphadeg_third() {
    return _realtimevessel_third.alpha_deg;
  }
  Eigen::Vector3i getrealtimerotation_first() {
    return _realtimevessel_first.rotation;
  }
  Eigen::Vector3i getrealtimerotation_second() {
    return _realtimevessel_second.rotation;
  }
  Eigen::Vector3i getrealtimerotation_third() {
    return _realtimevessel_third.rotation;
  }

 private:
  // thread pool
  typedef std::unordered_map<int, pthread_t> ThreadMap;
  ThreadMap _tmclients;  // the id array of thread for socket connection
  pthread_t _threadid_connection;  // the id of thread for socket connection
  pthread_t _threadid_database;    // the id of thread for saving data
  pthread_t _threadid_motion;      // the id of thread for motion capture
  pthread_t _threadid_gamepad;     // the id of thread for gamepad
  //
  int connection_status;
  databasecpp mydb;
  gamepadmonitor mygamepad;
  // mysql mysqli;
  motioncapture mymotioncapture;
  threadtcpsocketserver myserver;
  FILE *myfile;

  // constant parameters of the first vessel
  vessel_first _vessel_first{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      6.0,                                     // maxpositive_x_thrust(N)
      5.0,                                     // maxnegative_x_thrust(N)
      3,                                       // maxpositive_y_thrust(N)
      2,                                       // maxnegative_y_thrust(N)
      5,                                       // maxpositive_Mz_thrust(N*m)
      3,                                       // maxnegative_Mz_thrust(N*m)
      // 26.0,                                    // maxpositive_x_thrust(N)
      // 25.0,                                    // maxnegative_x_thrust(N)
      // 6,                                       // maxpositive_y_thrust(N)
      // 4,                                       // maxnegative_y_thrust(N)
      // 11,                                      // maxpositive_Mz_thrust(N*m)
      // 7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,       // m
      3,       // n
      9,       // numvar
      3,       // num_constraints
      5.6e-7,  // Kbar_positive
      2.2e-7,  // Kbar_negative
      100,     // max_delta_rotation_bow
      4000,    // max_rotation_bow
      8.96,    // max_thrust_bow_positive
      3.52,    // max_thrust_bow_negative
      2e-5,    // K_left
      2e-5,    // K_right
      20,      // max_delta_rotation_bow
      1000,    // max_rotation_azimuth
      20,      // max_thrust_azimuth_left
      20,      // max_thrust_azimuth_right
      0.1277,  // max_delta_alpha_azimuth
      M_PI,    // max_alpha_azimuth_left
      0,       // min_alpha_azimuth_left
      0,       // max_alpha_azimuth_right
      -M_PI,   // min_alpha_azimuth_right
      1.9,     // bow_x
      0,       // bow_y
      -1.893,  // left_x
      -0.216,  // left_y
      -1.893,  // right_x
      0.216    // right_y
  };
  // constant parameters of the second vessel
  vessel_second _vessel_second{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };
  // constant parameters of the third vessel
  vessel_third _vessel_third{
      {623, 0, 0, 0, 706, 444, 0, 444, 1298},  // mass
      {17, 0, 0, 0, 20, 0, 0, 0, 100},         // damping
      4,                                       // P_x
      1,                                       // P_y
      5,                                       // P_theta
      0.2,                                     // I_x
      0.1,                                     // I_y
      1,                                       // I_theta
      0.1,                                     // D_x
      0.1,                                     // D_y
      0.2,                                     // D_theta
      26.0,                                    // maxpositive_x_thrust(N)
      25.0,                                    // maxnegative_x_thrust(N)
      6,                                       // maxpositive_y_thrust(N)
      4,                                       // maxnegative_y_thrust(N)
      11,                                      // maxpositive_Mz_thrust(N*m)
      7.6,                                     // maxnegative_Mz_thrust(N*m)
      3,                                       // m
      3,                                       // n
      9,                                       // numvar
      3,                                       // num_constraints
      5.6e-7,                                  // Kbar_positive
      2.2e-7,                                  // Kbar_negative
      100,                                     // max_delta_rotation_bow
      4000,                                    // max_rotation_bow
      8.96,                                    // max_thrust_bow_positive
      3.52,                                    // max_thrust_bow_negative
      2e-5,                                    // K_left
      2e-5,                                    // K_right
      20,                                      // max_delta_rotation_bow
      1000,                                    // max_rotation_azimuth
      20,                                      // max_thrust_azimuth_left
      20,                                      // max_thrust_azimuth_right
      0.1277,                                  // max_delta_alpha_azimuth
      M_PI,                                    // max_alpha_azimuth_left
      0,                                       // min_alpha_azimuth_left
      0,                                       // max_alpha_azimuth_right
      -M_PI,                                   // min_alpha_azimuth_right
      1.9,                                     // bow_x
      0,                                       // bow_y
      -1.893,                                  // left_x
      -0.216,                                  // left_y
      -1.893,                                  // right_x
      0.216                                    // right_y
  };

  // realtime parameters of the first vessel (K class-I)
  realtimevessel_first _realtimevessel_first{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                 // alpha
      Eigen::Vector3i::Zero(),                         // alpha_deg
      (Eigen::Vector3d() << 2, 0.1, 0.01).finished(),  // u
      Eigen::Vector3i::Zero()                          // rotation
  };
  // realtime parameters of the second vessel (K class-II)
  realtimevessel_second _realtimevessel_second{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                // alpha
      Eigen::Vector3i::Zero(),                        // alpha_deg
      (Eigen::Vector3d() << 3, 0.0, 0.0).finished(),  // u
      Eigen::Vector3i::Zero()                         // rotation
  };
  // realtime parameters of the third vessel (X class)
  realtimevessel_third _realtimevessel_third{
      Vector6d::Zero(),         // measurement
      Vector6d::Zero(),         // position
      Vector6d::Zero(),         // velocity
      Vector6d::Zero(),         // state
      Eigen::Vector3d::Zero(),  // tau
      Eigen::Vector3d::Zero(),  // BalphaU
      (Eigen::Vector3d() << -M_PI / 2, M_PI / 180, -M_PI / 30)
          .finished(),                                    // alpha
      Eigen::Vector3i::Zero(),                            // alpha_deg
      (Eigen::Vector3d() << 0.001, 0.0, 0.0).finished(),  // u
      Eigen::Vector3i::Zero()                             // rotation
  };

  // controller of each vessel
  controller_first _controller_first;
  controller_second _controller_second;
  controller_third _controller_third;

  // stop one thread for client's socket
  void stopthread(int threadindex) {
    ThreadMap::const_iterator it = _tmclients.find(threadindex);
    if (it != _tmclients.end()) {
      pthread_cancel(it->second);
      _tmclients.erase(threadindex);
    }
  }

  // this function will called by only one thread
  void start_connection() {
    while (1) {
      int newfd = myserver.acceptclients(myfile);
      if (newfd != -1) {
        int numexist = myserver.GetexistnumClients();
        // create master table
        mydb.update_mastertable(numexist - 1);
        mydb.create_client_table(numexist - 1);
        if (numexist == MAXCONNECTION) {
          connection_status = 1;
          break;
        }
      }
    }
  }
  // get the real time gamepad response
  void updategamepad() {
    while (1) {
      mygamepad.updategamepad();
    }
  }
  // get the real time motion response
  void updatemotion() {
    while (1) {
      mymotioncapture.RequestPositionandVelocity(
          _realtimevessel_first, _realtimevessel_second, _realtimevessel_third);
    }
  }
  // save motion data to sqlite database
  void save2database() {
    while (1) {
      if (MAXCONNECTION > 0) {
        Eigen::Matrix<double, 9, 1> position9DoF_socket =
            Eigen::Matrix<double, 9, 1>::Zero();
        position9DoF_socket << _realtimevessel_first.Position, 1, 2, 3;
        mydb.update_client_table(0, false, position9DoF_socket);
      }
      if (MAXCONNECTION > 1) {
        Eigen::Matrix<double, 9, 1> position9DoF_socket =
            Eigen::Matrix<double, 9, 1>::Zero();
        position9DoF_socket << _realtimevessel_second.Position, 1, 2, 3;
        mydb.update_client_table(1, false, position9DoF_socket);
      }
      if (MAXCONNECTION > 2) {
        Eigen::Matrix<double, 9, 1> position9DoF_socket =
            Eigen::Matrix<double, 9, 1>::Zero();
        position9DoF_socket << _realtimevessel_third.Position, 1, 2, 3;
        mydb.update_client_table(2, false, position9DoF_socket);
      }
    }
  }
  // send and receive data from the first client (K class-I)
  void ReceiveClient_first(int sockfd_index = 0) {
    // run through the existing connections looking for data to read
    Vectfd clients_fd = myserver.GetClientsFd();
    int sockfd = clients_fd(sockfd_index);
    int count_temp = 0;
    unsigned char recv_buf[MAXDATASIZE];  // buffer for client data (recieved)
    unsigned char send_buf[MAXDATASIZE];  // buffer for client data (sent)
    int nbytes = 0;
    while (1) {
      // handle data from a client
      nbytes = recv(sockfd, recv_buf, sizeof recv_buf, 0);
      if (nbytes == 0) {
        // connection closed
        close(sockfd);                           // bye!
        myserver.clearclients_fd(sockfd_index);  // remove from master set

        // output error information
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "thread-server: socket %d hung up\n", sockfd);
          fclose(myfile);
        } else
          printf("thread-server: socket %d hung up\n", sockfd);
      } else if (nbytes < 0) {
        // connection error
        close(sockfd);                           // bye!
        myserver.clearclients_fd(sockfd_index);  // remove from master set

        // output error information
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "error in recv!\n");
          fclose(myfile);
        } else
          perror("recv");
      } else {  // we got some data from a client

        if (verify_crc16_checksum(recv_buf, MAXDATASIZE)) {
          // real-time control and optimization for each client
          _controller_first.fullymanualcontroller(
              mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
              mygamepad.getGamepadZmoment(), _vessel_first,
              _realtimevessel_first, myfile);

          Eigen::Matrix<double, 9, 1> position9DoF_socket =
              Eigen::Matrix<double, 9, 1>::Zero();
          position9DoF_socket << _realtimevessel_first.tau,
              _realtimevessel_first.BalphaU, _realtimevessel_first.u;
          packfloat32Eigenvector(send_buf, position9DoF_socket,
                                 CLIENT_DATA_SIZE);
          append_crc16_checksum(send_buf, MAXDATASIZE);
          // send message to clients only
          if (send(sockfd, send_buf, MAXDATASIZE, 0) == -1) {
            // print error information
            if (FILEORNOT) {
              myfile = fopen(logsavepath.c_str(), "a+");
              fprintf(myfile, "error in send!\n");
              fclose(myfile);
            } else
              perror("send");
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } else {
          // send message to clients only
          if (send(sockfd, recv_buf, nbytes, 0) == -1) {
            // print error information
            if (FILEORNOT) {
              myfile = fopen(logsavepath.c_str(), "a+");
              fprintf(myfile, "error in send!\n");
              fclose(myfile);
            } else
              perror("send");
          }
        }
      }

      // temporary code
      ++count_temp;
      if (count_temp == 3000) break;
    }
  }

  // send and receive data from the second client (K class-II)
  void ReceiveClient_second(int sockfd_index = 1) {
    // run through the existing connections looking for data to read
    Vectfd clients_fd = myserver.GetClientsFd();
    int sockfd = clients_fd(sockfd_index);
    int count_temp = 0;
    unsigned char recv_buf[MAXDATASIZE];  // buffer for client data (recieved)
    unsigned char send_buf[MAXDATASIZE];  // buffer for client data (sent)
    int nbytes = 0;
    while (1) {
      // handle data from a client
      nbytes = recv(sockfd, recv_buf, sizeof recv_buf, 0);
      if (nbytes == 0) {
        // connection closed
        close(sockfd);                           // bye!
        myserver.clearclients_fd(sockfd_index);  // remove from master set

        // output error information
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "thread-server: socket %d hung up\n", sockfd);
          fclose(myfile);
        } else
          printf("thread-server: socket %d hung up\n", sockfd);
      } else if (nbytes < 0) {
        // connection error
        close(sockfd);                           // bye!
        myserver.clearclients_fd(sockfd_index);  // remove from master set

        // output error information
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "error in recv!\n");
          fclose(myfile);
        } else
          perror("recv");
      } else {  // we got some data from a client

        if (verify_crc16_checksum(recv_buf, MAXDATASIZE)) {
          // real-time control and optimization for each client
          _controller_second.fullymanualcontroller(
              mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
              mygamepad.getGamepadZmoment(), _vessel_second,
              _realtimevessel_second, myfile);

          Eigen::Matrix<double, 9, 1> position9DoF_socket =
              Eigen::Matrix<double, 9, 1>::Zero();
          position9DoF_socket << _realtimevessel_second.tau,
              _realtimevessel_second.BalphaU, _realtimevessel_second.u;
          packfloat32Eigenvector(send_buf, position9DoF_socket,
                                 CLIENT_DATA_SIZE);
          append_crc16_checksum(send_buf, MAXDATASIZE);
          // send message to clients only
          if (send(sockfd, send_buf, MAXDATASIZE, 0) == -1) {
            // print error information
            if (FILEORNOT) {
              myfile = fopen(logsavepath.c_str(), "a+");
              fprintf(myfile, "error in send!\n");
              fclose(myfile);
            } else
              perror("send");
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } else {
          // send message to clients only
          if (send(sockfd, recv_buf, nbytes, 0) == -1) {
            // print error information
            if (FILEORNOT) {
              myfile = fopen(logsavepath.c_str(), "a+");
              fprintf(myfile, "error in send!\n");
              fclose(myfile);
            } else
              perror("send");
          }
        }
      }

      // temporary code
      ++count_temp;
      if (count_temp == 3000) break;
    }
  }

  // send and receive data from the third client (X class)
  void ReceiveClient_third(int sockfd_index = 2) {
    // run through the existing connections looking for data to read
    Vectfd clients_fd = myserver.GetClientsFd();
    int sockfd = clients_fd(sockfd_index);
    int count_temp = 0;
    unsigned char recv_buf[MAXDATASIZE];  // buffer for client data (recieved)
    unsigned char send_buf[MAXDATASIZE];  // buffer for client data (sent)
    int nbytes = 0;
    while (1) {
      // handle data from a client
      nbytes = recv(sockfd, recv_buf, sizeof recv_buf, 0);
      if (nbytes == 0) {
        // connection closed
        close(sockfd);                           // bye!
        myserver.clearclients_fd(sockfd_index);  // remove from master set

        // output error information
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "thread-server: socket %d hung up\n", sockfd);
          fclose(myfile);
        } else
          printf("thread-server: socket %d hung up\n", sockfd);
      } else if (nbytes < 0) {
        // connection error
        close(sockfd);                           // bye!
        myserver.clearclients_fd(sockfd_index);  // remove from master set

        // output error information
        if (FILEORNOT) {
          myfile = fopen(logsavepath.c_str(), "a+");
          fprintf(myfile, "error in recv!\n");
          fclose(myfile);
        } else
          perror("recv");
      } else {  // we got some data from a client

        if (verify_crc16_checksum(recv_buf, MAXDATASIZE)) {
          // real-time control and optimization for each client
          _controller_third.fullymanualcontroller(
              mygamepad.getGamepadXforce(), mygamepad.getGamepadYforce(),
              mygamepad.getGamepadZmoment(), _vessel_third,
              _realtimevessel_third);

          Eigen::Matrix<double, 9, 1> position9DoF_socket =
              Eigen::Matrix<double, 9, 1>::Zero();
          position9DoF_socket << _realtimevessel_third.tau,
              _realtimevessel_third.BalphaU, _realtimevessel_third.u;
          packfloat32Eigenvector(send_buf, position9DoF_socket,
                                 CLIENT_DATA_SIZE);
          append_crc16_checksum(send_buf, MAXDATASIZE);
          // send message to clients only
          if (send(sockfd, send_buf, MAXDATASIZE, 0) == -1) {
            // print error information
            if (FILEORNOT) {
              myfile = fopen(logsavepath.c_str(), "a+");
              fprintf(myfile, "error in send!\n");
              fclose(myfile);
            } else
              perror("send");
          }

          std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } else {
          // send message to clients only
          if (send(sockfd, recv_buf, nbytes, 0) == -1) {
            // print error information
            if (FILEORNOT) {
              myfile = fopen(logsavepath.c_str(), "a+");
              fprintf(myfile, "error in send!\n");
              fclose(myfile);
            } else
              perror("send");
          }
        }
      }

      // temporary code
      ++count_temp;
      if (count_temp == 3000) break;
    }
  }
  // pack the float array
  void packfloat32Eigenvector(unsigned char *t_buf,
                              const Eigen::VectorXd &t_vector,
                              size_t t_arraylength) {
    for (size_t i = 0; i != t_arraylength; ++i) {
      // convert to IEEE 754
      unsigned long long int fhold = pack754_32(t_vector(i));
      packi32(t_buf, fhold);
      t_buf += 4;
    }
  }
  // convert Eigen Vector to c double array
  void convertEigen2doublearray(const Eigen::VectorXd &t_vector,
                                double *t_array, int t_arraylength) {
    for (int i = 0; i != t_arraylength; ++i) *(t_array + i) = t_vector(i);
  }
};

#endif /* _THREADLOOP_HPP_*/

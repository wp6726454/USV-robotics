/*
***********************************************************************
* motioncapture.hpp:
* real-time SDK for QTM, which could enable real-time motion capture.
* This header file can be read by C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _MOTIONCAPTURE_H
#define _MOTIONCAPTURE_H

#include <math.h>
#include <stdio.h>
#include <Eigen/Core>
#include <chrono>
#include <iostream>
#include <thread>
#include "constants.h"
#include "qtmOperations.h"
#include "realtimedata.h"

#define LATEST_SELECTABLE_MAJOR_VERSION 1
#define LATEST_SELECTABLE_MINOR_VERSION 14
typedef Eigen::Matrix<double, 6, MAXCONNECTION> Mat4Allclient_6DoF;

class motioncapture {
 public:
  explicit motioncapture(int index_default = 1)
      : acquisitionindex(index_default) {}
  ~motioncapture() {
    delete poOperations;
    delete poRTProtocol;
    delete poOutput;
    delete poInput;
  }

  int initializemotioncapture() {
    // connect to QTM server
    poInput = new CInput();

    // By default assume you want to connect to QTM at the same machine - just
    // for
    // testing
    char pServerAddr[32] = "192.168.253.1";
    // The base port (as entered in QTM, TCP/IP port number, in the RT output
    // tab
    // of the workspace options.
    unsigned short nBasePort = 22222;

    poOutput = new COutput();
    poRTProtocol = new CRTProtocol();
    poOperations = new COperations(poInput, poOutput, poRTProtocol);
    if (poRTProtocol->Connect(pServerAddr, nBasePort, 1, 14, false)) {
      char pVer[64];
      if (poRTProtocol->GetQTMVersion(pVer, sizeof(pVer))) {
        printf("Connected. %s.\n", pVer);
        return 0;
      }

    } else {
      printf("\nFailed to connect to QTM RT Server. %s\n\n",
             poRTProtocol->GetErrorString());
      delete poOperations;
      delete poRTProtocol;
      delete poOutput;
      delete poInput;
      return 1;
    }
  }
  // request and update the motion data, this function could be called by
  // multi-thread
  void RequestPositionandVelocity(realtimevessel_first &_realtimevessel_first,
                                  realtimevessel_second &_realtimevessel_second,
                                  realtimevessel_third &_realtimevessel_third) {
    poOperations->DataTransfer(_realtimevessel_first, _realtimevessel_second,
                               _realtimevessel_third);
    // if (MAXCONNECTION > 0) getcurrentmotion_first(_realtimevessel_first);
    // if (MAXCONNECTION > 1) getcurrentmotion_second(_realtimevessel_second);
    // if (MAXCONNECTION > 2) getcurrentmotion_third(_realtimevessel_third);
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

 private:
  // raw data

  // data acquisition successful or not
  int acquisitionindex;

  // QTM RT protocal

  CInput *poInput = NULL;
  COutput *poOutput = NULL;
  CRTProtocol *poRTProtocol = NULL;
  COperations *poOperations = NULL;

  /* QTMclient API */
  // get the current position and velocity of the first vessel (K class-I)
  void getcurrentmotion_first(realtimevessel_first &_realtimevessel_first) {
    _realtimevessel_first.Position.setRandom();
    _realtimevessel_first.Velocity.setRandom();
    _realtimevessel_first.Measurement.setRandom();
    static int angle = 0;
    _realtimevessel_first.Position(0) = 1.2 * sin(angle * M_PI / 180);
    ++angle;
  }
  // get the current position and velocity of the second vessel (K class-II)
  void getcurrentmotion_second(realtimevessel_second &_realtimevessel_second) {
    _realtimevessel_second.Position.setRandom();
    _realtimevessel_second.Velocity.setRandom();
    _realtimevessel_second.Measurement.setRandom();
  }
  // get the current position and velocity of the first vessel (X class)
  void getcurrentmotion_third(realtimevessel_third &_realtimevessel_third) {
    _realtimevessel_third.Position.setRandom();
    _realtimevessel_third.Velocity.setRandom();
    _realtimevessel_third.Measurement.setRandom();
  }
};

#endif /*_MOTIONCAPTURE_H*/

#ifndef QTMOUTPUT_H
#define QTMOUTPUT_H

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <Eigen/Core>
#include <algorithm>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "RTProtocol.h"
#include "constants.h"
#include "qtmInput.h"
#include "realtimedata.h"

const double max_velocity_u = 1;              // m/s
const double max_velocity_v = 0.5;            // m/s
const double max_velocity_orientation = 0.5;  // rad/s
const double qtm_max_position = 30000;        // mm

using T_BOOST_CLOCK =
    boost::date_time::microsec_clock<boost::posix_time::ptime>;

class CDataPacket;

class COutput {
 public:
  COutput();
  void HandleDataFrame(FILE* logfile, bool bLogMinimum,
                       CRTProtocol* poRTProtocol);
  void HandleDataFrame(FILE* logfile, CRTProtocol* poRTProtocol,
                       realtimevessel_first& _realtimevessel_first,
                       realtimevessel_second& _realtimevessel_second,
                       realtimevessel_third& _realtimevessel_third);
  void PrintTimingData();
  void ResetCounters();
  void Print6DOFSettings(CRTProtocol* poRTProtocol);

 private:
  struct Marker {
    unsigned int nX;
    unsigned int nY;
  };

  void PrintData6D(FILE* logfile, CRTPacket* poRTPacket,
                   CRTProtocol* poRTProtocol);
  void PrintData6DRes(FILE* logfile, CRTPacket* poRTPacket,
                      CRTProtocol* poRTProtocol);
  void PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                        CRTProtocol* poRTProtocol);
  void PrintData6DEuler(CRTPacket* poRTPacket, CRTProtocol* poRTProtocol,
                        realtimevessel_first& _realtimevessel_first);
  void PrintData6DEuler(CRTPacket* poRTPacket, CRTProtocol* poRTProtocol,
                        realtimevessel_first& _realtimevessel_first,
                        realtimevessel_second& _realtimevessel_second);
  void PrintData6DEuler(CRTPacket* poRTPacket, CRTProtocol* poRTProtocol,
                        realtimevessel_first& _realtimevessel_first,
                        realtimevessel_second& _realtimevessel_second,
                        realtimevessel_third& _realtimevessel_third);
  void PrintData6DEulerRes(FILE* logfile, CRTPacket* poRTPacket,
                           CRTProtocol* poRTProtocol);

  void updaterealtimevesseldata(Vector6d& _measurement, Vector6d& _position,
                                float _fX, float _fY, float _fZ, float _fAng1,
                                float _fAng2, float _fAng3);
  void resetmeasurement(Vector6d& _measurement, Vector6d& _position);
  // How many cameras can be measured noise on
  static const int mcnMaxCameras = 20;
  // How many markers can be measured noise on
  static const int mcnMaxMarkers = 20;
  // How many samples to calculate max noise upon
  static const int mcnUseSamples = 100;
  static const int mcnMaxNoise = 30;  // Max noise in subpixels to display

  char msDist[100];
  float* mfDist;
  int mn2DFrames;

  bool mbWriteLogFileHeader;

  boost::posix_time::ptime time_start;
  double mfLastScreenUpdateTime;
  Marker masPrev2DMarkers[mcnMaxCameras][mcnMaxMarkers];

  int mnMaxPlotYPos;
  bool mbOutputModeScrolling;

  double mfCurrentRecvTime;
  double mfLastRecvTime;
  double mfRecvTimeDiff;
  double mfMaxRecvTimeDiff;
  double mfMinRecvTimeDiff;
  double mfRecFreq;
  double mfCameraFreq;
  unsigned long long mnLastTimeStamp;
  unsigned int mnLastFrameNumber;
  unsigned int mnMissingFrames;
  unsigned int mnReceivedFrames;
  int mnFrameNumberDiff;
  unsigned int mnMaxFrameNumberDiff;
};

#endif  // OUTPUT_H
#include "../include/qtmOutput.h"

#define WRITE_ANALOG_HEADERS_TO_FILE

COutput::COutput()
    : mfDist(NULL),
      mbWriteLogFileHeader(true),
      time_start(T_BOOST_CLOCK::local_time()),
      mbOutputModeScrolling(false) {}

void COutput::HandleDataFrame(FILE* logfile, bool bLogMinimum,
                              CRTProtocol* poRTProtocol) {
  CRTPacket* poRTPacket;

  mnReceivedFrames++;

  if (poRTProtocol) {
    poRTPacket = poRTProtocol->GetRTPacket();
    if (poRTPacket->GetComponentCount() == 0 ||
        poRTPacket->GetType() != CRTPacket::PacketData) {
      return;
    }
  } else {
    return;
  }

  unsigned int nFrameNumber = poRTPacket->GetFrameNumber();
  unsigned long long nTimeStamp = poRTPacket->GetTimeStamp();

  if (nFrameNumber == 1 && mnLastFrameNumber != 0) {
    // Start from the beginning in case we are running rt from file
    ResetCounters();
    mnReceivedFrames = 1;
  }

  // Update packet receive time.
  boost::posix_time::ptime time_now(T_BOOST_CLOCK::local_time());
  boost::posix_time::time_duration t_d = time_now - time_start;

  mfCurrentRecvTime = (double)(t_d.total_milliseconds() / 1000.0);

  if (mnReceivedFrames > 1) {
    mnFrameNumberDiff = nFrameNumber - mnLastFrameNumber;

    if (mnFrameNumberDiff <= 0) {
      // Frame repeated (should never happen).
      ResetCounters();
      mnReceivedFrames = 1;
    } else {
      // New frame received.
      mfCameraFreq =
          (mnFrameNumberDiff * 1000000.0) / (nTimeStamp - mnLastTimeStamp);
      mnMaxFrameNumberDiff =
          std::max((unsigned int)mnFrameNumberDiff, mnMaxFrameNumberDiff);
      mfRecvTimeDiff = mfCurrentRecvTime - mfLastRecvTime;
      mfMaxRecvTimeDiff = std::max(mfRecvTimeDiff, mfMaxRecvTimeDiff);
      mfMinRecvTimeDiff = std::min(mfRecvTimeDiff, mfMinRecvTimeDiff);
      mfRecFreq = 1.0 / mfRecvTimeDiff;

      if (mnFrameNumberDiff > 1) {
        mnMissingFrames += mnFrameNumberDiff - 1;
        printf("Missing %d frame%s. Frame number: %d", mnFrameNumberDiff,
               (mnFrameNumberDiff == 1) ? "" : "s", mnLastFrameNumber + 1);
        printf(" to %d\n\n", nFrameNumber - 1);
      }
    }
  }
  mnLastTimeStamp = nTimeStamp;
  mnLastFrameNumber = nFrameNumber;
  mfLastRecvTime = mfCurrentRecvTime;

  if (mfLastScreenUpdateTime == 0.0 ||
      (mfCurrentRecvTime - mfLastScreenUpdateTime) > 0.1) {
    mfLastScreenUpdateTime = mfCurrentRecvTime;
  } else {
    return;
  }

  PrintData6D(logfile, poRTPacket, poRTProtocol);
  PrintData6DRes(logfile, poRTPacket, poRTProtocol);
  PrintData6DEuler(logfile, poRTPacket, poRTProtocol);
  PrintData6DEulerRes(logfile, poRTPacket, poRTProtocol);

}  // PrintData

void COutput::HandleDataFrame(FILE* logfile, CRTProtocol* poRTProtocol,
                              realtimevessel_first& _realtimevessel_first,
                              realtimevessel_second& _realtimevessel_second,
                              realtimevessel_third& _realtimevessel_third) {
  CRTPacket* poRTPacket;

  if (poRTProtocol) {
    poRTPacket = poRTProtocol->GetRTPacket();
    if (poRTPacket->GetComponentCount() == 0 ||
        poRTPacket->GetType() != CRTPacket::PacketData) {
      return;
    }
  } else {
    return;
  }

  if (MAXCONNECTION == 1)
    PrintData6DEuler(logfile, poRTPacket, poRTProtocol, _realtimevessel_first);
  else if (MAXCONNECTION == 2)
    PrintData6DEuler(logfile, poRTPacket, poRTProtocol, _realtimevessel_first,
                     _realtimevessel_second);
  else if (MAXCONNECTION == 3)
    PrintData6DEuler(logfile, poRTPacket, poRTProtocol, _realtimevessel_first,
                     _realtimevessel_second, _realtimevessel_third);
}  // OutputData

void COutput::PrintData6D(FILE* logfile, CRTPacket* poRTPacket,
                          CRTProtocol* poRTProtocol) {
  float fX, fY, fZ;
  float afRotMatrix[9];

  if (poRTPacket->GetComponentSize(CRTPacket::Component6d)) {
    unsigned int nCount = poRTPacket->Get6DOFBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "------------------ 6 DOF -----------------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFBody(i, fX, fY, fZ, afRotMatrix);

        fprintf(
            logfile,
            "%15s : %f\t%f\t%f\t -    %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n",
            label, fX, fY, fZ, afRotMatrix[0], afRotMatrix[1], afRotMatrix[2],
            afRotMatrix[3], afRotMatrix[4], afRotMatrix[5], afRotMatrix[6],
            afRotMatrix[7], afRotMatrix[8]);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6D

void COutput::PrintData6DRes(FILE* logfile, CRTPacket* poRTPacket,
                             CRTProtocol* poRTProtocol) {
  float fX, fY, fZ, fResidual;
  float afRotMatrix[9];

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dRes)) {
    unsigned int nCount = poRTPacket->Get6DOFResidualBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "------------- 6 DOF Residual -------------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFResidualBody(i, fX, fY, fZ, afRotMatrix, fResidual);

        fprintf(logfile,
                "%15s : %f\t%f\t%f\t -    %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t "
                "-   %f\n",
                label, fX, fY, fZ, afRotMatrix[0], afRotMatrix[1],
                afRotMatrix[2], afRotMatrix[3], afRotMatrix[4], afRotMatrix[5],
                afRotMatrix[6], afRotMatrix[7], afRotMatrix[8], fResidual);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6DRes

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "--------------- 6 DOF Euler --------------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFEulerBody(i, fX, fY, fZ, fAng1, fAng2, fAng3);

        fprintf(logfile, "%15s : %f\t%f\t%f\t -   %f\t%f\t%f\n", label, fX, fY,
                fZ, fAng1, fAng2, fAng3);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6DEuler

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;  // mm, mm, mm, deg, deg, deg (QTM)

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();
    // determine if we have rigid body
    if (nCount > 0) {
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      if ((abs(fX) < qtm_max_position) && (abs(fY) < qtm_max_position))
        updaterealtimevesseldata(_realtimevessel_first.Measurement,
                                 _realtimevessel_first.Position, fX, fY, fZ,
                                 fAng1, fAng2, fAng3);
      else {
        resetmeasurement(_realtimevessel_first.Measurement,
                         _realtimevessel_first.Position);

        logfile = fopen(logsavepath.c_str(), "a+");
        fprintf(logfile, "First: motion data out of range or NaN!\n");
        fclose(logfile);
      }
    } else {
      resetmeasurement(_realtimevessel_first.Measurement,
                       _realtimevessel_first.Position);

      logfile = fopen(logsavepath.c_str(), "a+");
      fprintf(logfile, "No rigid body found!\n");
      fclose(logfile);
    }
  }
}  // PrintData6DEuler (the first vessel)

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;  // mm, mm, mm, deg, deg, deg (QTM)
  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      // the first vessel
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      if ((abs(fX) < qtm_max_position) && (abs(fY) < qtm_max_position))
        updaterealtimevesseldata(_realtimevessel_first.Measurement,
                                 _realtimevessel_first.Position, fX, fY, fZ,
                                 fAng1, fAng2, fAng3);
      else {
        resetmeasurement(_realtimevessel_first.Measurement,
                         _realtimevessel_first.Position);
        logfile = fopen(logsavepath.c_str(), "a+");
        fprintf(logfile, "First: motion data out of range or NaN!\n");
        fclose(logfile);
      }
      // the second vessel
      poRTPacket->Get6DOFEulerBody(1, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      if ((abs(fX) < qtm_max_position) && (abs(fY) < qtm_max_position))
        updaterealtimevesseldata(_realtimevessel_second.Measurement,
                                 _realtimevessel_second.Position, fX, fY, fZ,
                                 fAng1, fAng2, fAng3);
      else {
        resetmeasurement(_realtimevessel_second.Measurement,
                         _realtimevessel_second.Position);
        logfile = fopen(logsavepath.c_str(), "a+");
        fprintf(logfile, "Second: motion data out of range or NaN!\n");
        fclose(logfile);
      }

    } else {
      resetmeasurement(_realtimevessel_first.Measurement,
                       _realtimevessel_first.Position);
      resetmeasurement(_realtimevessel_second.Measurement,
                       _realtimevessel_second.Position);

      logfile = fopen(logsavepath.c_str(), "a+");
      fprintf(logfile, "No rigid body found!\n");
      fclose(logfile);
    }
  }
}  // PrintData6DEuler (the first and second vessel)

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second,
                               realtimevessel_third& _realtimevessel_third) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;  // mm, mm, mm, deg, deg, deg (QTM)
  double m_fx, m_fy, rad_orientation, raw_u, raw_v, raw_r;
  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      // the first vessel
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      if ((abs(fX) < qtm_max_position) && (abs(fY) < qtm_max_position))
        updaterealtimevesseldata(_realtimevessel_first.Measurement,
                                 _realtimevessel_first.Position, fX, fY, fZ,
                                 fAng1, fAng2, fAng3);
      else {
        resetmeasurement(_realtimevessel_first.Measurement,
                         _realtimevessel_first.Position);
        logfile = fopen(logsavepath.c_str(), "a+");
        fprintf(logfile, "First: motion data out of range or NaN!\n");
        fclose(logfile);
      }
      // the second vessel
      poRTPacket->Get6DOFEulerBody(1, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      if ((abs(fX) < qtm_max_position) && (abs(fY) < qtm_max_position))
        updaterealtimevesseldata(_realtimevessel_second.Measurement,
                                 _realtimevessel_second.Position, fX, fY, fZ,
                                 fAng1, fAng2, fAng3);
      else {
        resetmeasurement(_realtimevessel_second.Measurement,
                         _realtimevessel_second.Position);
        logfile = fopen(logsavepath.c_str(), "a+");
        fprintf(logfile, "Second: motion data out of range or NaN!\n");
        fclose(logfile);
      }
      // the third vessel
      poRTPacket->Get6DOFEulerBody(2, fX, fY, fZ, fAng1, fAng2, fAng3);
      // determine if the measured data is out of range or NaN
      if ((abs(fX) < qtm_max_position) && (abs(fY) < qtm_max_position))
        updaterealtimevesseldata(_realtimevessel_third.Measurement,
                                 _realtimevessel_third.Position, fX, fY, fZ,
                                 fAng1, fAng2, fAng3);
      else {
        resetmeasurement(_realtimevessel_third.Measurement,
                         _realtimevessel_third.Position);
        logfile = fopen(logsavepath.c_str(), "a+");
        fprintf(logfile, "Third: motion data out of range or NaN!\n");
        fclose(logfile);
      }

    } else {
      resetmeasurement(_realtimevessel_first.Measurement,
                       _realtimevessel_first.Position);
      resetmeasurement(_realtimevessel_second.Measurement,
                       _realtimevessel_second.Position);
      resetmeasurement(_realtimevessel_third.Measurement,
                       _realtimevessel_third.Position);

      logfile = fopen(logsavepath.c_str(), "a+");
      fprintf(logfile, "No rigid body found!\n");
      fclose(logfile);
    }
  }
}  // PrintData6DEuler (the first/second/third vessel)

void COutput::PrintData6DEulerRes(FILE* logfile, CRTPacket* poRTPacket,
                                  CRTProtocol* poRTProtocol) {
  float fX, fY, fZ, fAng1, fAng2, fAng3, fResidual;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEulerRes)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerResidualBodyCount();

    if (nCount > 0) {
      fprintf(logfile, "---------- 6 DOF Euler Residual ----------\n");
      for (unsigned int i = 0; i < nCount; i++) {
        char* label = (char*)poRTProtocol->Get6DOFBodyName(i);
        char emptyString[] = "";
        if (label == NULL) {
          label = emptyString;
        }
        poRTPacket->Get6DOFEulerResidualBody(i, fX, fY, fZ, fAng1, fAng2, fAng3,
                                             fResidual);

        fprintf(logfile, "%15s : %f\t%f\t%f\t -   %f\t%f\t%f\t -    %f\n",
                label, fX, fY, fZ, fAng1, fAng2, fAng3, fResidual);
      }
      fprintf(logfile, "\n");
    }
  }
}  // PrintData6DEulerRes

void COutput::PrintTimingData() {
  printf("\n\nReceived %d data frames in %f seconds\n\n", mnReceivedFrames,
         mfCurrentRecvTime);
  printf("Average receive frequency = %.1f\n",
         mnReceivedFrames / mfCurrentRecvTime);
  printf("Camera frequency = %.1f Hz\n", mfCameraFreq);
  printf("Missed frames = %d\n", mnMissingFrames);
  printf("Max frame receive time diff = %f ms\n", mfMaxRecvTimeDiff);
  printf("Min frame receive time diff = %f ms\n\n", mfMinRecvTimeDiff);
}  // PrintTimingData

void COutput::ResetCounters() {
  // Reset statistic counters
  mfRecvTimeDiff = 0.0;
  mfMaxRecvTimeDiff = 0.0;
  mfMinRecvTimeDiff = 100000.0;
  mfLastRecvTime = 0.0;
  mfRecFreq = 0.0;
  mfLastScreenUpdateTime = 0.0;
  mnLastFrameNumber = 0xffffffff;
  mnMaxFrameNumberDiff = 0;
  mnMissingFrames = 0;
  mnReceivedFrames = 0;
  mnFrameNumberDiff = 0;
  mnMaxFrameNumberDiff = 0;
  mnLastTimeStamp = 0;

}  // ResetCounters

void COutput::updaterealtimevesseldata(Vector6d& _measurement,
                                       Vector6d& _position, float _fX,
                                       float _fY, float _fZ, float _fAng1,
                                       float _fAng2, float _fAng3) {
  // the first vessel
  double m_fx = _fX / 1000;
  double m_fy = _fY / 1000;
  double rad_orientation = _fAng3 * M_PI / 180;
  _position(0) = m_fx;
  _position(1) = m_fy;
  _position(2) = _fZ / 1000;
  _position(3) = _fAng1;
  _position(4) = _fAng2;
  _position(5) = _fAng3;

  double raw_u = (m_fx - _measurement(0)) / sample_time;
  double raw_v = (m_fy - _measurement(1)) / sample_time;
  double raw_r = (rad_orientation - _measurement(2)) / sample_time;
  if ((abs(raw_u) > max_velocity_u) || (abs(raw_v) > max_velocity_v) ||
      (abs(raw_r) > max_velocity_orientation)) {
    _measurement(3) = 0;
    _measurement(4) = 0;
    _measurement(5) = 0;
  } else {
    _measurement(3) = raw_u;
    _measurement(4) = raw_v;
    _measurement(5) = raw_r;
  }

  _measurement(0) = m_fx;
  _measurement(1) = m_fy;
  _measurement(2) = rad_orientation;
  // the second vessel
}

void COutput::resetmeasurement(Vector6d& _measurement, Vector6d& _position) {
  _measurement.setZero();
  _position.setZero();
}

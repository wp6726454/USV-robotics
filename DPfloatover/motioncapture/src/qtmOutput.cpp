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
  float fX, fY, fZ, fAng1, fAng2, fAng3;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      _realtimevessel_first.Measurement(0) = fX;
      _realtimevessel_first.Measurement(1) = fY;
      _realtimevessel_first.Measurement(2) = fZ;
      _realtimevessel_first.Measurement(3) = fAng1;
      _realtimevessel_first.Measurement(4) = fAng2;
      _realtimevessel_first.Measurement(5) = fAng3;
    }
  }
}  // PrintData6DEuler (the first vessel)

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      // the first vessel
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // the first vessel
      _realtimevessel_first.Measurement(0) = fX;
      _realtimevessel_first.Measurement(1) = fY;
      _realtimevessel_first.Measurement(2) = fZ;
      _realtimevessel_first.Measurement(3) = fAng1;
      _realtimevessel_first.Measurement(4) = fAng2;
      _realtimevessel_first.Measurement(5) = fAng3;
      // the second vessel
      poRTPacket->Get6DOFEulerBody(1, fX, fY, fZ, fAng1, fAng2, fAng3);
      // the second vessel
      _realtimevessel_second.Measurement(0) = fX;
      _realtimevessel_second.Measurement(1) = fY;
      _realtimevessel_second.Measurement(2) = fZ;
      _realtimevessel_second.Measurement(3) = fAng1;
      _realtimevessel_second.Measurement(4) = fAng2;
      _realtimevessel_second.Measurement(5) = fAng3;
    }
  }
}  // PrintData6DEuler (the first and second vessel)

void COutput::PrintData6DEuler(FILE* logfile, CRTPacket* poRTPacket,
                               CRTProtocol* poRTProtocol,
                               realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second,
                               realtimevessel_third& _realtimevessel_third) {
  float fX, fY, fZ, fAng1, fAng2, fAng3;

  if (poRTPacket->GetComponentSize(CRTPacket::Component6dEuler)) {
    unsigned int nCount = poRTPacket->Get6DOFEulerBodyCount();

    if (nCount > 0) {
      // the first vessel
      poRTPacket->Get6DOFEulerBody(0, fX, fY, fZ, fAng1, fAng2, fAng3);
      // the first vessel
      _realtimevessel_first.Measurement(0) = fX;
      _realtimevessel_first.Measurement(1) = fY;
      _realtimevessel_first.Measurement(2) = fZ;
      _realtimevessel_first.Measurement(3) = fAng1;
      _realtimevessel_first.Measurement(4) = fAng2;
      _realtimevessel_first.Measurement(5) = fAng3;
      // the second vessel
      poRTPacket->Get6DOFEulerBody(1, fX, fY, fZ, fAng1, fAng2, fAng3);
      // the second vessel
      _realtimevessel_second.Measurement(0) = fX;
      _realtimevessel_second.Measurement(1) = fY;
      _realtimevessel_second.Measurement(2) = fZ;
      _realtimevessel_second.Measurement(3) = fAng1;
      _realtimevessel_second.Measurement(4) = fAng2;
      _realtimevessel_second.Measurement(5) = fAng3;
      // the third vessel
      poRTPacket->Get6DOFEulerBody(2, fX, fY, fZ, fAng1, fAng2, fAng3);
      // the third vessel
      _realtimevessel_third.Measurement(0) = fX;
      _realtimevessel_third.Measurement(1) = fY;
      _realtimevessel_third.Measurement(2) = fZ;
      _realtimevessel_third.Measurement(3) = fAng1;
      _realtimevessel_third.Measurement(4) = fAng2;
      _realtimevessel_third.Measurement(5) = fAng3;
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

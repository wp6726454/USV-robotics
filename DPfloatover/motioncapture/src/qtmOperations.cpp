#include "../include/qtmOperations.h"

COperations::COperations(CInput* poInput, COutput* poOutput,
                         CRTProtocol* poRTProtocol) {
  mpoInput = poInput;
  mpoOutput = poOutput;
  mpoRTProtocol = poRTProtocol;
}

void COperations::DataTransfer(realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second,
                               realtimevessel_third& _realtimevessel_third) {
  CRTPacket::EPacketType ePacketType;
  unsigned int nComponentType;
  CRTProtocol::EStreamRate eStreamRate;
  int nRateArgument;
  FILE* logfile = NULL;

  mpoOutput->ResetCounters();

  // nComponentType = CRTProtocol::cComponent2d;
  nComponentType = CRTProtocol::cComponent6dEuler;
  eStreamRate = CRTProtocol::RateAllFrames;
  nRateArgument = 0;

  logfile = stdout;

  bool bDataAvailable;

  if ((nComponentType & CRTProtocol::cComponent6d) |
      (nComponentType & CRTProtocol::cComponent6dRes) |
      (nComponentType & CRTProtocol::cComponent6dEuler) |
      (nComponentType & CRTProtocol::cComponent6dEulerRes)) {
    mpoRTProtocol->Read6DOFSettings(bDataAvailable);
  }

  mpoRTProtocol->StreamFrames(eStreamRate, nRateArgument, nComponentType);
  // Main data read loop

  while (1) {
    if (mpoRTProtocol->ReceiveRTPacket(ePacketType, true) > 0) {
      switch (ePacketType) {
        case CRTPacket::PacketError:  // sHeader.nType 0 indicates an error
        {
          CRTPacket* poRTPacket = mpoRTProtocol->GetRTPacket();
          fprintf(stderr, "Error at StreamFrames: %s\n",
                  poRTPacket->GetErrorString());
          break;
        }
        case CRTPacket::PacketData:  // Data received
          mpoOutput->HandleDataFrame(
              logfile, mpoRTProtocol, _realtimevessel_first,
              _realtimevessel_second, _realtimevessel_third);
          std::cout << _realtimevessel_first.Measurement << std::endl;
          break;
        case CRTPacket::PacketNoMoreData:  // No more data
          break;
        default:
          break;
      }
    }
  }
  mpoRTProtocol->StreamFramesStop();

}  // DataTransfer

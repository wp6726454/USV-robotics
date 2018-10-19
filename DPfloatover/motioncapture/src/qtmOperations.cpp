#include "../include/qtmOperations.h"

COperations::COperations(CInput* poInput, COutput* poOutput,
                         CRTProtocol* poRTProtocol) {
  mpoInput = poInput;
  mpoOutput = poOutput;
  mpoRTProtocol = poRTProtocol;
}

void COperations::DataTransfer(realtimevessel_first& _realtimevessel_first,
                               realtimevessel_second& _realtimevessel_second,
                               realtimevessel_third& _realtimevessel_third,
                               FILE* _file) {
  CRTPacket::EPacketType ePacketType;
  unsigned int nComponentType;
  CRTProtocol::EStreamRate eStreamRate;
  int nRateArgument;

  mpoOutput->ResetCounters();

  // nComponentType = CRTProtocol::cComponent2d;
  nComponentType = CRTProtocol::cComponent6dEuler;
  eStreamRate = CRTProtocol::RateAllFrames;
  nRateArgument = 0;

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
          _file = fopen(logsavepath.c_str(), "a+");
          fprintf(_file, "Error at StreamFrames: %s\n",
                  poRTPacket->GetErrorString());
          fclose(_file);
          break;
        }
        case CRTPacket::PacketData:  // Data received
          mpoOutput->HandleDataFrame(
              _file, mpoRTProtocol, _realtimevessel_first,
              _realtimevessel_second, _realtimevessel_third);
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

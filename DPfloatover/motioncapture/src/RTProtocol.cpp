#include "../include/RTProtocol.h"
#include <string.h>

namespace {

inline void ToLower(std::string& str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

inline void RemoveInvalidChars(std::string& str) {
  auto isInvalidChar = [](int c) -> int {
    // iscntrl: control codes (NUL, etc.), '\t', '\n', '\v', '\f', '\r',
    // backspace (DEL) isspace: some common checks but also 0x20 (SPACE) return
    // != 0 --> invalid char
    return std::iscntrl(c) + std::isspace(c);
  };
  str.erase(std::remove_if(str.begin(), str.end(), isInvalidChar), str.end());
}

}  // namespace

CRTProtocol::CRTProtocol() {
  mpoNetwork = new CNetwork();
  mpoRTPacket = NULL;
  meLastEvent = CRTPacket::EventCaptureStopped;
  meState = CRTPacket::EventCaptureStopped;
  mnMajorVersion = 1;
  mnMinorVersion = 0;
  mbBigEndian = false;
  maErrorStr[0] = 0;
  mnBroadcastPort = 0;
  mpFileBuffer = NULL;
}  // CRTProtocol

CRTProtocol::~CRTProtocol() {
  if (mpoNetwork) {
    delete mpoNetwork;
    mpoNetwork = NULL;
  }
  if (mpoRTPacket) {
    delete mpoRTPacket;
    mpoRTPacket = NULL;
  }
}  // ~CRTProtocol

bool CRTProtocol::Connect(char* pServerAddr, unsigned short nPort,
                          int nMajorVersion, int nMinorVersion,
                          bool bBigEndian) {
  CRTPacket::EPacketType eType;
  char tTemp[100];
  char pResponseStr[256];

  mbBigEndian = bBigEndian;

  mnMajorVersion = 1;
  if ((nMajorVersion == 1) && (nMinorVersion == 0)) {
    mnMinorVersion = 0;
  } else {
    mnMinorVersion = 1;
    if (mbBigEndian) {
      nPort += 2;
    } else {
      nPort += 1;
    }
  }

  if (mpoRTPacket) {
    delete mpoRTPacket;
  }
  mpoRTPacket = new CRTPacket(nMajorVersion, nMinorVersion, bBigEndian);

  if (mpoRTPacket == NULL) {
    snprintf(maErrorStr, sizeof(maErrorStr), "Could not allocate data packet.");
    return false;
  }

  if (mpoNetwork->Connect(pServerAddr, nPort)) {
    // Welcome message
    if (ReceiveRTPacket(eType, true)) {
      if (eType == CRTPacket::PacketError) {
        strcpy(maErrorStr, mpoRTPacket->GetErrorString());
        Disconnect();
        return false;
      }
      if (eType == CRTPacket::PacketCommand) {
        if (strcmp("QTM RT Interface connected",
                   mpoRTPacket->GetCommandString()) == 0) {
          // Set protocol version
          if (SetVersion(nMajorVersion, nMinorVersion)) {
            // Set byte order.
            // Unless we use protocol version 1.0, we have set the byte order by
            // selecting the correct port.

            if ((mnMajorVersion == 1) && (mnMinorVersion == 0)) {
              if (mbBigEndian) {
                snprintf(tTemp, sizeof(tTemp), "ByteOrder BigEndian");
              } else {
                snprintf(tTemp, sizeof(tTemp), "ByteOrder LittleEndian");
              }

              if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
                return true;
              } else {
                snprintf(maErrorStr, sizeof(maErrorStr),
                         "Set byte order failed.");
              }
            } else {
              GetState(meState, true);
              return true;
            }
          }
          Disconnect();
          return false;
        }
      }
    }
    snprintf(maErrorStr, sizeof(maErrorStr),
             "Missing QTM server welcome message.");
    Disconnect();
  } else {
    if (mpoNetwork->GetError() == 10061) {
      snprintf(maErrorStr, sizeof(maErrorStr),
               "Check if QTM is running on target machine.");
    } else {
      snprintf(maErrorStr, sizeof(maErrorStr), "%s",
               mpoNetwork->GetErrorString());
    }
  }

  return false;
}  // Connect

void CRTProtocol::Disconnect() {
  mpoNetwork->Disconnect();
  mnBroadcastPort = 0;
  if (mpoRTPacket) {
    delete mpoRTPacket;
    mpoRTPacket = NULL;
  }
}  // Disconnect

bool CRTProtocol::Connected() { return mpoNetwork->Connected(); }

bool CRTProtocol::SetVersion(int nMajorVersion, int nMinorVersion) {
  char tTemp[256];
  char pResponseStr[256];

  snprintf(tTemp, sizeof(tTemp), "Version %u.%u", nMajorVersion, nMinorVersion);

  if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
    snprintf(tTemp, sizeof(tTemp), "Version set to %u.%u", nMajorVersion,
             nMinorVersion);

    if (strcmp(pResponseStr, tTemp) == 0) {
      mnMajorVersion = nMajorVersion;
      mnMinorVersion = nMinorVersion;
      mpoRTPacket->SetVersion(mnMajorVersion, mnMinorVersion);
      return true;
    }

    if (pResponseStr) {
      snprintf(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
    } else {
      snprintf(maErrorStr, sizeof(maErrorStr), "Set Version failed.");
    }
  } else {
    strcpy(tTemp, maErrorStr);
    snprintf(maErrorStr, sizeof(maErrorStr), "Send Version failed. %s.", tTemp);
  }
  return false;
}

bool CRTProtocol::GetVersion(unsigned int& nMajorVersion,
                             unsigned int& nMinorVersion) {
  if (!Connected()) {
    return false;
  }

  nMajorVersion = mnMajorVersion;
  nMinorVersion = mnMinorVersion;

  return true;
}

bool CRTProtocol::GetQTMVersion(char* pVersion, unsigned int nVersionLen) {
  if (SendCommand("QTMVersion", pVersion, nVersionLen)) {
    return true;
  }
  snprintf(maErrorStr, sizeof(maErrorStr), "Get QTM Version failed.");
  return false;
}

bool CRTProtocol::StreamFrames(EStreamRate eRate, unsigned int nRateArg,
                               unsigned int nComponentType) {
  char pCommandStr[256];

  if (eRate == RateFrequencyDivisor) {
    snprintf(pCommandStr, sizeof(pCommandStr),
             "StreamFrames FrequencyDivisor:%d ", nRateArg);
  } else if (eRate == RateFrequency) {
    snprintf(pCommandStr, sizeof(pCommandStr), "StreamFrames Frequency:%d ",
             nRateArg);
  } else if (eRate == RateAllFrames) {
    snprintf(pCommandStr, sizeof(pCommandStr), "StreamFrames AllFrames ");
  } else {
    snprintf(maErrorStr, sizeof(maErrorStr), "No valid rate.");
    return false;
  }

  std::string::size_type nCommandStrSize = strlen(pCommandStr);

  if (GetComponentString(pCommandStr + (int)nCommandStrSize,
                         sizeof(pCommandStr) - (int)nCommandStrSize,
                         nComponentType)) {
    if (SendCommand(pCommandStr)) {
      return true;
    }
    snprintf(maErrorStr, sizeof(maErrorStr), "StreamFrames failed.");
  } else {
    snprintf(maErrorStr, sizeof(maErrorStr), "DataComponent missing.");
  }

  return false;
}

bool CRTProtocol::StreamFramesStop() {
  if (SendCommand("StreamFrames Stop")) {
    return true;
  }
  snprintf(maErrorStr, sizeof(maErrorStr), "StreamFrames Stop failed.");
  return false;
}

bool CRTProtocol::GetState(CRTPacket::EEvent& eEvent, bool bUpdate,
                           int nTimeout) {
  CRTPacket::EPacketType eType;

  if (bUpdate) {
    bool result;
    if (mnMajorVersion > 1 || mnMinorVersion > 9) {
      result = SendCommand("GetState");
    } else {
      result = SendCommand("GetLastEvent");
    }
    if (result) {
      int nReceived;
      do {
        nReceived = ReceiveRTPacket(eType, false, nTimeout);
        if (nReceived > 0) {
          if (mpoRTPacket->GetEvent(eEvent)) {
            return true;
          }
        }
      } while (nReceived > 0);
    }
    snprintf(maErrorStr, sizeof(maErrorStr), "GetLastEvent failed.");
  } else {
    eEvent = meState;
    return true;
  }
  return false;
}

bool CRTProtocol::GetComponentString(char* pComponentStr, int nComponentStrLen,
                                     unsigned int nComponentType) {
  pComponentStr[0] = 0;

  if (nComponentType == cComponentAll) {
    strcat(pComponentStr, "All");
    return true;
  }

  if (nComponentType & cComponent2d) {
    strcat(pComponentStr, "2D ");
  }
  if (nComponentType & cComponent2dLin) {
    strcat(pComponentStr, "2DLin ");
  }
  if (nComponentType & cComponent3d) {
    strcat(pComponentStr, "3D ");
  }
  if (nComponentType & cComponent3dRes) {
    strcat(pComponentStr, "3DRes ");
  }
  if (nComponentType & cComponent3dNoLabels) {
    strcat(pComponentStr, "3DNoLabels ");
  }
  if (nComponentType & cComponent3dNoLabelsRes) {
    strcat(pComponentStr, "3DNoLabelsRes ");
  }
  if (nComponentType & cComponent6d) {
    strcat(pComponentStr, "6D ");
  }
  if (nComponentType & cComponent6dRes) {
    strcat(pComponentStr, "6DRes ");
  }
  if (nComponentType & cComponent6dEuler) {
    strcat(pComponentStr, "6DEuler ");
  }
  if (nComponentType & cComponent6dEulerRes) {
    strcat(pComponentStr, "6DEulerRes ");
  }

  return (pComponentStr[0] != 0);
}

int CRTProtocol::ReceiveRTPacket(CRTPacket::EPacketType& eType,
                                 bool bSkipEvents, int nTimeout) {
  int nRecved = 0;
  unsigned int nRecvedTotal = 0;
  unsigned int nFrameSize;

  eType = CRTPacket::PacketNone;

  do {
    nRecved = 0;
    nRecvedTotal = 0;

    // nRecved =
    //     mpoNetwork->Receive(maDataBuff, sizeof(maDataBuff), true, nTimeout);
    nRecved = mpoNetwork->Receive(maDataBuff, sizeof(maDataBuff), true);
    if (nRecved == 0) {
      return 0;  // Receive timeout
    }
    if ((nRecved > 0) && ((unsigned)nRecved < sizeof(int) * 2)) {
      // QTM header not received.
      snprintf(maErrorStr, sizeof(maErrorStr), "Couldn't read header bytes.");
      return -1;
    }
    if (nRecved == -1) {
      snprintf(maErrorStr, sizeof(maErrorStr), "Socket Error.");
      return -1;
    }
    if (nRecved == -2) {
      snprintf(maErrorStr, sizeof(maErrorStr), "Disconnected from server.");
      return -1;
    }
    nRecvedTotal += nRecved;

    bool bBigEndian =
        (mbBigEndian || (mnMajorVersion == 1 && mnMinorVersion == 0));
    nFrameSize = mpoRTPacket->GetSize(maDataBuff, bBigEndian);
    eType = mpoRTPacket->GetType(maDataBuff, bBigEndian);

    unsigned int nReadSize;

    if (nFrameSize > sizeof(maDataBuff)) {
      snprintf(maErrorStr, sizeof(maErrorStr), "Receive buffer overflow.");
      return -1;
    }

    // Receive more data until we have read the whole packet
    while (nRecvedTotal < nFrameSize) {
      // As long as we haven't received enough data, wait for more

      // nRecved = mpoNetwork->Receive(&(maDataBuff[nRecvedTotal]),
      //                               nFrameSize - nRecvedTotal, false,
      //                               nTimeout);
      nRecved = mpoNetwork->Receive(&(maDataBuff[nRecvedTotal]),
                                    nFrameSize - nRecvedTotal, false);
      if (nRecved <= 0) {
        if (nRecved == -2) {
          snprintf(maErrorStr, sizeof(maErrorStr), "Disconnected from server.");
        } else {
          snprintf(maErrorStr, sizeof(maErrorStr), "Socket Error.");
        }
        return -1;
      }
      nRecvedTotal += nRecved;
    }

    mpoRTPacket->SetData(maDataBuff);
    if (mpoRTPacket->GetEvent(
            meLastEvent))  // Update last event if there is an event
    {
      if (meLastEvent != CRTPacket::EventCameraSettingsChanged) {
        meState = meLastEvent;
      }
    }
  } while (bSkipEvents && eType == CRTPacket::PacketEvent);

  if (nRecvedTotal == nFrameSize) {
    return nRecvedTotal;
  }
  snprintf(maErrorStr, sizeof(maErrorStr), "Packet truncated.");

  return -1;
}  // ReceiveRTPacket

CRTPacket* CRTProtocol::GetRTPacket() { return mpoRTPacket; };

bool CRTProtocol::Read6DOFSettings(bool& bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;

  bDataAvailable = false;

  mvs6DOFBodySettings.clear();

  if (!SendCommand("GetParameters 6D")) {
    snprintf(maErrorStr, sizeof(maErrorStr), "GetParameters 6D failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy(pTmpStr, maErrorStr);
    snprintf(maErrorStr, sizeof(maErrorStr),
             "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      snprintf(maErrorStr, sizeof(maErrorStr), "%s.",
               mpoRTPacket->GetErrorString());
    } else {
      snprintf(maErrorStr, sizeof(maErrorStr),
               "GetParameters 6DOF returned wrong packet type. Got type %d "
               "expected type 2.",
               eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());

  //
  // Read 6DOF bodies
  //
  if (!oXML.FindChildElem("The_6D")) {
    return true;  // NO 6DOF data available.
  }
  oXML.IntoElem();

  if (!oXML.FindChildElem("Bodies")) {
    return false;
  }
  int nBodies = atoi(oXML.GetChildData().c_str());
  SSettings6DOF s6DBodySettings;
  SPoint sPoint;
  s6DBodySettings.vsPoints.clear();

  for (int iBody = 0; iBody < nBodies; iBody++) {
    if (!oXML.FindChildElem("Body")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Name")) {
      return false;
    }
    s6DBodySettings.oName = oXML.GetChildData();

    if (!oXML.FindChildElem("RGBColor")) {
      return false;
    }
    s6DBodySettings.vsPoints.clear();
    s6DBodySettings.nRGBColor = atoi(oXML.GetChildData().c_str());

    while (oXML.FindChildElem("Point")) {
      oXML.IntoElem();
      if (!oXML.FindChildElem("X")) {
        return false;
      }
      sPoint.fX = (float)atof(oXML.GetChildData().c_str());

      if (!oXML.FindChildElem("Y")) {
        return false;
      }
      sPoint.fY = (float)atof(oXML.GetChildData().c_str());

      if (!oXML.FindChildElem("Z")) {
        return false;
      }
      sPoint.fZ = (float)atof(oXML.GetChildData().c_str());

      oXML.OutOfElem();  // Point
      s6DBodySettings.vsPoints.push_back(sPoint);
    }
    mvs6DOFBodySettings.push_back(s6DBodySettings);
    oXML.OutOfElem();  // Body
  }
  bDataAvailable = true;
  return true;
}  // Read6DOFSettings

unsigned int CRTProtocol::Get6DOFBodyCount() const {
  return (unsigned int)mvs6DOFBodySettings.size();
}

const char* CRTProtocol::Get6DOFBodyName(unsigned int nBodyIndex) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    return mvs6DOFBodySettings[nBodyIndex].oName.c_str();
  }

  maErrorStr[0] = 0;  // No error string.
  return NULL;
}

unsigned int CRTProtocol::Get6DOFBodyColor(unsigned int nBodyIndex) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    return mvs6DOFBodySettings[nBodyIndex].nRGBColor;
  }

  maErrorStr[0] = 0;  // No error string.
  return 0;
}

unsigned int CRTProtocol::Get6DOFBodyPointCount(unsigned int nBodyIndex) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    return (unsigned int)mvs6DOFBodySettings.at(nBodyIndex).vsPoints.size();
  }
  maErrorStr[0] = 0;  // No error string.
  return false;
}

bool CRTProtocol::Get6DOFBodyPoint(unsigned int nBodyIndex,
                                   unsigned int nMarkerIndex, SPoint& sPoint) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    if (nMarkerIndex < mvs6DOFBodySettings.at(nBodyIndex).vsPoints.size()) {
      sPoint.fX = mvs6DOFBodySettings.at(nBodyIndex).vsPoints[nMarkerIndex].fX;
      sPoint.fY = mvs6DOFBodySettings.at(nBodyIndex).vsPoints[nMarkerIndex].fY;
      sPoint.fZ = mvs6DOFBodySettings.at(nBodyIndex).vsPoints[nMarkerIndex].fZ;
      return true;
    }
  }
  maErrorStr[0] = 0;  // No error string.
  return false;
}

CRTProtocol::ECameraSystemType CRTProtocol::GetCameraSystemType() {
  return msGeneralSettings.sCameraSystem.eType;
}

char* CRTProtocol::GetErrorString() { return maErrorStr; }

bool CRTProtocol::SendString(const char* pCmdStr, int nType) {
  int nSize;
  int nCmdStrLen = (int)strlen(pCmdStr);
  static char aSendBuffer[5000];

  if ((unsigned)nCmdStrLen > sizeof(aSendBuffer)) {
    snprintf(maErrorStr, sizeof(maErrorStr),
             "String is larger than send buffer.");
    return false;
  }

  //
  // Header size + length of the string + terminating null char
  //
  nSize = 8 + nCmdStrLen + 1;

  memcpy(aSendBuffer + 8, pCmdStr, nCmdStrLen + 1);

  if ((mnMajorVersion == 1 && mnMinorVersion == 0) || mbBigEndian) {
    *((unsigned int*)aSendBuffer) = htonl(nSize);
    *((unsigned int*)(aSendBuffer + 4)) = htonl(nType);
  } else {
    *((unsigned int*)aSendBuffer) = nSize;
    *((unsigned int*)(aSendBuffer + 4)) = nType;
  }

  if (mpoNetwork->Send(aSendBuffer, nSize) == false) {
    snprintf(maErrorStr, sizeof(maErrorStr), mpoNetwork->GetErrorString());
    return false;
  }

  return true;

}  // SendString

bool CRTProtocol::SendCommand(const char* pCmdStr) {
  return SendString(pCmdStr, CRTPacket::PacketCommand);
}  // SendCommand

bool CRTProtocol::SendCommand(const char* pCmdStr, char* pCommandResponseStr,
                              unsigned int nCommandResponseLen,
                              unsigned int timeout) {
  CRTPacket::EPacketType eType;

  if (SendString(pCmdStr, CRTPacket::PacketCommand)) {
    while (ReceiveRTPacket(eType, true, timeout) > 0) {
      if (eType == CRTPacket::PacketCommand) {
        strcpy(pCommandResponseStr, mpoRTPacket->GetCommandString());
        return true;
      }
      if (eType == CRTPacket::PacketError) {
        strcpy(pCommandResponseStr, mpoRTPacket->GetErrorString());
        return false;
      }
    }
  } else {
    char pTmpStr[256];
    strcpy(pTmpStr, maErrorStr);
    snprintf(maErrorStr, sizeof(maErrorStr), "\'%s\' command failed. %s",
             pCmdStr, pTmpStr);
  }
  pCommandResponseStr[0] = 0;
  return false;
}  // SendCommand

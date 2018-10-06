#include "RTProtocol.h"
#include "Markup.h"
#include "Network.h"
#include "stdafx.h"
#include <algorithm>
#include <cctype>
#include <float.h>
#include <iphlpapi.h>
// import the internet protocol helper library.
#pragma comment(lib, "IPHLPAPI.lib")

namespace {

inline void ToLower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), std::tolower);
}

inline void RemoveInvalidChars(std::string &str) {
  auto isInvalidChar = [](int c) -> int {
    // iscntrl: control codes (NUL, etc.), '\t', '\n', '\v', '\f', '\r',
    // backspace (DEL) isspace: some common checks but also 0x20 (SPACE) return
    // != 0 --> invalid char
    return std::iscntrl(c) + std::isspace(c);
  };
  str.erase(std::remove_if(str.begin(), str.end(), isInvalidChar), str.end());
}

} // namespace

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
} // CRTProtocol

CRTProtocol::~CRTProtocol() {
  if (mpoNetwork) {
    delete mpoNetwork;
    mpoNetwork = NULL;
  }
  if (mpoRTPacket) {
    delete mpoRTPacket;
    mpoRTPacket = NULL;
  }
} // ~CRTProtocol

bool CRTProtocol::Connect(char *pServerAddr, unsigned short nPort,
                          unsigned short *pnUDPServerPort, int nMajorVersion,
                          int nMinorVersion, bool bBigEndian) {
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
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "Could not allocate data packet.");
    return false;
  }

  if (mpoNetwork->Connect(pServerAddr, nPort)) {
    if (pnUDPServerPort != NULL) {
      if (mpoNetwork->CreateUDPSocket(*pnUDPServerPort) == false) {
        sprintf_s(maErrorStr, sizeof(maErrorStr), "CreateUDPSocket failed. %s",
                  mpoNetwork->GetErrorString());
        Disconnect();
        return false;
      }
    }

    // Welcome message
    if (ReceiveRTPacket(eType, true)) {
      if (eType == CRTPacket::PacketError) {
        strcpy_s(maErrorStr, sizeof(maErrorStr), mpoRTPacket->GetErrorString());
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
                sprintf_s(tTemp, sizeof(tTemp), "ByteOrder BigEndian");
              } else {
                sprintf_s(tTemp, sizeof(tTemp), "ByteOrder LittleEndian");
              }

              if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
                return true;
              } else {
                sprintf_s(maErrorStr, sizeof(maErrorStr),
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
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "Missing QTM server welcome message.");
    Disconnect();
  } else {
    if (mpoNetwork->GetError() == 10061) {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "Check if QTM is running on target machine.");
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s",
                mpoNetwork->GetErrorString());
    }
  }

  return false;
} // Connect

unsigned short CRTProtocol::GetUdpServerPort() {
  if (mpoNetwork) {
    return mpoNetwork->GetUdpServerPort();
  }
  return 0;
}

void CRTProtocol::Disconnect() {
  mpoNetwork->Disconnect();
  mnBroadcastPort = 0;
  if (mpoRTPacket) {
    delete mpoRTPacket;
    mpoRTPacket = NULL;
  }
} // Disconnect

bool CRTProtocol::Connected() { return mpoNetwork->Connected(); }

bool CRTProtocol::SetVersion(int nMajorVersion, int nMinorVersion) {
  char tTemp[256];
  char pResponseStr[256];

  sprintf_s(tTemp, sizeof(tTemp), "Version %u.%u", nMajorVersion,
            nMinorVersion);

  if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
    sprintf_s(tTemp, sizeof(tTemp), "Version set to %u.%u", nMajorVersion,
              nMinorVersion);

    if (strcmp(pResponseStr, tTemp) == 0) {
      mnMajorVersion = nMajorVersion;
      mnMinorVersion = nMinorVersion;
      mpoRTPacket->SetVersion(mnMajorVersion, mnMinorVersion);
      return true;
    }

    if (pResponseStr) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Set Version failed.");
    }
  } else {
    strcpy_s(tTemp, sizeof(tTemp), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Send Version failed. %s.",
              tTemp);
  }
  return false;
}

bool CRTProtocol::GetVersion(unsigned int &nMajorVersion,
                             unsigned int &nMinorVersion) {
  if (!Connected()) {
    return false;
  }

  nMajorVersion = mnMajorVersion;
  nMinorVersion = mnMinorVersion;

  return true;
}

bool CRTProtocol::GetQTMVersion(char *pVersion, unsigned int nVersionLen) {
  if (SendCommand("QTMVersion", pVersion, nVersionLen)) {
    return true;
  }
  sprintf_s(maErrorStr, sizeof(maErrorStr), "Get QTM Version failed.");
  return false;
}

bool CRTProtocol::GetByteOrder(bool &bBigEndian) {
  char pResponseStr[256];

  if (SendCommand("ByteOrder", pResponseStr, sizeof(pResponseStr))) {
    bBigEndian = (strcmp(pResponseStr, "Byte order is big endian") == 0);
    return true;
  }
  sprintf_s(maErrorStr, sizeof(maErrorStr), "Get Byte order failed.");
  return false;
}

bool CRTProtocol::CheckLicense(char *pLicenseCode) {
  char tTemp[100];
  char pResponseStr[256];

  if (strlen(pLicenseCode) <= 85) {
    sprintf_s(tTemp, sizeof(tTemp), "CheckLicense %s", pLicenseCode);

    if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
      if (strcmp(pResponseStr, "License pass") == 0) {
        return true;
      }
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Wrong license code.");
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "CheckLicense failed.");
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "License code too long");
  }
  return false;
}

bool CRTProtocol::DiscoverRTServer(unsigned short nServerPort,
                                   bool bNoLocalResponses,
                                   unsigned short nDiscoverPort) {
  char pData[10];
  SDiscoverResponse sResponse;

  if (mnBroadcastPort > 0 || mpoNetwork->CreateUDPSocket(nServerPort, true)) {
    if (mnBroadcastPort == 0) {
      mnBroadcastPort = nServerPort;
    } else {
      nServerPort = mnBroadcastPort;
    }

    *((unsigned int *)pData) = (unsigned int)10;
    *((unsigned int *)(pData + 4)) = (unsigned int)CRTPacket::PacketDiscover;
    *((unsigned short *)(pData + 8)) = htons(nServerPort);

    if (mpoNetwork->SendUDPBroadcast(pData, 10, nDiscoverPort)) {
      int nReceived;
      unsigned int nAddr;

      mvsDiscoverResponseList.clear();

      do {
        nReceived = mpoNetwork->Receive(maDataBuff, sizeof(maDataBuff), false,
                                        100000, &nAddr);

        if (nReceived != -1 && nReceived != -2 && nReceived > 8) {
          char *pResponseStr;

          if (CRTPacket::GetType(maDataBuff) == CRTPacket::PacketCommand) {
            pResponseStr = CRTPacket::GetCommandString(maDataBuff);
            sResponse.nAddr = nAddr;
            sResponse.nBasePort =
                CRTPacket::GetDiscoverResponseBasePort(maDataBuff);

            if (pResponseStr &&
                (!bNoLocalResponses || !mpoNetwork->IsLocalAddress(nAddr))) {
              strcpy_s(sResponse.pMessage, sizeof(sResponse.pMessage),
                       pResponseStr);
              mvsDiscoverResponseList.push_back(sResponse);
            }
          }
        }
      } while (nReceived != -1 && nReceived != -2 &&
               nReceived > 8); // Keep reading until no more responses.

      return true;
    }
  }
  return false;
}

int CRTProtocol::GetNumberOfDiscoverResponses() {
  return (int)mvsDiscoverResponseList.size();
}

bool CRTProtocol::GetDiscoverResponse(unsigned int nIndex, unsigned int &nAddr,
                                      unsigned short &nBasePort, char *pMessage,
                                      int nMessageLen) {
  if (nIndex < mvsDiscoverResponseList.size()) {
    nAddr = mvsDiscoverResponseList[nIndex].nAddr;
    nBasePort = mvsDiscoverResponseList[nIndex].nBasePort;
    strcpy_s(pMessage, nMessageLen, mvsDiscoverResponseList[nIndex].pMessage);
    return true;
  }
  return false;
}

bool CRTProtocol::GetCurrentFrame(unsigned int nComponentType,
                                  const char *selectedAnalogChannels) {
  char pCommandStr[256];

  sprintf_s(pCommandStr, sizeof(pCommandStr), "GetCurrentFrame ");
  std::string::size_type nCommandStrSize = strlen(pCommandStr);

  if (GetComponentString(pCommandStr + (int)nCommandStrSize,
                         sizeof(pCommandStr) - (int)nCommandStrSize,
                         nComponentType, selectedAnalogChannels)) {
    if (SendCommand(pCommandStr)) {
      return true;
    }
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetCurrentFrame failed.");
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "DataComponent missing.");
  }
  return false;
}

bool CRTProtocol::StreamFrames(EStreamRate eRate, unsigned int nRateArg,
                               unsigned short nUDPPort, const char *pUDPAddr,
                               unsigned int nComponentType,
                               const char *selectedAnalogChannels) {
  char pCommandStr[256];

  if (eRate == RateFrequencyDivisor) {
    sprintf_s(pCommandStr, sizeof(pCommandStr),
              "StreamFrames FrequencyDivisor:%d ", nRateArg);
  } else if (eRate == RateFrequency) {
    sprintf_s(pCommandStr, sizeof(pCommandStr), "StreamFrames Frequency:%d ",
              nRateArg);
  } else if (eRate == RateAllFrames) {
    sprintf_s(pCommandStr, sizeof(pCommandStr), "StreamFrames AllFrames ");
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "No valid rate.");
    return false;
  }

  if (nUDPPort > 0) {
    if (pUDPAddr != NULL && strlen(pUDPAddr) > 64) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "UDP address string too long.");
      return false;
    }
    sprintf_s(pCommandStr, sizeof(pCommandStr), "%s UDP%s%s:%d ", pCommandStr,
              pUDPAddr != NULL ? ":" : "", pUDPAddr != NULL ? pUDPAddr : "",
              nUDPPort);
  }

  std::string::size_type nCommandStrSize = strlen(pCommandStr);

  if (GetComponentString(pCommandStr + (int)nCommandStrSize,
                         sizeof(pCommandStr) - (int)nCommandStrSize,
                         nComponentType, selectedAnalogChannels)) {
    if (SendCommand(pCommandStr)) {
      return true;
    }
    sprintf_s(maErrorStr, sizeof(maErrorStr), "StreamFrames failed.");
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "DataComponent missing.");
  }

  return false;
}

bool CRTProtocol::StreamFramesStop() {
  if (SendCommand("StreamFrames Stop")) {
    return true;
  }
  sprintf_s(maErrorStr, sizeof(maErrorStr), "StreamFrames Stop failed.");
  return false;
}

bool CRTProtocol::GetState(CRTPacket::EEvent &eEvent, bool bUpdate,
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
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetLastEvent failed.");
  } else {
    eEvent = meState;
    return true;
  }
  return false;
}

bool CRTProtocol::GetCapture(char *pFileName, bool bC3D) {
  CRTPacket::EPacketType eType;
  char pResponseStr[256];

  if (fopen_s(&mpFileBuffer, pFileName, "wb") == 0) {
    if (bC3D) {
      // C3D file
      if (SendCommand((mnMajorVersion > 1 || mnMinorVersion > 7)
                          ? "GetCaptureC3D"
                          : "GetCapture",
                      pResponseStr, sizeof(pResponseStr))) {
        if (strcmp(pResponseStr, "Sending capture") == 0) {
          if (ReceiveRTPacket(eType, true,
                              5000000)) // Wait for C3D file in 5 seconds.
          {
            if (eType == CRTPacket::PacketC3DFile) {
              if (mpFileBuffer != NULL) {
                fclose(mpFileBuffer);
                return true;
              }
              sprintf_s(maErrorStr, sizeof(maErrorStr),
                        "Writing C3D file failed.");
            } else {
              sprintf_s(maErrorStr, sizeof(maErrorStr),
                        "Wrong packet type received.");
            }
          } else {
            sprintf_s(maErrorStr, sizeof(maErrorStr), "No packet received.");
          }
        } else {
          sprintf_s(maErrorStr, sizeof(maErrorStr), "%s failed.",
                    (mnMajorVersion > 1 || mnMinorVersion > 7) ? "GetCaptureC3D"
                                                               : "GetCapture");
        }
      } else {
        sprintf_s(maErrorStr, sizeof(maErrorStr), "%s failed.",
                  (mnMajorVersion > 1 || mnMinorVersion > 7) ? "GetCaptureC3D"
                                                             : "GetCapture");
      }
    } else {
      // QTM file
      if (SendCommand("GetCaptureQTM", pResponseStr, sizeof(pResponseStr))) {
        if (strcmp(pResponseStr, "Sending capture") == 0) {
          if (ReceiveRTPacket(eType, true,
                              5000000)) // Wait for QTM file in 5 seconds.
          {
            if (eType == CRTPacket::PacketQTMFile) {
              if (mpFileBuffer != NULL) {
                fclose(mpFileBuffer);
                return true;
              }
              sprintf_s(maErrorStr, sizeof(maErrorStr),
                        "Writing QTM file failed.");
            } else {
              sprintf_s(maErrorStr, sizeof(maErrorStr),
                        "Wrong packet type received.");
            }
          } else {
            sprintf_s(maErrorStr, sizeof(maErrorStr), "No packet received. %s.",
                      maErrorStr);
          }
        } else {
          sprintf_s(maErrorStr, sizeof(maErrorStr), "GetCaptureQTM failed.");
        }
      } else {
        sprintf_s(maErrorStr, sizeof(maErrorStr), "GetCaptureQTM failed.");
      }
    }
  }
  if (mpFileBuffer) {
    fclose(mpFileBuffer);
  }

  return false;
}

bool CRTProtocol::SendTrig() {
  char pResponseStr[256];

  if (SendCommand("Trig", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Trig ok") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Trig failed.");
  }
  return false;
}

bool CRTProtocol::SetQTMEvent(char *pLabel) {
  char tTemp[100];
  char pResponseStr[256];

  if (strlen(pLabel) <= 92) {
    sprintf_s(tTemp, sizeof(tTemp), "%s %s",
              (mnMajorVersion > 1 || mnMinorVersion > 7) ? "SetQTMEvent"
                                                         : "Event",
              pLabel);

    if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
      if (strcmp(pResponseStr, "Event set") == 0) {
        return true;
      }
    }
    if (pResponseStr) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s failed.",
                (mnMajorVersion > 1 || mnMinorVersion > 7) ? "SetQTMEvent"
                                                           : "Event");
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Event label too long.");
  }
  return false;
}

bool CRTProtocol::TakeControl(char *pPassword) {
  char pResponseStr[256];
  char pCmd[64];

  sprintf_s(pCmd, sizeof(pCmd), "TakeControl");
  if (pPassword != NULL) {
    // Add password
    if (pPassword[0] != 0) {
      strcat_s(pCmd, sizeof(pCmd), " ");
      strcat_s(pCmd, sizeof(pCmd), pPassword);
    }
  }
  if (SendCommand(pCmd, pResponseStr, sizeof(pResponseStr))) {
    if (strcmp("You are now master", pResponseStr) == 0 ||
        strcmp("You are already master", pResponseStr) == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "TakeControl failed.");
  }
  return false;
} // TakeControl

bool CRTProtocol::ReleaseControl() {
  char pResponseStr[256];

  if (SendCommand("ReleaseControl", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp("You are now a regular client", pResponseStr) == 0 ||
        strcmp("You are already a regular client", pResponseStr) == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "ReleaseControl failed.");
  }
  return false;
} // ReleaseControl

bool CRTProtocol::NewMeasurement() {
  char pResponseStr[256];

  if (SendCommand("New", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Creating new connection") == 0 ||
        strcmp(pResponseStr, "Already connected") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "New failed.");
  }
  return false;
}

bool CRTProtocol::CloseMeasurement() {
  char pResponseStr[256];

  if (SendCommand("Close", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Closing connection") == 0 ||
        strcmp(pResponseStr, "File closed") == 0 ||
        strcmp(pResponseStr, "Closing file") == 0 ||
        strcmp(pResponseStr, "No connection to close") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Close failed.");
  }
  return false;
}

bool CRTProtocol::StartCapture() {
  char pResponseStr[256];

  if (SendCommand("Start", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Starting measurement") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Start failed.");
  }
  return false;
}

bool CRTProtocol::StartRTOnFile() {
  char pResponseStr[256];

  if (SendCommand("Start rtfromfile", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Starting RT from file") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Starting RT from file failed.");
  }
  return false;
}

bool CRTProtocol::StopCapture() {
  char pResponseStr[256];

  if (SendCommand("Stop", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Stopping measurement") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Stop failed.");
  }
  return false;
}

bool CRTProtocol::LoadCapture(char *pFileName) {
  char tTemp[100];
  char pResponseStr[256];

  if (strlen(pFileName) <= 94) {
    sprintf_s(tTemp, sizeof(tTemp), "Load %s", pFileName);

    if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr),
                    20000000)) // Set timeout to 20 s for Load command.
    {
      if (strcmp(pResponseStr, "Measurement loaded") == 0) {
        return true;
      }
    }
    if (strlen(pResponseStr) > 0) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Load failed.");
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "File name too long.");
  }
  return false;
}

bool CRTProtocol::SaveCapture(char *pFileName, bool bOverwrite,
                              char *pNewFileName, int nSizeOfNewFileName) {
  char tTemp[100];
  char pResponseStr[256];
  char tNewFileNameTmp[300];

  tNewFileNameTmp[0] = 0;

  if (strlen(pFileName) <= 94) {
    sprintf_s(tTemp, sizeof(tTemp), "Save %s%s", pFileName,
              bOverwrite ? " Overwrite" : "");

    if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
      if (strcmp(pResponseStr, "Measurement saved") == 0) {
        if (pNewFileName && nSizeOfNewFileName > 0) {
          pNewFileName[0] = 0;
        }
        return true;
      }
      if (sscanf_s(pResponseStr, "Measurement saved as '%[^']'",
                   tNewFileNameTmp, (int)sizeof(tNewFileNameTmp)) == 1) {
        if (pNewFileName) {
          strcpy_s(pNewFileName, nSizeOfNewFileName, tNewFileNameTmp);
        }
        return true;
      }
    }
    if (pResponseStr) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Save failed.");
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "File name too long.");
  }
  return false;
}

bool CRTProtocol::LoadProject(char *pFileName) {
  char tTemp[100];
  char pResponseStr[256];

  if (strlen(pFileName) <= 94) {
    sprintf_s(tTemp, sizeof(tTemp), "LoadProject %s", pFileName);

    if (SendCommand(tTemp, pResponseStr, sizeof(pResponseStr))) {
      if (strcmp(pResponseStr, "Project loaded") == 0) {
        return true;
      }
    }
    if (pResponseStr) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Load project failed.");
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "File name too long.");
  }
  return false;
}

bool CRTProtocol::Reprocess() {
  char pResponseStr[256];

  if (SendCommand("Reprocess", pResponseStr, sizeof(pResponseStr))) {
    if (strcmp(pResponseStr, "Reprocessing file") == 0) {
      return true;
    }
  }
  if (pResponseStr) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.", pResponseStr);
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Reprocess failed.");
  }
  return false;
}

bool CRTProtocol::GetEventString(CRTPacket::EEvent eEvent, char *pStr,
                                 int nStrLen) {
  switch (eEvent) {
  case CRTPacket::EventConnected:
    sprintf_s(pStr, nStrLen, "Connected");
    break;
  case CRTPacket::EventConnectionClosed:
    sprintf_s(pStr, nStrLen, "Connection Closed");
    break;
  case CRTPacket::EventCaptureStarted:
    sprintf_s(pStr, nStrLen, "Capture Started");
    break;
  case CRTPacket::EventCaptureStopped:
    sprintf_s(pStr, nStrLen, "Capture Stopped");
    break;
  case CRTPacket::EventCaptureFetchingFinished:
    sprintf_s(pStr, nStrLen, "Fetching Finished");
    break;
  case CRTPacket::EventCalibrationStarted:
    sprintf_s(pStr, nStrLen, "Calibration Started");
    break;
  case CRTPacket::EventCalibrationStopped:
    sprintf_s(pStr, nStrLen, "Calibration Finished");
    break;
  case CRTPacket::EventRTfromFileStarted:
    sprintf_s(pStr, nStrLen, "RT From File Started");
    break;
  case CRTPacket::EventRTfromFileStopped:
    sprintf_s(pStr, nStrLen, "RT From File Stopped");
    break;
  case CRTPacket::EventWaitingForTrigger:
    sprintf_s(pStr, nStrLen, "Waiting For Trigger");
    break;
  case CRTPacket::EventCameraSettingsChanged:
    sprintf_s(pStr, nStrLen, "Camera Settings Changed");
    break;
  case CRTPacket::EventQTMShuttingDown:
    sprintf_s(pStr, nStrLen, "QTM Shutting Down");
    break;
  case CRTPacket::EventCaptureSaved:
    sprintf_s(pStr, nStrLen, "Capture Saved");
    break;
  case CRTPacket::EventReprocessingStarted:
    sprintf_s(pStr, nStrLen, "Reprocessing Started");
    break;
  case CRTPacket::EventReprocessingStopped:
    sprintf_s(pStr, nStrLen, "Reprocessing Stopped");
    break;
  default:
    return false;
  }
  return true;
}

bool CRTProtocol::ConvertRateString(const char *pRate, EStreamRate &eRate,
                                    unsigned int &nRateArg) {
  std::string rateString;

  rateString.assign(pRate);
  std::transform(rateString.begin(), rateString.end(), rateString.begin(),
                 ::tolower);

  if (rateString.compare(0, rateString.size(), _T("allframes"), 9) == 0) {
    eRate = RateAllFrames;
  } else if (rateString.compare(0, rateString.size(), _T("frequency:"), 10) ==
             0) {
    nRateArg = atoi(rateString.substr(10).c_str());
    if (nRateArg > 0) {
      eRate = RateFrequency;
    }
  }
  if (rateString.compare(0, rateString.size(), _T("frequencydivisor:"), 17) ==
      0) {
    nRateArg = atoi(rateString.substr(17).c_str());
    if (nRateArg > 0) {
      eRate = RateFrequencyDivisor;
    }
  }
  return eRate != RateNone;
}

unsigned int CRTProtocol::ConvertComponentString(const char *pComponentType) {
  std::string componentString;
  unsigned int componentTypes = 0;

  componentString.assign(pComponentType);
  // Make string lower case.
  std::transform(componentString.begin(), componentString.end(),
                 componentString.begin(), ::tolower);

  if (componentString.find(_T("all")) != std::string::npos) {
    return CRTProtocol::cComponentAll;
  }
  if (componentString.find(_T("2d")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent2d;
  }
  if (componentString.find(_T("2dlin")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent2dLin;
  }
  if (componentString.find(_T("3d")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent3d;
  }
  if (componentString.find(_T("3dres")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent3dRes;
  }
  if (componentString.find(_T("3dnolabels")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent3dNoLabels;
  }
  if (componentString.find(_T("3dnolabelsres")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent3dNoLabelsRes;
  }
  if (componentString.find(_T("analog")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponentAnalog;
  }
  if (componentString.find(_T("analogsingle")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponentAnalogSingle;
  }
  if (componentString.find(_T("force")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponentForce;
  }
  if (componentString.find(_T("forcesingle")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponentForceSingle;
  }
  if (componentString.find(_T("6d")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent6d;
  }
  if (componentString.find(_T("6dres")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent6dRes;
  }
  if (componentString.find(_T("6deuler")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent6dEuler;
  }
  if (componentString.find(_T("6deulerres")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponent6dEulerRes;
  }
  if (componentString.find(_T("image")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponentImage;
  }
  if (componentString.find(_T("gazevector")) != std::string::npos) {
    componentTypes += CRTProtocol::cComponentGazeVector;
  }
  return componentTypes;
}

bool CRTProtocol::GetComponentString(char *pComponentStr, int nComponentStrLen,
                                     unsigned int nComponentType,
                                     const char *selectedAnalogChannels) {
  pComponentStr[0] = 0;

  if (nComponentType == cComponentAll) {
    strcat_s(pComponentStr, nComponentStrLen, "All");
    return true;
  }

  if (nComponentType & cComponent2d) {
    strcat_s(pComponentStr, nComponentStrLen, "2D ");
  }
  if (nComponentType & cComponent2dLin) {
    strcat_s(pComponentStr, nComponentStrLen, "2DLin ");
  }
  if (nComponentType & cComponent3d) {
    strcat_s(pComponentStr, nComponentStrLen, "3D ");
  }
  if (nComponentType & cComponent3dRes) {
    strcat_s(pComponentStr, nComponentStrLen, "3DRes ");
  }
  if (nComponentType & cComponent3dNoLabels) {
    strcat_s(pComponentStr, nComponentStrLen, "3DNoLabels ");
  }
  if (nComponentType & cComponent3dNoLabelsRes) {
    strcat_s(pComponentStr, nComponentStrLen, "3DNoLabelsRes ");
  }
  if (nComponentType & cComponent6d) {
    strcat_s(pComponentStr, nComponentStrLen, "6D ");
  }
  if (nComponentType & cComponent6dRes) {
    strcat_s(pComponentStr, nComponentStrLen, "6DRes ");
  }
  if (nComponentType & cComponent6dEuler) {
    strcat_s(pComponentStr, nComponentStrLen, "6DEuler ");
  }
  if (nComponentType & cComponent6dEulerRes) {
    strcat_s(pComponentStr, nComponentStrLen, "6DEulerRes ");
  }
  if (nComponentType & cComponentAnalog) {
    if ((nComponentType & cComponentAnalog) && selectedAnalogChannels != NULL &&
        strlen(selectedAnalogChannels) > 0) {
      strcat_s(pComponentStr, nComponentStrLen, "Analog:");
      strcat_s(pComponentStr, nComponentStrLen, selectedAnalogChannels);
      strcat_s(pComponentStr, nComponentStrLen, " ");
    } else {
      strcat_s(pComponentStr, nComponentStrLen, "Analog ");
    }
  }
  if (nComponentType & cComponentAnalogSingle) {
    if ((nComponentType & cComponentAnalogSingle) &&
        selectedAnalogChannels != NULL) {
      strcat_s(pComponentStr, nComponentStrLen, "AnalogSingle:");
      strcat_s(pComponentStr, nComponentStrLen, selectedAnalogChannels);
      strcat_s(pComponentStr, nComponentStrLen, " ");
    } else {
      strcat_s(pComponentStr, nComponentStrLen, "AnalogSingle ");
    }
  }
  if (nComponentType & cComponentForce) {
    strcat_s(pComponentStr, nComponentStrLen, "Force ");
  }
  if (nComponentType & cComponentForceSingle) {
    strcat_s(pComponentStr, nComponentStrLen, "ForceSingle ");
  }
  if (nComponentType & cComponentGazeVector) {
    strcat_s(pComponentStr, nComponentStrLen, "GazeVector ");
  }
  if (nComponentType & cComponentImage) {
    strcat_s(pComponentStr, nComponentStrLen, "Image ");
  }

  return (pComponentStr[0] != 0);
}

int CRTProtocol::ReceiveRTPacket(CRTPacket::EPacketType &eType,
                                 bool bSkipEvents, int nTimeout) {
  int nRecved = 0;
  unsigned int nRecvedTotal = 0;
  unsigned int nFrameSize;

  eType = CRTPacket::PacketNone;

  do {
    nRecved = 0;
    nRecvedTotal = 0;

    nRecved =
        mpoNetwork->Receive(maDataBuff, sizeof(maDataBuff), true, nTimeout);
    if (nRecved == 0) {
      return 0; // Receive timeout
    }
    if (nRecved < sizeof(int) * 2) {
      // QTM header not received.
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Couldn't read header bytes.");
      return -1;
    }
    if (nRecved == -1) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Socket Error.");
      return -1;
    }
    if (nRecved == -2) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "Disconnected from server.");
      return -1;
    }
    nRecvedTotal += nRecved;

    bool bBigEndian =
        (mbBigEndian || (mnMajorVersion == 1 && mnMinorVersion == 0));
    nFrameSize = mpoRTPacket->GetSize(maDataBuff, bBigEndian);
    eType = mpoRTPacket->GetType(maDataBuff, bBigEndian);

    unsigned int nReadSize;

    if (eType == CRTPacket::PacketC3DFile ||
        eType == CRTPacket::PacketQTMFile) {
      if (mpFileBuffer != NULL) {
        rewind(mpFileBuffer); // Start from the beginning
        if (fwrite(maDataBuff + sizeof(int) * 2, 1,
                   nRecvedTotal - sizeof(int) * 2,
                   mpFileBuffer) != nRecvedTotal - sizeof(int) * 2) {
          sprintf_s(maErrorStr, sizeof(maErrorStr),
                    "Failed to write file to disk.");
          fclose(mpFileBuffer);
          mpFileBuffer = NULL;
          return -1;
        }
        // Receive more data until we have read the whole packet
        while (nRecvedTotal < nFrameSize) {
          nReadSize = nFrameSize - nRecvedTotal;
          if (nFrameSize > sizeof(maDataBuff)) {
            nReadSize = sizeof(maDataBuff);
          }
          // As long as we haven't received enough data, wait for more
          nRecved = mpoNetwork->Receive(&(maDataBuff[sizeof(int) * 2]),
                                        nReadSize, false, nTimeout);
          if (nRecved <= 0) {
            if (nRecved == -2) {
              sprintf_s(maErrorStr, sizeof(maErrorStr),
                        "Disconnected from server.");
            } else {
              sprintf_s(maErrorStr, sizeof(maErrorStr), "Socket Error.");
            }
            fclose(mpFileBuffer);
            mpFileBuffer = NULL;
            return -1;
          }
          if (fwrite(maDataBuff + sizeof(int) * 2, 1, nRecved, mpFileBuffer) !=
              (size_t)nRecved) {
            sprintf_s(maErrorStr, sizeof(maErrorStr),
                      "Failed to write file to disk.");
            fclose(mpFileBuffer);
            mpFileBuffer = NULL;
            return -1;
          }
          nRecvedTotal += nRecved;
        }
      } else {
        sprintf_s(maErrorStr, sizeof(maErrorStr),
                  "Receive file buffer not opened.");
        if (mpFileBuffer) {
          fclose(mpFileBuffer);
        }
        mpFileBuffer = NULL;
        return -1;
      }
    } else {
      if (nFrameSize > sizeof(maDataBuff)) {
        sprintf_s(maErrorStr, sizeof(maErrorStr), "Receive buffer overflow.");
        return -1;
      }

      // Receive more data until we have read the whole packet
      while (nRecvedTotal < nFrameSize) {
        // As long as we haven't received enough data, wait for more
        nRecved =
            mpoNetwork->Receive(&(maDataBuff[nRecvedTotal]),
                                nFrameSize - nRecvedTotal, false, nTimeout);
        if (nRecved <= 0) {
          if (nRecved == -2) {
            sprintf_s(maErrorStr, sizeof(maErrorStr),
                      "Disconnected from server.");
          } else {
            sprintf_s(maErrorStr, sizeof(maErrorStr), "Socket Error.");
          }
          return -1;
        }
        nRecvedTotal += nRecved;
      }
    }

    mpoRTPacket->SetData(maDataBuff);
    if (mpoRTPacket->GetEvent(
            meLastEvent)) // Update last event if there is an event
    {
      if (meLastEvent != CRTPacket::EventCameraSettingsChanged) {
        meState = meLastEvent;
      }
    }
  } while (bSkipEvents && eType == CRTPacket::PacketEvent);

  if (nRecvedTotal == nFrameSize) {
    return nRecvedTotal;
  }
  sprintf_s(maErrorStr, sizeof(maErrorStr), "Packet truncated.");

  return -1;
} // ReceiveRTPacket

CRTPacket *CRTProtocol::GetRTPacket() { return mpoRTPacket; };

bool CRTProtocol::ReadXmlBool(CMarkup &xml, const std::string &element,
                              bool &value) const {
  if (!xml.FindChildElem(element.c_str())) {
    return false;
  }

  auto str = xml.GetChildData();
  RemoveInvalidChars(str);
  ToLower(str);

  if (str == "true") {
    value = true;
  } else if (str == "false") {
    value = false;
  } else {
    // Don't change value, just report error.
    return false;
  }

  return true;
}

bool CRTProtocol::ReadCameraSystemSettings() {
  CRTPacket::EPacketType eType;
  CMarkup oXML;
  std::string tStr;

  msGeneralSettings.vsCameras.clear();

  if (!SendCommand("GetParameters General")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetParameters General failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "GetParameters General returned wrong packet type. Got type %d "
                "expected type 2.",
                eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());

  // ==================== General ====================
  if (!oXML.FindChildElem("General")) {
    return false;
  }
  oXML.IntoElem();

  if (!oXML.FindChildElem("Frequency")) {
    return false;
  }
  msGeneralSettings.nCaptureFrequency = atoi(oXML.GetChildData().c_str());

  if (!oXML.FindChildElem("Capture_Time")) {
    return false;
  }
  msGeneralSettings.fCaptureTime = (float)atof(oXML.GetChildData().c_str());

  // Refactored variant of all this copy/paste code. TODO: Refactor everything
  // else.
  if (!ReadXmlBool(oXML, "Start_On_External_Trigger",
                   msGeneralSettings.bStartOnExternalTrigger)) {
    return false;
  }
  if (mnMajorVersion > 1 || mnMinorVersion > 14) {
    if (!ReadXmlBool(oXML, "Start_On_Trigger_NO",
                     msGeneralSettings.bStartOnTrigNO)) {
      return false;
    }
    if (!ReadXmlBool(oXML, "Start_On_Trigger_NC",
                     msGeneralSettings.bStartOnTrigNC)) {
      return false;
    }
    if (!ReadXmlBool(oXML, "Start_On_Trigger_Software",
                     msGeneralSettings.bStartOnTrigSoftware)) {
      return false;
    }
  }

  // ==================== External time base ====================
  if (!oXML.FindChildElem("External_Time_Base")) {
    return false;
  }
  oXML.IntoElem();

  if (!oXML.FindChildElem("Enabled")) {
    return false;
  }
  tStr = oXML.GetChildData();
  std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
  msGeneralSettings.sExternalTimebase.bEnabled = (tStr == "true");

  if (!oXML.FindChildElem("Signal_Source")) {
    return false;
  }
  tStr = oXML.GetChildData();
  std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
  if (tStr == "control port") {
    msGeneralSettings.sExternalTimebase.eSignalSource = SourceControlPort;
  } else if (tStr == "ir receiver") {
    msGeneralSettings.sExternalTimebase.eSignalSource = SourceIRReceiver;
  } else if (tStr == "smpte") {
    msGeneralSettings.sExternalTimebase.eSignalSource = SourceSMPTE;
  } else if (tStr == "video sync") {
    msGeneralSettings.sExternalTimebase.eSignalSource = SourceVideoSync;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Signal_Mode")) {
    return false;
  }
  tStr = oXML.GetChildData();
  std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
  if (tStr == "periodic") {
    msGeneralSettings.sExternalTimebase.bSignalModePeriodic = true;
  } else if (tStr == "non-periodic") {
    msGeneralSettings.sExternalTimebase.bSignalModePeriodic = false;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Frequency_Multiplier")) {
    return false;
  }
  unsigned int nMultiplier;
  tStr = oXML.GetChildData();
  if (sscanf_s(tStr.c_str(), "%d", &nMultiplier) == 1) {
    msGeneralSettings.sExternalTimebase.nFreqMultiplier = nMultiplier;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Frequency_Divisor")) {
    return false;
  }
  unsigned int nDivisor;
  tStr = oXML.GetChildData();
  if (sscanf_s(tStr.c_str(), "%d", &nDivisor) == 1) {
    msGeneralSettings.sExternalTimebase.nFreqDivisor = nDivisor;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Frequency_Tolerance")) {
    return false;
  }
  unsigned int nTolerance;
  tStr = oXML.GetChildData();
  if (sscanf_s(tStr.c_str(), "%d", &nTolerance) == 1) {
    msGeneralSettings.sExternalTimebase.nFreqTolerance = nTolerance;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Nominal_Frequency")) {
    return false;
  }
  tStr = oXML.GetChildData();
  std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);

  if (tStr == "none") {
    msGeneralSettings.sExternalTimebase.fNominalFrequency = -1; // -1 = disabled
  } else {
    float fFrequency;
    if (sscanf_s(tStr.c_str(), "%f", &fFrequency) == 1) {
      msGeneralSettings.sExternalTimebase.fNominalFrequency = fFrequency;
    } else {
      return false;
    }
  }

  if (!oXML.FindChildElem("Signal_Edge")) {
    return false;
  }
  tStr = oXML.GetChildData();
  std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
  if (tStr == "negative") {
    msGeneralSettings.sExternalTimebase.bNegativeEdge = true;
  } else if (tStr == "positive") {
    msGeneralSettings.sExternalTimebase.bNegativeEdge = false;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Signal_Shutter_Delay")) {
    return false;
  }
  unsigned int nDelay;
  tStr = oXML.GetChildData();
  if (sscanf_s(tStr.c_str(), "%d", &nDelay) == 1) {
    msGeneralSettings.sExternalTimebase.nSignalShutterDelay = nDelay;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("Non_Periodic_Timeout")) {
    return false;
  }
  float fTimeout;
  tStr = oXML.GetChildData();
  if (sscanf_s(tStr.c_str(), "%f", &fTimeout) == 1) {
    msGeneralSettings.sExternalTimebase.fNonPeriodicTimeout = fTimeout;
  } else {
    return false;
  }

  oXML.OutOfElem(); // External_Time_Base

  _TCHAR *processings[3] = {(_TCHAR *)"Processing_Actions",
                            (_TCHAR *)"RealTime_Processing_Actions",
                            (_TCHAR *)"Reprocessing_Actions"};
  EProcessingActions *processingActions[3] = {
      &msGeneralSettings.eProcessingActions,
      &msGeneralSettings.eRtProcessingActions,
      &msGeneralSettings.eReprocessingActions};
  auto actionsCount = (mnMajorVersion > 1 || mnMinorVersion > 13) ? 3 : 1;
  for (auto i = 0; i < actionsCount; i++) {
    // ==================== Processing actions ====================
    if (!oXML.FindChildElem(processings[i])) {
      return false;
    }
    oXML.IntoElem();

    *processingActions[i] = ProcessingNone;

    if (mnMajorVersion > 1 || mnMinorVersion > 13) {
      if (!oXML.FindChildElem("PreProcessing2D")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] = (EProcessingActions)(*processingActions[i] +
                                                     ProcessingPreProcess2D);
      }
    }

    if (!oXML.FindChildElem("Tracking")) {
      return false;
    }
    tStr = oXML.GetChildData();
    std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
    if (tStr == "3d") {
      *processingActions[i] =
          (EProcessingActions)(*processingActions[i] + ProcessingTracking3D);
    } else if (tStr == "2d" && i != 1) // i != 1 => Not RtProcessingSettings
    {
      *processingActions[i] =
          (EProcessingActions)(*processingActions[i] + ProcessingTracking2D);
    }

    if (i != 1) // Not RtProcessingSettings
    {
      if (!oXML.FindChildElem("TwinSystemMerge")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] = (EProcessingActions)(*processingActions[i] +
                                                     ProcessingTwinSystemMerge);
      }

      if (!oXML.FindChildElem("SplineFill")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] =
            (EProcessingActions)(*processingActions[i] + ProcessingSplineFill);
      }
    }

    if (!oXML.FindChildElem("AIM")) {
      return false;
    }
    if (CompareNoCase(oXML.GetChildData(), "true")) {
      *processingActions[i] =
          (EProcessingActions)(*processingActions[i] + ProcessingAIM);
    }

    if (!oXML.FindChildElem("Track6DOF")) {
      return false;
    }
    if (CompareNoCase(oXML.GetChildData(), "true")) {
      *processingActions[i] =
          (EProcessingActions)(*processingActions[i] + Processing6DOFTracking);
    }

    if (!oXML.FindChildElem("ForceData")) {
      return false;
    }
    if (CompareNoCase(oXML.GetChildData(), "true")) {
      *processingActions[i] =
          (EProcessingActions)(*processingActions[i] + ProcessingForceData);
    }

    if (mnMajorVersion > 1 || mnMinorVersion > 11) {
      if (!oXML.FindChildElem("GazeVector")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] =
            (EProcessingActions)(*processingActions[i] + ProcessingGazeVector);
      }
    }

    if (i != 1) // Not RtProcessingSettings
    {
      if (!oXML.FindChildElem("ExportTSV")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] =
            (EProcessingActions)(*processingActions[i] + ProcessingExportTSV);
      }

      if (!oXML.FindChildElem("ExportC3D")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] =
            (EProcessingActions)(*processingActions[i] + ProcessingExportC3D);
      }

      if (!oXML.FindChildElem("ExportMatlabFile")) {
        return false;
      }
      if (CompareNoCase(oXML.GetChildData(), "true")) {
        *processingActions[i] = (EProcessingActions)(
            *processingActions[i] + ProcessingExportMatlabFile);
      }

      if (mnMajorVersion > 1 || mnMinorVersion > 11) {
        if (!oXML.FindChildElem("ExportAviFile")) {
          return false;
        }
        if (CompareNoCase(oXML.GetChildData(), "true")) {
          *processingActions[i] = (EProcessingActions)(*processingActions[i] +
                                                       ProcessingExportAviFile);
        }
      }
    }
    oXML.OutOfElem(); // Processing_Actions
  }

  // ==================== Camera ====================
  msGeneralSettings.sCameraSystem.eType = ECameraSystemType::Unknown;
  if (oXML.FindChildElem("Camera_System") && oXML.IntoElem()) {
    if (oXML.FindChildElem("Type")) {
      tStr = oXML.GetChildData();
      if (CompareNoCase(tStr, "Oqus")) {
        msGeneralSettings.sCameraSystem.eType = ECameraSystemType::Oqus;
      } else if (CompareNoCase(tStr, "Miqus")) {
        msGeneralSettings.sCameraSystem.eType = ECameraSystemType::Miqus;
      }
    }
    oXML.OutOfElem();
  }

  SSettingsGeneralCamera sCameraSettings;

  while (oXML.FindChildElem("Camera")) {
    oXML.IntoElem();

    if (!oXML.FindChildElem("ID")) {
      return false;
    }
    sCameraSettings.nID = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Model")) {
      return false;
    }
    tStr = oXML.GetChildData();
    std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);

    if (tStr == "macreflex") {
      sCameraSettings.eModel = ModelMacReflex;
    } else if (tStr == "proreflex 120") {
      sCameraSettings.eModel = ModelProReflex120;
    } else if (tStr == "proreflex 240") {
      sCameraSettings.eModel = ModelProReflex240;
    } else if (tStr == "proreflex 500") {
      sCameraSettings.eModel = ModelProReflex500;
    } else if (tStr == "proreflex 1000") {
      sCameraSettings.eModel = ModelProReflex1000;
    } else if (tStr == "oqus 100") {
      sCameraSettings.eModel = ModelOqus100;
    } else if (tStr == "oqus 200" || tStr == "oqus 200 c") {
      sCameraSettings.eModel = ModelOqus200C;
    } else if (tStr == "oqus 300") {
      sCameraSettings.eModel = ModelOqus300;
    } else if (tStr == "oqus 300 plus") {
      sCameraSettings.eModel = ModelOqus300Plus;
    } else if (tStr == "oqus 400") {
      sCameraSettings.eModel = ModelOqus400;
    } else if (tStr == "oqus 500") {
      sCameraSettings.eModel = ModelOqus500;
    } else if (tStr == "oqus 500 plus") {
      sCameraSettings.eModel = ModelOqus500Plus;
    } else if (tStr == "oqus 700") {
      sCameraSettings.eModel = ModelOqus700;
    } else if (tStr == "oqus 700 plus") {
      sCameraSettings.eModel = ModelOqus700Plus;
    } else if (tStr == "miqus m1") {
      sCameraSettings.eModel = ModelMiqusM1;
    } else if (tStr == "miqus m3") {
      sCameraSettings.eModel = ModelMiqusM3;
    } else if (tStr == "miqus m5") {
      sCameraSettings.eModel = ModelMiqusM5;
    } else if (tStr == "miqus sync unit") {
      sCameraSettings.eModel = ModelMiqusSyncUnit;
    } else {
      return false;
    }

    // Only available from protocol version 1.10 and later.
    if (oXML.FindChildElem("Underwater")) {
      tStr = oXML.GetChildData();
      std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
      sCameraSettings.bUnderwater = (tStr == "true");
    }

    if (oXML.FindChildElem("Supports_HW_Sync")) {
      tStr = oXML.GetChildData();
      std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
      sCameraSettings.bSupportsHwSync = (tStr == "true");
    }

    if (!oXML.FindChildElem("Serial")) {
      return false;
    }
    sCameraSettings.nSerial = atoi(oXML.GetChildData().c_str());

    // ==================== Camera Mode ====================
    if (!oXML.FindChildElem("Mode")) {
      return false;
    }
    tStr = oXML.GetChildData();
    std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
    if (tStr == "marker") {
      sCameraSettings.eMode = ModeMarker;
    } else if (tStr == "marker intensity") {
      sCameraSettings.eMode = ModeMarkerIntensity;
    } else if (tStr == "video") {
      sCameraSettings.eMode = ModeVideo;
    } else {
      return false;
    }

    if (mnMajorVersion > 1 || mnMinorVersion > 11) {
      // ==================== Video Mode ====================
      if (!oXML.FindChildElem("Video_Mode")) {
        return false;
      }
      tStr = oXML.GetChildData();
      std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
      if (tStr == "custom") {
        sCameraSettings.eVideoMode = VideoModeCustom;
      } else if (tStr == "1080p_24hz") {
        sCameraSettings.eVideoMode = VideoMode1080p_24hz;
      } else if (tStr == "720p_25hz") {
        sCameraSettings.eVideoMode = VideoMode720p_25hz;
      } else if (tStr == "720p_50hz") {
        sCameraSettings.eVideoMode = VideoMode720p_50hz;
      } else if (tStr == "540p_35hz") {
        sCameraSettings.eVideoMode = VideoMode540p_25hz;
      } else if (tStr == "540p_50hz") {
        sCameraSettings.eVideoMode = VideoMode540p_50hz;
      } else if (tStr == "540p_60hz") {
        sCameraSettings.eVideoMode = VideoMode540p_60hz;
      } else if (tStr == "480p_25hz") {
        sCameraSettings.eVideoMode = VideoMode480p_25hz;
      } else if (tStr == "480p_50hz") {
        sCameraSettings.eVideoMode = VideoMode480p_50hz;
      } else if (tStr == "480p_60hz") {
        sCameraSettings.eVideoMode = VideoMode480p_60hz;
      } else if (tStr == "480p_120hz") {
        sCameraSettings.eVideoMode = VideoMode480p_120hz;
      } else {
        return false;
      }

      // ==================== Video frequency ====================
      if (!oXML.FindChildElem("Video_Frequency")) {
        return false;
      }
      sCameraSettings.nVideoFrequency = atoi(oXML.GetChildData().c_str());
    }

    // ==================== Video exposure ====================
    if (!oXML.FindChildElem("Video_Exposure")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Current")) {
      return false;
    }
    sCameraSettings.nVideoExposure = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Min")) {
      return false;
    }
    sCameraSettings.nVideoExposureMin = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Max")) {
      return false;
    }
    sCameraSettings.nVideoExposureMax = atoi(oXML.GetChildData().c_str());
    oXML.OutOfElem(); // Video_Exposure

    // ==================== Video flash time ====================
    if (!oXML.FindChildElem("Video_Flash_Time")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Current")) {
      return false;
    }
    sCameraSettings.nVideoFlashTime = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Min")) {
      return false;
    }
    sCameraSettings.nVideoFlashTimeMin = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Max")) {
      return false;
    }
    sCameraSettings.nVideoFlashTimeMax = atoi(oXML.GetChildData().c_str());
    oXML.OutOfElem(); // Video_Flash_Time

    // ==================== Marker exposure ====================
    if (!oXML.FindChildElem("Marker_Exposure")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Current")) {
      return false;
    }
    sCameraSettings.nMarkerExposure = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Min")) {
      return false;
    }
    sCameraSettings.nMarkerExposureMin = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Max")) {
      return false;
    }
    sCameraSettings.nMarkerExposureMax = atoi(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Marker_Exposure

    // ==================== Marker threshold ====================
    if (!oXML.FindChildElem("Marker_Threshold")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Current")) {
      return false;
    }
    sCameraSettings.nMarkerThreshold = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Min")) {
      return false;
    }
    sCameraSettings.nMarkerThresholdMin = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Max")) {
      return false;
    }
    sCameraSettings.nMarkerThresholdMax = atoi(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Marker_Threshold

    // ==================== Position ====================
    if (!oXML.FindChildElem("Position")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("X")) {
      return false;
    }
    sCameraSettings.fPositionX = (float)atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Y")) {
      return false;
    }
    sCameraSettings.fPositionY = (float)atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Z")) {
      return false;
    }
    sCameraSettings.fPositionZ = (float)atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_1_1")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[0][0] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_2_1")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[1][0] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_3_1")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[2][0] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_1_2")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[0][1] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_2_2")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[1][1] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_3_2")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[2][1] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_1_3")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[0][2] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_2_3")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[1][2] =
        (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Rot_3_3")) {
      return false;
    }
    sCameraSettings.fPositionRotMatrix[2][2] =
        (float)atof(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Position

    if (!oXML.FindChildElem("Orientation")) {
      return false;
    }
    sCameraSettings.nOrientation = atoi(oXML.GetChildData().c_str());

    // ==================== Marker exposure ====================
    if (!oXML.FindChildElem("Marker_Res")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Width")) {
      return false;
    }
    sCameraSettings.nMarkerResolutionWidth = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Height")) {
      return false;
    }
    sCameraSettings.nMarkerResolutionHeight = atoi(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Marker_Res

    // ==================== Marker resolution ====================
    if (!oXML.FindChildElem("Video_Res")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Width")) {
      return false;
    }
    sCameraSettings.nVideoResolutionWidth = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Height")) {
      return false;
    }
    sCameraSettings.nVideoResolutionHeight = atoi(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Video_Res

    // ==================== Marker FOV ====================
    if (!oXML.FindChildElem("Marker_FOV")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Left")) {
      return false;
    }
    sCameraSettings.nMarkerFOVLeft = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Top")) {
      return false;
    }
    sCameraSettings.nMarkerFOVTop = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Right")) {
      return false;
    }
    sCameraSettings.nMarkerFOVRight = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Bottom")) {
      return false;
    }
    sCameraSettings.nMarkerFOVBottom = atoi(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Marker_FOV

    // ==================== Video FOV ====================
    if (!oXML.FindChildElem("Video_FOV")) {
      return false;
    }
    oXML.IntoElem();

    if (!oXML.FindChildElem("Left")) {
      return false;
    }
    sCameraSettings.nVideoFOVLeft = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Top")) {
      return false;
    }
    sCameraSettings.nVideoFOVTop = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Right")) {
      return false;
    }
    sCameraSettings.nVideoFOVRight = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Bottom")) {
      return false;
    }
    sCameraSettings.nVideoFOVBottom = atoi(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // Video_FOV

    // ==================== Sync out ====================
    // Only available from protocol version 1.10 and later.
    for (int port = 0; port < 3; port++) {
      char syncOutStr[16];
      sprintf_s(syncOutStr, 16, "Sync_Out%s",
                port == 0 ? "" : (port == 1 ? "2" : "_MT"));
      if (oXML.FindChildElem(syncOutStr)) {
        oXML.IntoElem();

        if (port < 2) {
          if (!oXML.FindChildElem("Mode")) {
            return false;
          }
          tStr = oXML.GetChildData();
          std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);
          if (tStr == "shutter out") {
            sCameraSettings.eSyncOutMode[port] = ModeShutterOut;
          } else if (tStr == "multiplier") {
            sCameraSettings.eSyncOutMode[port] = ModeMultiplier;
          } else if (tStr == "divisor") {
            sCameraSettings.eSyncOutMode[port] = ModeDivisor;
          } else if (tStr == "camera independent") {
            sCameraSettings.eSyncOutMode[port] = ModeActualFreq;
          } else if (tStr == "measurement time") {
            sCameraSettings.eSyncOutMode[port] = ModeMeasurementTime;
          } else if (tStr == "continuous 100hz") {
            sCameraSettings.eSyncOutMode[port] = ModeFixed100Hz;
          } else {
            return false;
          }

          if (sCameraSettings.eSyncOutMode[port] == ModeMultiplier ||
              sCameraSettings.eSyncOutMode[port] == ModeDivisor ||
              sCameraSettings.eSyncOutMode[port] == ModeActualFreq) {
            if (!oXML.FindChildElem("Value")) {
              return false;
            }
            sCameraSettings.nSyncOutValue[port] =
                atoi(oXML.GetChildData().c_str());

            if (!oXML.FindChildElem("Duty_Cycle")) {
              return false;
            }
            sCameraSettings.fSyncOutDutyCycle[port] =
                (float)atof(oXML.GetChildData().c_str());
          }
        }
        if (port == 2 ||
            (sCameraSettings.eSyncOutMode[port] != ModeFixed100Hz)) {
          if (!oXML.FindChildElem("Signal_Polarity")) {
            return false;
          }
          if (CompareNoCase(oXML.GetChildData(), "negative")) {
            sCameraSettings.bSyncOutNegativePolarity[port] = true;
          } else {
            sCameraSettings.bSyncOutNegativePolarity[port] = false;
          }
        }
        oXML.OutOfElem(); // Sync_Out
      } else {
        sCameraSettings.eSyncOutMode[port] = ModeActualFreq;
        sCameraSettings.nSyncOutValue[port] = 0;
        sCameraSettings.fSyncOutDutyCycle[port] = 0;
        sCameraSettings.bSyncOutNegativePolarity[port] = false;
      }
    }
    oXML.OutOfElem(); // Camera

    msGeneralSettings.vsCameras.push_back(sCameraSettings);
  }

  return true;
} // ReadGeneralSettings

bool CRTProtocol::Read3DSettings(bool &bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;
  std::string tStr;

  bDataAvailable = false;

  ms3DSettings.s3DLabels.clear();
  ms3DSettings.pCalibrationTime[0] = 0;

  if (!SendCommand("GetParameters 3D")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetParameters 3D failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "GetParameters 3D returned wrong packet type. Got type %d "
                "expected type 2.",
                eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());

  if (!oXML.FindChildElem("The_3D")) {
    // No 3D data available.
    return true;
  }
  oXML.IntoElem();

  if (!oXML.FindChildElem("AxisUpwards")) {
    return false;
  }
  tStr = oXML.GetChildData();
  std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);

  if (tStr == "+x") {
    ms3DSettings.eAxisUpwards = XPos;
  } else if (tStr == "-x") {
    ms3DSettings.eAxisUpwards = XNeg;
  } else if (tStr == "+y") {
    ms3DSettings.eAxisUpwards = YPos;
  } else if (tStr == "-y") {
    ms3DSettings.eAxisUpwards = YNeg;
  } else if (tStr == "+z") {
    ms3DSettings.eAxisUpwards = ZPos;
  } else if (tStr == "-z") {
    ms3DSettings.eAxisUpwards = ZNeg;
  } else {
    return false;
  }

  if (!oXML.FindChildElem("CalibrationTime")) {
    return false;
  }
  tStr = oXML.GetChildData();
  strcpy_s(ms3DSettings.pCalibrationTime, sizeof(ms3DSettings.pCalibrationTime),
           tStr.c_str());

  if (!oXML.FindChildElem("Labels")) {
    return false;
  }
  unsigned int nNumberOfLabels = atoi(oXML.GetChildData().c_str());

  ms3DSettings.s3DLabels.resize(nNumberOfLabels);
  SSettings3DLabel sLabel;

  for (unsigned int iLabel = 0; iLabel < nNumberOfLabels; iLabel++) {
    if (oXML.FindChildElem("Label")) {
      oXML.IntoElem();
      if (oXML.FindChildElem("Name")) {
        sLabel.oName = oXML.GetChildData();
        if (oXML.FindChildElem("RGBColor")) {
          sLabel.nRGBColor = atoi(oXML.GetChildData().c_str());
        }
        ms3DSettings.s3DLabels[iLabel] = sLabel;
      }
      oXML.OutOfElem();
    } else {
      return false;
    }
  }

  ms3DSettings.sBones.clear();
  if (oXML.FindChildElem("Bones")) {
    oXML.IntoElem();
    while (oXML.FindChildElem("Bone")) {
      oXML.IntoElem();
      SSettingsBone bone;
      bone.fromName = oXML.GetAttrib("From").c_str();
      bone.toName = oXML.GetAttrib("To").c_str();

      bone.color = RGB(246, 249, 124);
      auto colorString = oXML.GetAttrib("Color");
      if (!colorString.empty()) {
        bone.color = atoi(colorString.c_str());
      }
      ms3DSettings.sBones.push_back(bone);
      oXML.OutOfElem();
    }
    oXML.OutOfElem();
  }

  bDataAvailable = true;
  return true;
} // Read3DSettings

bool CRTProtocol::Read6DOFSettings(bool &bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;

  bDataAvailable = false;

  mvs6DOFBodySettings.clear();

  if (!SendCommand("GetParameters 6D")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetParameters 6D failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
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
    return true; // NO 6DOF data available.
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

      oXML.OutOfElem(); // Point
      s6DBodySettings.vsPoints.push_back(sPoint);
    }
    mvs6DOFBodySettings.push_back(s6DBodySettings);
    oXML.OutOfElem(); // Body
  }
  bDataAvailable = true;
  return true;
} // Read6DOFSettings

bool CRTProtocol::ReadGazeVectorSettings(bool &bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;

  bDataAvailable = false;

  mvsGazeVectorSettings.clear();

  if (!SendCommand("GetParameters GazeVector")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "GetParameters GazeVector failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "GetParameters GazeVector returned wrong packet type. Got type "
                "%d expected type 2.",
                eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());

  //
  // Read gaze vectors
  //
  if (!oXML.FindChildElem("Gaze_Vector")) {
    return true; // NO gaze vector data available.
  }
  oXML.IntoElem();

  std::string tGazeVectorName;

  int nGazeVectorCount = 0;

  while (oXML.FindChildElem("Vector")) {
    oXML.IntoElem();

    if (!oXML.FindChildElem("Name")) {
      return false;
    }
    tGazeVectorName = oXML.GetChildData();
    mvsGazeVectorSettings.push_back(tGazeVectorName);
    nGazeVectorCount++;
    oXML.OutOfElem(); // Vector
  }

  bDataAvailable = true;
  return true;
} // ReadGazeVectorSettings

bool CRTProtocol::ReadAnalogSettings(bool &bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;

  bDataAvailable = false;
  mvsAnalogDeviceSettings.clear();

  if (!SendCommand("GetParameters Analog")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetParameters Analog failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "GetParameters Analog returned wrong packet type. Got type %d "
                "expected type 2.",
                eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());

  if (!oXML.FindChildElem("Analog")) {
    // No analog data available.
    return true;
  }

  SAnalogDevice sAnalogDevice;

  oXML.IntoElem();

  if (mnMajorVersion == 1 && mnMinorVersion == 0) {
    sAnalogDevice.nDeviceID = 1; // Always channel 1
    sAnalogDevice.oName = "AnalogDevice";
    if (!oXML.FindChildElem("Channels")) {
      return false;
    }
    sAnalogDevice.nChannels = atoi(oXML.GetChildData().c_str());
    if (!oXML.FindChildElem("Frequency")) {
      return false;
    }
    sAnalogDevice.nFrequency = atoi(oXML.GetChildData().c_str());
    if (!oXML.FindChildElem("Unit")) {
      return false;
    }
    sAnalogDevice.oUnit = oXML.GetChildData();
    if (!oXML.FindChildElem("Range")) {
      return false;
    }
    oXML.IntoElem();
    if (!oXML.FindChildElem("Min")) {
      return false;
    }
    sAnalogDevice.fMinRange = (float)atof(oXML.GetChildData().c_str());
    if (!oXML.FindChildElem("Max")) {
      return false;
    }
    sAnalogDevice.fMaxRange = (float)atof(oXML.GetChildData().c_str());
    mvsAnalogDeviceSettings.push_back(sAnalogDevice);
    bDataAvailable = true;
    return true;
  } else {
    while (oXML.FindChildElem("Device")) {
      sAnalogDevice.voLabels.clear();
      sAnalogDevice.voUnits.clear();
      oXML.IntoElem();
      if (!oXML.FindChildElem("Device_ID")) {
        oXML.OutOfElem(); // Device
        continue;
      }
      sAnalogDevice.nDeviceID = atoi(oXML.GetChildData().c_str());

      if (!oXML.FindChildElem("Device_Name")) {
        oXML.OutOfElem(); // Device
        continue;
      }
      sAnalogDevice.oName = oXML.GetChildData();

      if (!oXML.FindChildElem("Channels")) {
        oXML.OutOfElem(); // Device
        continue;
      }
      sAnalogDevice.nChannels = atoi(oXML.GetChildData().c_str());

      if (!oXML.FindChildElem("Frequency")) {
        oXML.OutOfElem(); // Device
        continue;
      }
      sAnalogDevice.nFrequency = atoi(oXML.GetChildData().c_str());

      if (mnMajorVersion == 1 && mnMinorVersion < 11) {
        if (!oXML.FindChildElem("Unit")) {
          oXML.OutOfElem(); // Device
          continue;
        }
        sAnalogDevice.oUnit = oXML.GetChildData();
      }
      if (!oXML.FindChildElem("Range")) {
        oXML.OutOfElem(); // Device
        continue;
      }
      oXML.IntoElem();

      if (!oXML.FindChildElem("Min")) {
        oXML.OutOfElem(); // Device
        oXML.OutOfElem(); // Range
        continue;
      }
      sAnalogDevice.fMinRange = (float)atof(oXML.GetChildData().c_str());

      if (!oXML.FindChildElem("Max")) {
        oXML.OutOfElem(); // Device
        oXML.OutOfElem(); // Range
        continue;
      }
      sAnalogDevice.fMaxRange = (float)atof(oXML.GetChildData().c_str());
      oXML.OutOfElem(); // Range

      if (mnMajorVersion == 1 && mnMinorVersion < 11) {
        for (unsigned int i = 0; i < sAnalogDevice.nChannels; i++) {
          if (oXML.FindChildElem("Label")) {
            sAnalogDevice.voLabels.push_back(oXML.GetChildData());
          }
        }
        if (sAnalogDevice.voLabels.size() != sAnalogDevice.nChannels) {
          oXML.OutOfElem(); // Device
          continue;
        }
      } else {
        while (oXML.FindChildElem("Channel")) {
          oXML.IntoElem();
          if (oXML.FindChildElem("Label")) {
            sAnalogDevice.voLabels.push_back(oXML.GetChildData());
          }
          if (oXML.FindChildElem("Unit")) {
            sAnalogDevice.voUnits.push_back(oXML.GetChildData());
          }
          oXML.OutOfElem(); // Channel
        }
        if (sAnalogDevice.voLabels.size() != sAnalogDevice.nChannels ||
            sAnalogDevice.voUnits.size() != sAnalogDevice.nChannels) {
          oXML.OutOfElem(); // Device
          continue;
        }
      }
      oXML.OutOfElem(); // Device
      mvsAnalogDeviceSettings.push_back(sAnalogDevice);
      bDataAvailable = true;
    }
  }

  return true;
} // ReadAnalogSettings

bool CRTProtocol::ReadForceSettings(bool &bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;

  bDataAvailable = false;

  msForceSettings.vsForcePlates.clear();

  if (!SendCommand("GetParameters Force")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetParameters Force failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "GetParameters Force returned wrong packet type. Got type %d "
                "expected type 2.",
                eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());
  //
  // Read some force plate parameters
  //
  if (!oXML.FindChildElem("Force")) {
    return true;
  }

  oXML.IntoElem();

  SForcePlate sForcePlate;
  sForcePlate.bValidCalibrationMatrix = false;
  sForcePlate.nCalibrationMatrixRows = 6;
  sForcePlate.nCalibrationMatrixColumns = 6;

  if (!oXML.FindChildElem("Unit_Length")) {
    return false;
  }
  msForceSettings.oUnitLength = oXML.GetChildData();

  if (!oXML.FindChildElem("Unit_Force")) {
    return false;
  }
  msForceSettings.oUnitForce = oXML.GetChildData();

  int iPlate = 1;
  while (oXML.FindChildElem("Plate")) {
    //
    // Get name and type of the plates
    //
    oXML.IntoElem();                             // "Plate"
    if (oXML.FindChildElem("Force_Plate_Index")) // Version 1.7 and earlier.
    {
      sForcePlate.nID = atoi(oXML.GetChildData().c_str());
    } else if (oXML.FindChildElem("Plate_ID")) // Version 1.8 and later.
    {
      sForcePlate.nID = atoi(oXML.GetChildData().c_str());
    } else {
      return false;
    }

    if (oXML.FindChildElem("Analog_Device_ID")) {
      sForcePlate.nAnalogDeviceID = atoi(oXML.GetChildData().c_str());
    } else {
      sForcePlate.nAnalogDeviceID = 0;
    }

    if (!oXML.FindChildElem("Frequency")) {
      return false;
    }
    sForcePlate.nFrequency = atoi(oXML.GetChildData().c_str());

    if (oXML.FindChildElem("Type")) {
      sForcePlate.oType = oXML.GetChildData();
    } else {
      sForcePlate.oType = "unknown";
    }

    if (oXML.FindChildElem("Name")) {
      sForcePlate.oName = oXML.GetChildData();
    } else {
      sForcePlate.oName = Format("#%d", iPlate);
    }

    if (oXML.FindChildElem("Length")) {
      sForcePlate.fLength = (float)atof(oXML.GetChildData().c_str());
    }
    if (oXML.FindChildElem("Width")) {
      sForcePlate.fWidth = (float)atof(oXML.GetChildData().c_str());
    }

    if (oXML.FindChildElem("Location")) {
      oXML.IntoElem();
      if (oXML.FindChildElem("Corner1")) {
        oXML.IntoElem();
        if (oXML.FindChildElem("X")) {
          sForcePlate.asCorner[0].fX = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Y")) {
          sForcePlate.asCorner[0].fY = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Z")) {
          sForcePlate.asCorner[0].fZ = (float)atof(oXML.GetChildData().c_str());
        }
        oXML.OutOfElem();
      }
      if (oXML.FindChildElem("Corner2")) {
        oXML.IntoElem();
        if (oXML.FindChildElem("X")) {
          sForcePlate.asCorner[1].fX = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Y")) {
          sForcePlate.asCorner[1].fY = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Z")) {
          sForcePlate.asCorner[1].fZ = (float)atof(oXML.GetChildData().c_str());
        }
        oXML.OutOfElem();
      }
      if (oXML.FindChildElem("Corner3")) {
        oXML.IntoElem();
        if (oXML.FindChildElem("X")) {
          sForcePlate.asCorner[2].fX = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Y")) {
          sForcePlate.asCorner[2].fY = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Z")) {
          sForcePlate.asCorner[2].fZ = (float)atof(oXML.GetChildData().c_str());
        }
        oXML.OutOfElem();
      }
      if (oXML.FindChildElem("Corner4")) {
        oXML.IntoElem();
        if (oXML.FindChildElem("X")) {
          sForcePlate.asCorner[3].fX = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Y")) {
          sForcePlate.asCorner[3].fY = (float)atof(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("Z")) {
          sForcePlate.asCorner[3].fZ = (float)atof(oXML.GetChildData().c_str());
        }
        oXML.OutOfElem();
      }
      oXML.OutOfElem();
    }

    if (oXML.FindChildElem("Origin")) {
      oXML.IntoElem();
      if (oXML.FindChildElem("X")) {
        sForcePlate.sOrigin.fX = (float)atof(oXML.GetChildData().c_str());
      }
      if (oXML.FindChildElem("Y")) {
        sForcePlate.sOrigin.fY = (float)atof(oXML.GetChildData().c_str());
      }
      if (oXML.FindChildElem("Z")) {
        sForcePlate.sOrigin.fZ = (float)atof(oXML.GetChildData().c_str());
      }
      oXML.OutOfElem();
    }

    sForcePlate.vChannels.clear();
    if (oXML.FindChildElem("Channels")) {
      oXML.IntoElem();
      SForceChannel sForceChannel;
      while (oXML.FindChildElem("Channel")) {
        oXML.IntoElem();
        if (oXML.FindChildElem("Channel_No")) {
          sForceChannel.nChannelNumber = atoi(oXML.GetChildData().c_str());
        }
        if (oXML.FindChildElem("ConversionFactor")) {
          sForceChannel.fConversionFactor =
              (float)atof(oXML.GetChildData().c_str());
        }
        sForcePlate.vChannels.push_back(sForceChannel);
        oXML.OutOfElem();
      }
      oXML.OutOfElem();
    }

    if (oXML.FindChildElem("Calibration_Matrix")) {
      oXML.IntoElem();
      int nRow = 0;

      if (mnMajorVersion == 1 && mnMinorVersion < 12) {
        char strRow[16];
        char strCol[16];
        sprintf_s(strRow, sizeof(strRow), "Row%d", nRow + 1);
        while (oXML.FindChildElem(strRow)) {
          oXML.IntoElem();
          int nCol = 0;
          sprintf_s(strCol, sizeof(strCol), "Col%d", nCol + 1);
          while (oXML.FindChildElem(strCol)) {
            sForcePlate.afCalibrationMatrix[nRow][nCol] =
                (float)atof(oXML.GetChildData().c_str());
            nCol++;
            sprintf_s(strCol, sizeof(strCol), "Col%d", nCol + 1);
          }
          sForcePlate.nCalibrationMatrixColumns = nCol;

          nRow++;
          sprintf_s(strRow, sizeof(strRow), "Row%d", nRow + 1);
          oXML.OutOfElem(); // RowX
        }
      } else {
        //<Rows>
        if (oXML.FindChildElem(_T("Rows"))) {
          oXML.IntoElem();

          while (oXML.FindChildElem(_T("Row"))) {
            oXML.IntoElem();

            //<Columns>
            if (oXML.FindChildElem(_T("Columns"))) {
              oXML.IntoElem();

              int nCol = 0;
              while (oXML.FindChildElem(_T("Column"))) {
                sForcePlate.afCalibrationMatrix[nRow][nCol] =
                    (float)atof(oXML.GetChildData().c_str());
                nCol++;
              }
              sForcePlate.nCalibrationMatrixColumns = nCol;

              nRow++;
              oXML.OutOfElem(); // Columns
            }
            oXML.OutOfElem(); // Row
          }
          oXML.OutOfElem(); // Rows
        }
      }
      sForcePlate.nCalibrationMatrixRows = nRow;
      sForcePlate.bValidCalibrationMatrix = true;

      oXML.OutOfElem(); // "Calibration_Matrix"
    }
    oXML.OutOfElem(); // "Plate"

    bDataAvailable = true;
    msForceSettings.vsForcePlates.push_back(sForcePlate);
  }

  return true;
} // Read force settings

bool CRTProtocol::ReadImageSettings(bool &bDataAvailable) {
  CRTPacket::EPacketType eType;
  CMarkup oXML;

  bDataAvailable = false;

  mvsImageSettings.clear();

  if (!SendCommand("GetParameters Image")) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "GetParameters 6D failed");
    return false;
  }

  if (!ReceiveRTPacket(eType, true)) {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "No response received. Expected XML packet. %s", pTmpStr);
    return false;
  }

  if (eType != CRTPacket::PacketXML) {
    if (eType == CRTPacket::PacketError) {
      sprintf_s(maErrorStr, sizeof(maErrorStr), "%s.",
                mpoRTPacket->GetErrorString());
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "GetParameters Image returned wrong packet type. Got type %d "
                "expected type 2.",
                eType);
    }
    return false;
  }

  oXML.SetDoc(mpoRTPacket->GetXMLString());
  //
  // Read some Image parameters
  //
  if (!oXML.FindChildElem("Image")) {
    return true;
  }
  oXML.IntoElem();

  while (oXML.FindChildElem("Camera")) {
    oXML.IntoElem();

    SImageCamera sImageCamera;

    if (!oXML.FindChildElem("ID")) {
      return false;
    }
    sImageCamera.nID = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Enabled")) {
      return false;
    }
    std::string tStr;
    tStr = oXML.GetChildData();
    std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);

    if (tStr == "true") {
      sImageCamera.bEnabled = true;
    } else {
      sImageCamera.bEnabled = false;
    }

    if (!oXML.FindChildElem("Format")) {
      return false;
    }
    tStr = oXML.GetChildData();
    std::transform(tStr.begin(), tStr.end(), tStr.begin(), ::tolower);

    if (tStr == "rawgrayscale") {
      sImageCamera.eFormat = CRTPacket::FormatRawGrayscale;
    } else if (tStr == "rawbgr") {
      sImageCamera.eFormat = CRTPacket::FormatRawBGR;
    } else if (tStr == "jpg") {
      sImageCamera.eFormat = CRTPacket::FormatJPG;
    } else if (tStr == "png") {
      sImageCamera.eFormat = CRTPacket::FormatPNG;
    } else {
      return false;
    }

    if (!oXML.FindChildElem("Width")) {
      return false;
    }
    sImageCamera.nWidth = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Height")) {
      return false;
    }
    sImageCamera.nHeight = atoi(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Left_Crop")) {
      return false;
    }
    sImageCamera.fCropLeft = (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Top_Crop")) {
      return false;
    }
    sImageCamera.fCropTop = (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Right_Crop")) {
      return false;
    }
    sImageCamera.fCropRight = (float)atof(oXML.GetChildData().c_str());

    if (!oXML.FindChildElem("Bottom_Crop")) {
      return false;
    }
    sImageCamera.fCropBottom = (float)atof(oXML.GetChildData().c_str());

    oXML.OutOfElem(); // "Camera"

    mvsImageSettings.push_back(sImageCamera);
    bDataAvailable = true;
  }

  return true;
} // ReadImageSettings

void CRTProtocol::GetSystemSettings(unsigned int &nCaptureFrequency,
                                    float &fCaptureTime, bool &bStartOnExtTrig,
                                    bool &startOnTrigNO, bool &startOnTrigNC,
                                    bool &startOnTrigSoftware,
                                    EProcessingActions &eProcessingActions,
                                    EProcessingActions &eRtProcessingActions,
                                    EProcessingActions &eReprocessingActions) {
  nCaptureFrequency = msGeneralSettings.nCaptureFrequency;
  fCaptureTime = msGeneralSettings.fCaptureTime;
  bStartOnExtTrig = msGeneralSettings.bStartOnExternalTrigger;
  startOnTrigNO = msGeneralSettings.bStartOnTrigNO;
  startOnTrigNC = msGeneralSettings.bStartOnTrigNC;
  startOnTrigSoftware = msGeneralSettings.bStartOnTrigSoftware;
  eProcessingActions = msGeneralSettings.eProcessingActions;
  eRtProcessingActions = msGeneralSettings.eRtProcessingActions;
  eReprocessingActions = msGeneralSettings.eReprocessingActions;
}

// External time base settings only available in version 1.10 of the rt protocol
// and later
void CRTProtocol::GetExtTimeBaseSettings(
    bool &bEnabled, ESignalSource &eSignalSource, bool &bSignalModePeriodic,
    unsigned int &nFreqMultiplier, unsigned int &nFreqDivisor,
    unsigned int &nFreqTolerance, float &fNominalFrequency, bool &bNegativeEdge,
    unsigned int &nSignalShutterDelay, float &fNonPeriodicTimeout) {
  bEnabled = msGeneralSettings.sExternalTimebase.bEnabled;
  eSignalSource = msGeneralSettings.sExternalTimebase.eSignalSource;
  bSignalModePeriodic = msGeneralSettings.sExternalTimebase.bSignalModePeriodic;
  nFreqMultiplier = msGeneralSettings.sExternalTimebase.nFreqMultiplier;
  nFreqDivisor = msGeneralSettings.sExternalTimebase.nFreqDivisor;
  nFreqTolerance = msGeneralSettings.sExternalTimebase.nFreqTolerance;
  fNominalFrequency = msGeneralSettings.sExternalTimebase.fNominalFrequency;
  bNegativeEdge = msGeneralSettings.sExternalTimebase.bNegativeEdge;
  nSignalShutterDelay = msGeneralSettings.sExternalTimebase.nSignalShutterDelay;
  fNonPeriodicTimeout = msGeneralSettings.sExternalTimebase.fNonPeriodicTimeout;
}

unsigned int CRTProtocol::GetCameraCount() {
  return (unsigned int)msGeneralSettings.vsCameras.size();
}

bool CRTProtocol::GetCameraSettings(unsigned int nCameraIndex,
                                    unsigned int &nID, ECameraModel &eModel,
                                    bool &bUnderwater, bool &bSupportsHwSync,
                                    unsigned int &nSerial, ECameraMode &eMode) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    nID = msGeneralSettings.vsCameras[nCameraIndex].nID;
    eModel = msGeneralSettings.vsCameras[nCameraIndex].eModel;
    bUnderwater = msGeneralSettings.vsCameras[nCameraIndex].bUnderwater;
    bSupportsHwSync = msGeneralSettings.vsCameras[nCameraIndex].bSupportsHwSync;
    nSerial = msGeneralSettings.vsCameras[nCameraIndex].nSerial;
    eMode = msGeneralSettings.vsCameras[nCameraIndex].eMode;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraMarkerSettings(unsigned int nCameraIndex,
                                          unsigned int &nCurrentExposure,
                                          unsigned int &nMinExposure,
                                          unsigned int &nMaxExposure,
                                          unsigned int &nCurrentThreshold,
                                          unsigned int &nMinThreshold,
                                          unsigned int &nMaxThreshold) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    nCurrentExposure =
        msGeneralSettings.vsCameras[nCameraIndex].nMarkerExposure;
    nMinExposure = msGeneralSettings.vsCameras[nCameraIndex].nMarkerExposureMin;
    nMaxExposure = msGeneralSettings.vsCameras[nCameraIndex].nMarkerExposureMax;
    nCurrentThreshold =
        msGeneralSettings.vsCameras[nCameraIndex].nMarkerThreshold;
    nMinThreshold =
        msGeneralSettings.vsCameras[nCameraIndex].nMarkerThresholdMin;
    nMaxThreshold =
        msGeneralSettings.vsCameras[nCameraIndex].nMarkerThresholdMax;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraVideoSettings(
    unsigned int nCameraIndex, ECameraVideoMode &eVideoMode,
    unsigned int &nVideoFrequency, unsigned int &nCurrentExposure,
    unsigned int &nMinExposure, unsigned int &nMaxExposure,
    unsigned int &nCurrentFlashTime, unsigned int &nMinFlashTime,
    unsigned int &nMaxFlashTime) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    nVideoFrequency = msGeneralSettings.vsCameras[nCameraIndex].nVideoFrequency;
    eVideoMode = msGeneralSettings.vsCameras[nCameraIndex].eVideoMode;
    nCurrentExposure = msGeneralSettings.vsCameras[nCameraIndex].nVideoExposure;
    nMinExposure = msGeneralSettings.vsCameras[nCameraIndex].nVideoExposureMin;
    nMaxExposure = msGeneralSettings.vsCameras[nCameraIndex].nVideoExposureMax;
    nCurrentFlashTime =
        msGeneralSettings.vsCameras[nCameraIndex].nVideoFlashTime;
    nMinFlashTime =
        msGeneralSettings.vsCameras[nCameraIndex].nVideoFlashTimeMin;
    nMaxFlashTime =
        msGeneralSettings.vsCameras[nCameraIndex].nVideoFlashTimeMax;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraSyncOutSettings(unsigned int nCameraIndex,
                                           unsigned int portNumber,
                                           ESyncOutFreqMode &eSyncOutMode,
                                           unsigned int &nSyncOutValue,
                                           float &fSyncOutDutyCycle,
                                           bool &bSyncOutNegativePolarity) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    if (portNumber == 1 || portNumber == 2) {
      eSyncOutMode = msGeneralSettings.vsCameras[nCameraIndex]
                         .eSyncOutMode[portNumber - 1];
      nSyncOutValue = msGeneralSettings.vsCameras[nCameraIndex]
                          .nSyncOutValue[portNumber - 1];
      fSyncOutDutyCycle = msGeneralSettings.vsCameras[nCameraIndex]
                              .fSyncOutDutyCycle[portNumber - 1];
    }
    if (portNumber > 0 && portNumber < 4) {
      bSyncOutNegativePolarity = msGeneralSettings.vsCameras[nCameraIndex]
                                     .bSyncOutNegativePolarity[portNumber - 1];
    } else {
      maErrorStr[0] = 0; // No error string.
      return false;
    }
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraPosition(unsigned int nCameraIndex, SPoint &sPoint,
                                    float fvRotationMatrix[3][3]) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    sPoint.fX = msGeneralSettings.vsCameras[nCameraIndex].fPositionX;
    sPoint.fY = msGeneralSettings.vsCameras[nCameraIndex].fPositionY;
    sPoint.fZ = msGeneralSettings.vsCameras[nCameraIndex].fPositionZ;
    memcpy_s(fvRotationMatrix, 9 * sizeof(float),
             msGeneralSettings.vsCameras[nCameraIndex].fPositionRotMatrix,
             9 * sizeof(float));
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraOrientation(unsigned int nCameraIndex,
                                       int &nOrientation) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    nOrientation = msGeneralSettings.vsCameras[nCameraIndex].nOrientation;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraResolution(unsigned int nCameraIndex,
                                      unsigned int &nMarkerWidth,
                                      unsigned int &nMarkerHeight,
                                      unsigned int &nVideoWidth,
                                      unsigned int &nVideoHeight) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    nMarkerWidth =
        msGeneralSettings.vsCameras[nCameraIndex].nMarkerResolutionWidth;
    nMarkerHeight =
        msGeneralSettings.vsCameras[nCameraIndex].nMarkerResolutionHeight;
    nVideoWidth =
        msGeneralSettings.vsCameras[nCameraIndex].nVideoResolutionWidth;
    nVideoHeight =
        msGeneralSettings.vsCameras[nCameraIndex].nVideoResolutionHeight;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetCameraFOV(
    unsigned int nCameraIndex, unsigned int &nMarkerLeft,
    unsigned int &nMarkerTop, unsigned int &nMarkerRight,
    unsigned int &nMarkerBottom, unsigned int &nVideoLeft,
    unsigned int &nVideoTop, unsigned int &nVideoRight,
    unsigned int &nVideoBottom) {
  if (nCameraIndex < msGeneralSettings.vsCameras.size()) {
    nMarkerLeft = msGeneralSettings.vsCameras[nCameraIndex].nMarkerFOVLeft;
    nMarkerTop = msGeneralSettings.vsCameras[nCameraIndex].nMarkerFOVTop;
    nMarkerRight = msGeneralSettings.vsCameras[nCameraIndex].nMarkerFOVRight;
    nMarkerBottom = msGeneralSettings.vsCameras[nCameraIndex].nMarkerFOVBottom;
    nVideoLeft = msGeneralSettings.vsCameras[nCameraIndex].nVideoFOVLeft;
    nVideoTop = msGeneralSettings.vsCameras[nCameraIndex].nVideoFOVTop;
    nVideoRight = msGeneralSettings.vsCameras[nCameraIndex].nVideoFOVRight;
    nVideoBottom = msGeneralSettings.vsCameras[nCameraIndex].nVideoFOVBottom;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

CRTProtocol::EAxis CRTProtocol::Get3DUpwardAxis() const {
  return ms3DSettings.eAxisUpwards;
}

const char *CRTProtocol::Get3DCalibrated() const {
  return ms3DSettings.pCalibrationTime;
}

unsigned int CRTProtocol::Get3DLabeledMarkerCount() const {
  return (unsigned int)ms3DSettings.s3DLabels.size();
}

const char *CRTProtocol::Get3DLabelName(unsigned int nMarkerIndex) {
  if (nMarkerIndex < ms3DSettings.s3DLabels.size()) {
    return ms3DSettings.s3DLabels[nMarkerIndex].oName.c_str();
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

unsigned int CRTProtocol::Get3DLabelColor(unsigned int nMarkerIndex) {
  if (nMarkerIndex < ms3DSettings.s3DLabels.size()) {
    return ms3DSettings.s3DLabels[nMarkerIndex].nRGBColor;
  }

  maErrorStr[0] = 0; // No error string.
  return 0;
}

unsigned int CRTProtocol::Get3DBoneCount() const {
  return (unsigned int)ms3DSettings.sBones.size();
}

const char *CRTProtocol::Get3DBoneFromName(unsigned int boneIndex) {
  if (boneIndex < ms3DSettings.sBones.size()) {
    return ms3DSettings.sBones[boneIndex].fromName.c_str();
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

const char *CRTProtocol::Get3DBoneToName(unsigned int boneIndex) {
  if (boneIndex < ms3DSettings.sBones.size()) {
    return ms3DSettings.sBones[boneIndex].toName.c_str();
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

unsigned int CRTProtocol::Get6DOFBodyCount() const {
  return (unsigned int)mvs6DOFBodySettings.size();
}

const char *CRTProtocol::Get6DOFBodyName(unsigned int nBodyIndex) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    return mvs6DOFBodySettings[nBodyIndex].oName.c_str();
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

unsigned int CRTProtocol::Get6DOFBodyColor(unsigned int nBodyIndex) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    return mvs6DOFBodySettings[nBodyIndex].nRGBColor;
  }

  maErrorStr[0] = 0; // No error string.
  return 0;
}

unsigned int CRTProtocol::Get6DOFBodyPointCount(unsigned int nBodyIndex) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    return (unsigned int)mvs6DOFBodySettings.at(nBodyIndex).vsPoints.size();
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::Get6DOFBodyPoint(unsigned int nBodyIndex,
                                   unsigned int nMarkerIndex, SPoint &sPoint) {
  if (nBodyIndex < mvs6DOFBodySettings.size()) {
    if (nMarkerIndex < mvs6DOFBodySettings.at(nBodyIndex).vsPoints.size()) {
      sPoint.fX = mvs6DOFBodySettings.at(nBodyIndex).vsPoints[nMarkerIndex].fX;
      sPoint.fY = mvs6DOFBodySettings.at(nBodyIndex).vsPoints[nMarkerIndex].fY;
      sPoint.fZ = mvs6DOFBodySettings.at(nBodyIndex).vsPoints[nMarkerIndex].fZ;
      return true;
    }
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

unsigned int CRTProtocol::GetGazeVectorCount() const {
  return (unsigned int)mvsGazeVectorSettings.size();
}

const char *CRTProtocol::GetGazeVectorName(unsigned int nGazeVectorIndex) {
  if (nGazeVectorIndex < mvsGazeVectorSettings.size()) {
    return mvsGazeVectorSettings[nGazeVectorIndex].c_str();
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

unsigned int CRTProtocol::GetAnalogDeviceCount() const {
  return (unsigned int)mvsAnalogDeviceSettings.size();
}

bool CRTProtocol::GetAnalogDevice(unsigned int nDeviceIndex,
                                  unsigned int &nDeviceID,
                                  unsigned int &nChannels, char *&pName,
                                  unsigned int &nFrequency, char *&pUnit,
                                  float &fMinRange, float &fMaxRange) {
  if (nDeviceIndex < mvsAnalogDeviceSettings.size()) {
    nDeviceID = mvsAnalogDeviceSettings.at(nDeviceIndex).nDeviceID;
    pName = (char *)mvsAnalogDeviceSettings.at(nDeviceIndex).oName.c_str();
    nChannels = mvsAnalogDeviceSettings.at(nDeviceIndex).nChannels;
    nFrequency = mvsAnalogDeviceSettings.at(nDeviceIndex).nFrequency;
    pUnit = (char *)mvsAnalogDeviceSettings.at(nDeviceIndex).oUnit.c_str();
    fMinRange = mvsAnalogDeviceSettings.at(nDeviceIndex).fMinRange;
    fMaxRange = mvsAnalogDeviceSettings.at(nDeviceIndex).fMaxRange;

    return true;
  }

  maErrorStr[0] = 0; // No error string.
  return false;
}

const char *CRTProtocol::GetAnalogLabel(unsigned int nDeviceIndex,
                                        unsigned int nChannelIndex) {
  if (nDeviceIndex < mvsAnalogDeviceSettings.size()) {
    if (nChannelIndex <
        mvsAnalogDeviceSettings.at(nDeviceIndex).voLabels.size()) {
      return mvsAnalogDeviceSettings.at(nDeviceIndex)
          .voLabels.at(nChannelIndex)
          .c_str();
    }
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

const char *CRTProtocol::GetAnalogUnit(unsigned int nDeviceIndex,
                                       unsigned int nChannelIndex) {
  if (nDeviceIndex < mvsAnalogDeviceSettings.size()) {
    if (nChannelIndex <
        mvsAnalogDeviceSettings.at(nDeviceIndex).voUnits.size()) {
      return mvsAnalogDeviceSettings.at(nDeviceIndex)
          .voUnits.at(nChannelIndex)
          .c_str();
    }
  }

  maErrorStr[0] = 0; // No error string.
  return NULL;
}

void CRTProtocol::GetForceUnits(char *&pLength, char *&pForce) {
  pLength = (char *)msForceSettings.oUnitLength.c_str();
  pForce = (char *)msForceSettings.oUnitForce.c_str();
}

unsigned int CRTProtocol::GetForcePlateCount() const {
  return (unsigned int)msForceSettings.vsForcePlates.size();
}

bool CRTProtocol::GetForcePlate(unsigned int nPlateIndex, unsigned int &nID,
                                unsigned int &nAnalogDeviceID,
                                unsigned int &nFrequency, char *&pType,
                                char *&pName, float &fLength, float &fWidth) {
  if (nPlateIndex < msForceSettings.vsForcePlates.size()) {
    nID = msForceSettings.vsForcePlates[nPlateIndex].nID;
    nAnalogDeviceID =
        msForceSettings.vsForcePlates[nPlateIndex].nAnalogDeviceID;
    nFrequency = msForceSettings.vsForcePlates[nPlateIndex].nFrequency;
    pType = (char *)msForceSettings.vsForcePlates[nPlateIndex].oType.c_str();
    pName = (char *)msForceSettings.vsForcePlates[nPlateIndex].oName.c_str();
    fLength = msForceSettings.vsForcePlates[nPlateIndex].fLength;
    fWidth = msForceSettings.vsForcePlates[nPlateIndex].fWidth;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetForcePlateLocation(unsigned int nPlateIndex,
                                        SPoint sCorner[4]) {
  if (nPlateIndex < msForceSettings.vsForcePlates.size()) {
    memcpy_s(sCorner, 3 * 4 * sizeof(float),
             msForceSettings.vsForcePlates[nPlateIndex].asCorner,
             3 * 4 * sizeof(float));
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

bool CRTProtocol::GetForcePlateOrigin(unsigned int nPlateIndex,
                                      SPoint &sOrigin) {
  if (nPlateIndex < msForceSettings.vsForcePlates.size()) {
    sOrigin = msForceSettings.vsForcePlates[nPlateIndex].sOrigin;
    return true;
  }
  maErrorStr[0] = 0; // No error string.
  return false;
}

unsigned int CRTProtocol::GetForcePlateChannelCount(unsigned int nPlateIndex) {
  if (nPlateIndex < msForceSettings.vsForcePlates.size()) {
    return (unsigned int)msForceSettings.vsForcePlates[nPlateIndex]
        .vChannels.size();
  }
  maErrorStr[0] = 0; // No error string.
  return 0;
}

bool CRTProtocol::GetForcePlateChannel(unsigned int nPlateIndex,
                                       unsigned int nChannelIndex,
                                       unsigned int &nChannelNumber,
                                       float &fConversionFactor) {
  if (nPlateIndex < msForceSettings.vsForcePlates.size()) {
    if (nChannelIndex <
        msForceSettings.vsForcePlates[nPlateIndex].vChannels.size()) {
      nChannelNumber = msForceSettings.vsForcePlates[nPlateIndex]
                           .vChannels[nChannelIndex]
                           .nChannelNumber;
      fConversionFactor = msForceSettings.vsForcePlates[nPlateIndex]
                              .vChannels[nChannelIndex]
                              .fConversionFactor;
      return true;
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "No channel index % on force plate index %d.", nChannelIndex,
                nPlateIndex);
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "No force plate index %d.",
              nPlateIndex);
  }
  return false;
}

bool CRTProtocol::GetForcePlateCalibrationMatrix(unsigned int nPlateIndex,
                                                 float fvCalMatrix[12][12],
                                                 unsigned int *rows,
                                                 unsigned int *columns) {
  if (nPlateIndex < msForceSettings.vsForcePlates.size()) {
    if (msForceSettings.vsForcePlates[nPlateIndex].bValidCalibrationMatrix) {
      *rows = msForceSettings.vsForcePlates[nPlateIndex].nCalibrationMatrixRows;
      *columns =
          msForceSettings.vsForcePlates[nPlateIndex].nCalibrationMatrixColumns;
      memcpy_s(
          fvCalMatrix,
          msForceSettings.vsForcePlates[nPlateIndex].nCalibrationMatrixRows *
              msForceSettings.vsForcePlates[nPlateIndex]
                  .nCalibrationMatrixColumns *
              sizeof(float),
          msForceSettings.vsForcePlates[nPlateIndex].afCalibrationMatrix,
          msForceSettings.vsForcePlates[nPlateIndex].nCalibrationMatrixRows *
              msForceSettings.vsForcePlates[nPlateIndex]
                  .nCalibrationMatrixColumns *
              sizeof(float));
      return true;
    }
  }
  sprintf_s(maErrorStr, sizeof(maErrorStr),
            "No force plate calibration matrix for camera index %d.",
            nPlateIndex);
  return false;
}

unsigned int CRTProtocol::GetImageCameraCount() {
  return (unsigned int)mvsImageSettings.size();
}

bool CRTProtocol::GetImageCamera(unsigned int nCameraIndex,
                                 unsigned int &nCameraID, bool &bEnabled,
                                 CRTPacket::EImageFormat &eFormat,
                                 unsigned int &nWidth, unsigned int &nHeight,
                                 float &fCropLeft, float &fCropTop,
                                 float &fCropRight, float &fCropBottom) {
  if (nCameraIndex < mvsImageSettings.size()) {
    nCameraID = mvsImageSettings[nCameraIndex].nID;
    bEnabled = mvsImageSettings[nCameraIndex].bEnabled;
    eFormat = mvsImageSettings[nCameraIndex].eFormat;
    nWidth = mvsImageSettings[nCameraIndex].nWidth;
    nHeight = mvsImageSettings[nCameraIndex].nHeight;
    fCropLeft = mvsImageSettings[nCameraIndex].fCropLeft;
    fCropTop = mvsImageSettings[nCameraIndex].fCropTop;
    fCropRight = mvsImageSettings[nCameraIndex].fCropRight;
    fCropBottom = mvsImageSettings[nCameraIndex].fCropBottom;
    return true;
  }
  sprintf_s(maErrorStr, sizeof(maErrorStr),
            "No camera image settings for camera index %d.", nCameraIndex);
  return false;
}

CRTProtocol::ECameraSystemType CRTProtocol::GetCameraSystemType() {
  return msGeneralSettings.sCameraSystem.eType;
}

bool CRTProtocol::SetSystemSettings(
    const unsigned int *pnCaptureFrequency, const float *pfCaptureTime,
    const bool *pbStartOnExtTrig, const bool *startOnTrigNO,
    const bool *startOnTrigNC, const bool *startOnTrigSoftware,
    const EProcessingActions *peProcessingActions,
    const EProcessingActions *peRtProcessingActions,
    const EProcessingActions *peReprocessingActions) {
  CMarkup oXML;

  oXML.AddElem("QTM_Settings");
  oXML.IntoElem();
  oXML.AddElem("General");
  oXML.IntoElem();

  if (pnCaptureFrequency) {
    AddXMLElementUnsignedInt(&oXML, "Frequency", pnCaptureFrequency);
  }
  if (pfCaptureTime) {
    AddXMLElementFloat(&oXML, "Capture_Time", pfCaptureTime, 3);
  }
  if (pbStartOnExtTrig) {
    AddXMLElementBool(&oXML, "Start_On_External_Trigger", pbStartOnExtTrig);
    if (mnMajorVersion > 1 || mnMinorVersion > 14) {
      AddXMLElementBool(&oXML, "Start_On_Trigger_NO", startOnTrigNO);
      AddXMLElementBool(&oXML, "Start_On_Trigger_NC", startOnTrigNC);
      AddXMLElementBool(&oXML, "Start_On_Trigger_Software",
                        startOnTrigSoftware);
    }
  }

  const _TCHAR *processings[3] = {"Processing_Actions",
                                  "RealTime_Processing_Actions",
                                  "Reprocessing_Actions"};
  const EProcessingActions *processingActions[3] = {
      peProcessingActions, peRtProcessingActions, peReprocessingActions};

  auto actionsCount = (mnMajorVersion > 1 || mnMinorVersion > 13) ? 3 : 1;

  for (auto i = 0; i < actionsCount; i++) {
    if (processingActions[i]) {
      oXML.AddElem(processings[i]);
      oXML.IntoElem();

      if (mnMajorVersion > 1 || mnMinorVersion > 13) {
        AddXMLElementBool(&oXML, "PreProcessing2D",
                          (*processingActions[i] & ProcessingPreProcess2D) !=
                              0);
      }
      if (*processingActions[i] & ProcessingTracking2D &&
          i != 1) // i != 1 => Not RtProcessingSettings
      {
        oXML.AddElem("Tracking", "2D");
      } else if (*processingActions[i] & ProcessingTracking3D) {
        oXML.AddElem("Tracking", "3D");
      } else {
        oXML.AddElem("Tracking", "False");
      }
      if (i != 1) // Not RtProcessingSettings
      {
        AddXMLElementBool(&oXML, "TwinSystemMerge",
                          (*processingActions[i] & ProcessingTwinSystemMerge) !=
                              0);
        AddXMLElementBool(&oXML, "SplineFill",
                          (*processingActions[i] & ProcessingSplineFill) != 0);
      }
      AddXMLElementBool(&oXML, "AIM",
                        (*processingActions[i] & ProcessingAIM) != 0);
      AddXMLElementBool(&oXML, "Track6DOF",
                        (*processingActions[i] & Processing6DOFTracking) != 0);
      AddXMLElementBool(&oXML, "ForceData",
                        (*processingActions[i] & ProcessingForceData) != 0);
      AddXMLElementBool(&oXML, "GazeVector",
                        (*processingActions[i] & ProcessingGazeVector) != 0);
      if (i != 1) // Not RtProcessingSettings
      {
        AddXMLElementBool(&oXML, "ExportTSV",
                          (*processingActions[i] & ProcessingExportTSV) != 0);
        AddXMLElementBool(&oXML, "ExportC3D",
                          (*processingActions[i] & ProcessingExportC3D) != 0);
        AddXMLElementBool(
            &oXML, "ExportMatlabFile",
            (*processingActions[i] & ProcessingExportMatlabFile) != 0);
        AddXMLElementBool(&oXML, "ExportAviFile",
                          (*processingActions[i] & ProcessingExportAviFile) !=
                              0);
      }
      oXML.OutOfElem(); // Processing_Actions
    }
  }
  oXML.OutOfElem(); // General
  oXML.OutOfElem(); // QTM_Settings

  if (SendXML(oXML.GetDoc().c_str())) {
    return true;
  }

  return false;
} // SetGeneral

bool CRTProtocol::SetExtTimeBaseSettings(
    const bool *pbEnabled, const ESignalSource *peSignalSource,
    const bool *pbSignalModePeriodic, const unsigned int *pnFreqMultiplier,
    const unsigned int *pnFreqDivisor, const unsigned int *pnFreqTolerance,
    const float *pfNominalFrequency, const bool *pbNegativeEdge,
    const unsigned int *pnSignalShutterDelay,
    const float *pfNonPeriodicTimeout) {
  CMarkup oXML;

  oXML.AddElem("QTM_Settings");
  oXML.IntoElem();
  oXML.AddElem("General");
  oXML.IntoElem();
  oXML.AddElem("External_Time_Base");
  oXML.IntoElem();

  AddXMLElementBool(&oXML, "Enabled", pbEnabled);

  if (peSignalSource) {
    switch (*peSignalSource) {
    case SourceControlPort:
      oXML.AddElem("Signal_Source", "Control port");
      break;
    case SourceIRReceiver:
      oXML.AddElem("Signal_Source", "IR receiver");
      break;
    case SourceSMPTE:
      oXML.AddElem("Signal_Source", "SMPTE");
      break;
    case SourceVideoSync:
      oXML.AddElem("Signal_Source", "Video sync");
      break;
    }
  }

  AddXMLElementBool(&oXML, "Signal_Mode", pbSignalModePeriodic, "Periodic",
                    "Non-periodic");
  AddXMLElementUnsignedInt(&oXML, "Frequency_Multiplier", pnFreqMultiplier);
  AddXMLElementUnsignedInt(&oXML, "Frequency_Divisor", pnFreqDivisor);
  AddXMLElementUnsignedInt(&oXML, "Frequency_Tolerance", pnFreqTolerance);

  if (pfNominalFrequency) {
    if (*pfNominalFrequency < 0) {
      oXML.AddElem("Nominal_Frequency", "None");
    } else {
      AddXMLElementFloat(&oXML, "Nominal_Frequency", pfNominalFrequency, 3);
    }
  }

  AddXMLElementBool(&oXML, "Signal_Edge", pbNegativeEdge, "Negative",
                    "Positive");
  AddXMLElementUnsignedInt(&oXML, "Signal_Shutter_Delay", pnSignalShutterDelay);
  AddXMLElementFloat(&oXML, "Non_Periodic_Timeout", pfNonPeriodicTimeout, 3);

  oXML.OutOfElem(); // External_Time_Base
  oXML.OutOfElem(); // General
  oXML.OutOfElem(); // QTM_Settings

  if (SendXML(oXML.GetDoc().c_str())) {
    return true;
  }

  return false;
} // SetGeneralExtTimeBase

// nCameraID starts on 1. If nCameraID < 0 then settings are applied to all
// cameras.
bool CRTProtocol::SetCameraSettings(const unsigned int nCameraID,
                                    const ECameraMode *peMode,
                                    const float *pfMarkerExposure,
                                    const float *pfMarkerThreshold,
                                    const int *pnOrientation) {
  CMarkup oXML;

  oXML.AddElem("QTM_Settings");
  oXML.IntoElem();
  oXML.AddElem("General");
  oXML.IntoElem();

  oXML.AddElem("Camera");
  oXML.IntoElem();

  AddXMLElementUnsignedInt(&oXML, "ID", &nCameraID);

  if (peMode) {
    switch (*peMode) {
    case ModeMarker:
      oXML.AddElem("Mode", "Marker");
      break;
    case ModeMarkerIntensity:
      oXML.AddElem("Mode", "Marker Intensity");
      break;
    case ModeVideo:
      oXML.AddElem("Mode", "Video");
      break;
    }
  }
  AddXMLElementFloat(&oXML, "Marker_Exposure", pfMarkerExposure);
  AddXMLElementFloat(&oXML, "Marker_Threshold", pfMarkerThreshold);
  AddXMLElementInt(&oXML, "Orientation", pnOrientation);

  oXML.OutOfElem(); // Camera
  oXML.OutOfElem(); // General
  oXML.OutOfElem(); // QTM_Settings

  if (SendXML(oXML.GetDoc().c_str())) {
    return true;
  }

  return false;
} // SetGeneralCamera

// nCameraID starts on 1. If nCameraID < 0 then settings are applied to all
// cameras.
bool CRTProtocol::SetCameraVideoSettings(const unsigned int nCameraID,
                                         const ECameraVideoMode *peVideoMode,
                                         const unsigned int *pnVideoFrequency,
                                         const float *pfVideoExposure,
                                         const float *pfVideoFlashTime) {
  CMarkup oXML;

  oXML.AddElem("QTM_Settings");
  oXML.IntoElem();
  oXML.AddElem("General");
  oXML.IntoElem();

  oXML.AddElem("Camera");
  oXML.IntoElem();

  AddXMLElementUnsignedInt(&oXML, "ID", &nCameraID);

  if (peVideoMode) {
    switch (*peVideoMode) {
    case VideoModeCustom:
      oXML.AddElem("Video_Mode", "Custom");
      break;
    case VideoMode1080p_24hz:
      oXML.AddElem("Video_Mode", "1080p_24hz");
      break;
    case VideoMode720p_25hz:
      oXML.AddElem("Video_Mode", "720p_25hz");
      break;
    case VideoMode720p_50hz:
      oXML.AddElem("Video_Mode", "720p_50hz");
      break;
    case VideoMode540p_25hz:
      oXML.AddElem("Video_Mode", "540p_25hz");
      break;
    case VideoMode540p_50hz:
      oXML.AddElem("Video_Mode", "540p_50hz");
      break;
    case VideoMode540p_60hz:
      oXML.AddElem("Video_Mode", "540p_60hz");
      break;
    case VideoMode480p_25hz:
      oXML.AddElem("Video_Mode", "480p_25hz");
      break;
    case VideoMode480p_50hz:
      oXML.AddElem("Video_Mode", "480p_50hz");
      break;
    case VideoMode480p_60hz:
      oXML.AddElem("Video_Mode", "480p_60hz");
      break;
    case VideoMode480p_120hz:
      oXML.AddElem("Video_Mode", "480p_120hz");
      break;
    }
  }
  AddXMLElementUnsignedInt(&oXML, "Video_Frequency", pnVideoFrequency);
  AddXMLElementFloat(&oXML, "Video_Exposure", pfVideoExposure);
  AddXMLElementFloat(&oXML, "Video_Flash_Time", pfVideoFlashTime);

  oXML.OutOfElem(); // Camera
  oXML.OutOfElem(); // General
  oXML.OutOfElem(); // QTM_Settings

  if (SendXML(oXML.GetDoc().c_str())) {
    return true;
  }

  return false;
} // SetGeneralCameraVideo

// nCameraID starts on 1. If nCameraID < 0 then settings are applied to all
// cameras.
bool CRTProtocol::SetCameraSyncOutSettings(
    const unsigned int nCameraID, const unsigned int portNumber,
    const ESyncOutFreqMode *peSyncOutMode, const unsigned int *pnSyncOutValue,
    const float *pfSyncOutDutyCycle, const bool *pbSyncOutNegativePolarity) {
  CMarkup oXML;

  oXML.AddElem("QTM_Settings");
  oXML.IntoElem();
  oXML.AddElem("General");
  oXML.IntoElem();

  oXML.AddElem("Camera");
  oXML.IntoElem();

  AddXMLElementUnsignedInt(&oXML, "ID", &nCameraID);

  int port = portNumber - 1;
  if (((port == 0 || port == 1) && peSyncOutMode) || (port == 2)) {
    oXML.AddElem(port == 0 ? "Sync_Out"
                           : (port == 1 ? "Sync_Out2" : "Sync_Out_MT"));
    oXML.IntoElem();

    if (port == 0 || port == 1) {
      switch (*peSyncOutMode) {
      case ModeShutterOut:
        oXML.AddElem("Mode", "Shutter out");
        break;
      case ModeMultiplier:
        oXML.AddElem("Mode", "Multiplier");
        break;
      case ModeDivisor:
        oXML.AddElem("Mode", "Divisor");
        break;
      case ModeActualFreq:
        oXML.AddElem("Mode", "Camera independent");
        break;
      case ModeMeasurementTime:
        oXML.AddElem("Mode", "Measurement time");
        break;
      case ModeFixed100Hz:
        oXML.AddElem("Mode", "Continuous 100Hz");
        break;
      default:
        return false; // Should never happen
      }

      if (*peSyncOutMode == ModeMultiplier || *peSyncOutMode == ModeDivisor ||
          *peSyncOutMode == ModeActualFreq) {
        if (pnSyncOutValue) {
          AddXMLElementUnsignedInt(&oXML, "Value", pnSyncOutValue);
        }
        if (pfSyncOutDutyCycle) {
          AddXMLElementFloat(&oXML, "Duty_Cycle", pfSyncOutDutyCycle, 3);
        }
      }
    }
    if (pbSyncOutNegativePolarity &&
        (port == 2 || (peSyncOutMode && *peSyncOutMode != ModeFixed100Hz))) {
      AddXMLElementBool(&oXML, "Signal_Polarity", pbSyncOutNegativePolarity,
                        "Negative", "Positive");
    }
    oXML.OutOfElem(); // Sync_Out
  }
  oXML.OutOfElem(); // Camera
  oXML.OutOfElem(); // General
  oXML.OutOfElem(); // QTM_Settings

  if (SendXML(oXML.GetDoc().c_str())) {
    return true;
  }

  return false;
} // SetGeneralCameraSyncOut

bool CRTProtocol::SetImageSettings(
    const unsigned int nCameraID, const bool *pbEnable,
    const CRTPacket::EImageFormat *peFormat, const unsigned int *pnWidth,
    const unsigned int *pnHeight, const float *pfLeftCrop,
    const float *pfTopCrop, const float *pfRightCrop,
    const float *pfBottomCrop) {
  CMarkup oXML;

  oXML.AddElem("QTM_Settings");
  oXML.IntoElem();
  oXML.AddElem("Image");
  oXML.IntoElem();

  oXML.AddElem("Camera");
  oXML.IntoElem();

  AddXMLElementUnsignedInt(&oXML, "ID", &nCameraID);

  AddXMLElementBool(&oXML, "Enabled", pbEnable);

  if (peFormat) {
    switch (*peFormat) {
    case CRTPacket::FormatRawGrayscale:
      oXML.AddElem("Format", "RAWGrayscale");
      break;
    case CRTPacket::FormatRawBGR:
      oXML.AddElem("Format", "RAWBGR");
      break;
    case CRTPacket::FormatJPG:
      oXML.AddElem("Format", "JPG");
      break;
    case CRTPacket::FormatPNG:
      oXML.AddElem("Format", "PNG");
      break;
    }
  }
  AddXMLElementUnsignedInt(&oXML, "Width", pnWidth);
  AddXMLElementUnsignedInt(&oXML, "Height", pnHeight);
  AddXMLElementFloat(&oXML, "Left_Crop", pfLeftCrop);
  AddXMLElementFloat(&oXML, "Top_Crop", pfTopCrop);
  AddXMLElementFloat(&oXML, "Right_Crop", pfRightCrop);
  AddXMLElementFloat(&oXML, "Bottom_Crop", pfBottomCrop);

  oXML.OutOfElem(); // Camera
  oXML.OutOfElem(); // Image
  oXML.OutOfElem(); // QTM_Settings

  if (SendXML(oXML.GetDoc().c_str())) {
    return true;
  }

  return false;
} // SetImageSettings

bool CRTProtocol::SetForceSettings(const unsigned int nPlateID,
                                   const SPoint *psCorner1,
                                   const SPoint *psCorner2,
                                   const SPoint *psCorner3,
                                   const SPoint *psCorner4) {
  CMarkup oXML;

  if (nPlateID > 0) {
    oXML.AddElem("QTM_Settings");
    oXML.IntoElem();
    oXML.AddElem("Force");
    oXML.IntoElem();

    oXML.AddElem("Plate");
    oXML.IntoElem();

    if (mnMajorVersion > 1 || mnMinorVersion > 7) {
      AddXMLElementUnsignedInt(&oXML, "Plate_ID", &nPlateID);
    } else {
      AddXMLElementUnsignedInt(&oXML, "Force_Plate_Index", &nPlateID);
    }
    if (psCorner1) {
      oXML.AddElem("Corner1");
      oXML.IntoElem();
      AddXMLElementFloat(&oXML, "X", &(psCorner1->fX));
      AddXMLElementFloat(&oXML, "Y", &(psCorner1->fY));
      AddXMLElementFloat(&oXML, "Z", &(psCorner1->fZ));
      oXML.OutOfElem(); // Corner1
    }
    if (psCorner2) {
      oXML.AddElem("Corner2");
      oXML.IntoElem();
      AddXMLElementFloat(&oXML, "X", &(psCorner2->fX));
      AddXMLElementFloat(&oXML, "Y", &(psCorner2->fY));
      AddXMLElementFloat(&oXML, "Z", &(psCorner2->fZ));
      oXML.OutOfElem(); // Corner2
    }
    if (psCorner3) {
      oXML.AddElem("Corner3");
      oXML.IntoElem();
      AddXMLElementFloat(&oXML, "X", &(psCorner3->fX));
      AddXMLElementFloat(&oXML, "Y", &(psCorner3->fY));
      AddXMLElementFloat(&oXML, "Z", &(psCorner3->fZ));
      oXML.OutOfElem(); // Corner3
    }
    if (psCorner4) {
      oXML.AddElem("Corner4");
      oXML.IntoElem();
      AddXMLElementFloat(&oXML, "X", &(psCorner4->fX));
      AddXMLElementFloat(&oXML, "Y", &(psCorner4->fY));
      AddXMLElementFloat(&oXML, "Z", &(psCorner4->fZ));
      oXML.OutOfElem(); // Corner4
    }
    oXML.OutOfElem(); // Plate

    oXML.OutOfElem(); // Force
    oXML.OutOfElem(); // QTM_Settings

    if (SendXML(oXML.GetDoc().c_str())) {
      return true;
    }
  } else {
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Illegal force plate id: %d.",
              nPlateID);
  }
  return false;
} // SetForceSettings

char *CRTProtocol::GetErrorString() { return maErrorStr; }

bool CRTProtocol::SendString(const char *pCmdStr, int nType) {
  int nSize;
  int nCmdStrLen = (int)strlen(pCmdStr);
  static char aSendBuffer[5000];

  if (nCmdStrLen > sizeof(aSendBuffer)) {
    sprintf_s(maErrorStr, sizeof(maErrorStr),
              "String is larger than send buffer.");
    return false;
  }

  //
  // Header size + length of the string + terminating null char
  //
  nSize = 8 + nCmdStrLen + 1;

  memcpy(aSendBuffer + 8, pCmdStr, nCmdStrLen + 1);

  if ((mnMajorVersion == 1 && mnMinorVersion == 0) || mbBigEndian) {
    *((unsigned int *)aSendBuffer) = htonl(nSize);
    *((unsigned int *)(aSendBuffer + 4)) = htonl(nType);
  } else {
    *((unsigned int *)aSendBuffer) = nSize;
    *((unsigned int *)(aSendBuffer + 4)) = nType;
  }

  if (mpoNetwork->Send(aSendBuffer, nSize) == false) {
    sprintf_s(maErrorStr, sizeof(maErrorStr), mpoNetwork->GetErrorString());
    return false;
  }

  return true;

} // SendString

bool CRTProtocol::SendCommand(const char *pCmdStr) {
  return SendString(pCmdStr, CRTPacket::PacketCommand);
} // SendCommand

bool CRTProtocol::SendCommand(const char *pCmdStr, char *pCommandResponseStr,
                              unsigned int nCommandResponseLen,
                              unsigned int timeout) {
  CRTPacket::EPacketType eType;

  if (SendString(pCmdStr, CRTPacket::PacketCommand)) {
    while (ReceiveRTPacket(eType, true, timeout) > 0) {
      if (eType == CRTPacket::PacketCommand) {
        strcpy_s(pCommandResponseStr, nCommandResponseLen,
                 mpoRTPacket->GetCommandString());
        return true;
      }
      if (eType == CRTPacket::PacketError) {
        strcpy_s(pCommandResponseStr, nCommandResponseLen,
                 mpoRTPacket->GetErrorString());
        return false;
      }
    }
  } else {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr), "\'%s\' command failed. %s",
              pCmdStr, pTmpStr);
  }
  pCommandResponseStr[0] = 0;
  return false;
} // SendCommand

bool CRTProtocol::SendXML(const char *pCmdStr) {
  CRTPacket::EPacketType eType;

  if (SendString(pCmdStr, CRTPacket::PacketXML)) {
    if (ReceiveRTPacket(eType, true)) {
      if (eType == CRTPacket::PacketCommand) {
        if (strcmp(mpoRTPacket->GetCommandString(),
                   "Setting parameters succeeded") == 0) {
          return true;
        } else {
          sprintf_s(maErrorStr, sizeof(maErrorStr),
                    "Expected command response \"Setting parameters "
                    "succeeded\". Got \"%s\".",
                    mpoRTPacket->GetCommandString());
        }
      } else {
        sprintf_s(maErrorStr, sizeof(maErrorStr),
                  "Expected command response packet. Got packet type %d.",
                  (int)eType);
      }
    } else {
      sprintf_s(maErrorStr, sizeof(maErrorStr),
                "Missing command response packet.");
    }
  } else {
    char pTmpStr[256];
    strcpy_s(pTmpStr, sizeof(pTmpStr), maErrorStr);
    sprintf_s(maErrorStr, sizeof(maErrorStr), "Failed to send XML string. %s",
              pTmpStr);
  }
  return false;
} // SendXML

void CRTProtocol::AddXMLElementBool(CMarkup *oXML, const _TCHAR *tTag,
                                    const bool *pbValue, const _TCHAR *tTrue,
                                    const _TCHAR *tFalse) {
  if (pbValue) {
    oXML->AddElem(tTag, *pbValue ? tTrue : tFalse);
  }
}

void CRTProtocol::AddXMLElementBool(CMarkup *oXML, const _TCHAR *tTag,
                                    const bool pbValue, const _TCHAR *tTrue,
                                    const _TCHAR *tFalse) {
  oXML->AddElem(tTag, pbValue ? tTrue : tFalse);
}

void CRTProtocol::AddXMLElementInt(CMarkup *oXML, const _TCHAR *tTag,
                                   const int *pnValue) {
  if (pnValue) {
    std::string tVal;

    tVal = Format("%d", *pnValue);
    oXML->AddElem(tTag, tVal.c_str());
  }
}

void CRTProtocol::AddXMLElementUnsignedInt(CMarkup *oXML, const _TCHAR *tTag,
                                           const unsigned int *pnValue) {
  if (pnValue) {
    std::string tVal;

    tVal = Format("%u", *pnValue);
    oXML->AddElem(tTag, tVal.c_str());
  }
}

void CRTProtocol::AddXMLElementFloat(CMarkup *oXML, const _TCHAR *tTag,
                                     const float *pfValue,
                                     unsigned int pnDecimals) {
  if (pfValue) {
    std::string tVal;
    char fFormat[10];

    sprintf_s(fFormat, sizeof(fFormat), "%%.%df", pnDecimals);
    tVal = Format(fFormat, *pfValue);
    oXML->AddElem(tTag, tVal.c_str());
  }
}

std::string CRTProtocol::Format(const char *fmt, ...) {
  using std::string;
  using std::vector;

  string retStr("");

  if (NULL != fmt) {
    va_list marker = NULL;

    // initialize variable arguments
    va_start(marker, fmt);

    // Get formatted string length adding one for NULL
    size_t len = _vscprintf(fmt, marker) + 1;

    // Create a char vector to hold the formatted string.
    vector<char> buffer(len, '\0');
    int nWritten = _vsnprintf_s(&buffer[0], buffer.size(), len, fmt, marker);

    if (nWritten > 0) {
      retStr = &buffer[0];
    }

    // Reset variable arguments
    va_end(marker);
  }

  return retStr;
}

bool CRTProtocol::CompareNoCase(std::string tStr1, const _TCHAR *tStr2) {
  std::transform(tStr1.begin(), tStr1.end(), tStr1.begin(), ::tolower);
  return tStr1 == tStr2;
}

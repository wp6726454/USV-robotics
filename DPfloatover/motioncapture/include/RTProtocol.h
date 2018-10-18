#ifndef RTPROTOCOL_H
#define RTPROTOCOL_H

#include <float.h>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include "RTPacket.h"
#include "qtmMarkup.h"
#include "qtmNetwork.h"

//#ifdef EXPORT_DLL
//#define DLL_EXPORT __declspec(dllexport)
//#else
//#define DLL_EXPORT
//#endif

#define DEFAULT_AUTO_DESCOVER_PORT 22226
#define WAIT_FOR_DATA_TIMEOUT 5000000  // 5 s
#define __STDC_WANT_LIB_EXT1__ 1

class CMarkup;

// class DLL_EXPORT CRTProtocol
class CRTProtocol {
 public:
  static const unsigned int cComponent3d = 0x000001;
  static const unsigned int cComponent3dNoLabels = 0x000002;
  static const unsigned int cComponentAnalog = 0x000004;
  static const unsigned int cComponentForce = 0x000008;
  static const unsigned int cComponent6d = 0x000010;
  static const unsigned int cComponent6dEuler = 0x000020;
  static const unsigned int cComponent2d = 0x000040;
  static const unsigned int cComponent2dLin = 0x000080;
  static const unsigned int cComponent3dRes = 0x000100;
  static const unsigned int cComponent3dNoLabelsRes = 0x000200;
  static const unsigned int cComponent6dRes = 0x000400;
  static const unsigned int cComponent6dEulerRes = 0x000800;
  static const unsigned int cComponentAnalogSingle = 0x001000;
  static const unsigned int cComponentImage = 0x002000;
  static const unsigned int cComponentForceSingle = 0x004000;
  static const unsigned int cComponentGazeVector = 0x008000;
  static const unsigned int cComponentAll = 0x00ffff;

  enum EStreamRate {
    RateNone = 0,
    RateAllFrames = 1,
    RateFrequency = 2,
    RateFrequencyDivisor = 3
  };

  enum ECameraModel {
    ModelMacReflex = 0,
    ModelProReflex120 = 1,
    ModelProReflex240 = 2,
    ModelProReflex500 = 3,
    ModelProReflex1000 = 4,
    ModelOqus100 = 5,
    ModelOqus300 = 6,
    ModelOqus300Plus = 7,
    ModelOqus400 = 8,
    ModelOqus500 = 9,
    ModelOqus200C = 10,
    ModelOqus500Plus = 11,
    ModelOqus700 = 12,
    ModelOqus700Plus = 13,
    ModelMiqusM1 = 14,
    ModelMiqusM3 = 15,
    ModelMiqusM5 = 16,
    ModelMiqusSyncUnit = 17
  };

  enum ECameraSystemType { Unknown = 0, Oqus = 1, Miqus = 2 };

  enum ECameraMode { ModeMarker = 0, ModeMarkerIntensity = 1, ModeVideo = 2 };

  enum ECameraVideoMode {
    VideoModeCustom = 0,
    VideoMode1080p_24hz = 1,
    VideoMode720p_25hz = 2,
    VideoMode720p_50hz = 3,
    VideoMode540p_25hz = 4,
    VideoMode540p_50hz = 5,
    VideoMode540p_60hz = 6,
    VideoMode480p_25hz = 7,
    VideoMode480p_50hz = 8,
    VideoMode480p_60hz = 9,
    VideoMode480p_120hz = 10,
  };

  enum ESyncOutFreqMode {
    ModeShutterOut = 1,  // A pulse per frame is sent
    ModeMultiplier,
    ModeDivisor,
    ModeActualFreq,
    ModeMeasurementTime,
    ModeFixed100Hz
  };

  enum ESignalSource {
    SourceControlPort = 0,
    SourceIRReceiver = 1,
    SourceSMPTE = 2,
    SourceVideoSync = 3
  };

  enum EAxis { XPos = 0, XNeg = 1, YPos = 2, YNeg = 3, ZPos = 4, ZNeg = 5 };

  enum EProcessingActions {
    ProcessingNone = 0x0000,
    ProcessingTracking2D = 0x0001,
    ProcessingTracking3D = 0x0002,
    ProcessingTwinSystemMerge = 0x0004,
    ProcessingSplineFill = 0x0008,
    ProcessingAIM = 0x0010,
    Processing6DOFTracking = 0x0020,
    ProcessingForceData = 0x0040,
    ProcessingGazeVector = 0x0080,
    ProcessingExportTSV = 0x0100,
    ProcessingExportC3D = 0x0200,
    ProcessingExportMatlabFile = 0x0800,
    ProcessingExportAviFile = 0x1000,
    ProcessingPreProcess2D = 0x2000
  };

  struct SPoint {
    float fX;
    float fY;
    float fZ;
  };

  struct SDiscoverResponse {
    char pMessage[128];
    unsigned int nAddr;
    unsigned short nBasePort;
  };

 private:
  struct SSettingsGeneralCamera {
    unsigned int nID;
    ECameraModel eModel;
    bool bUnderwater;
    bool bSupportsHwSync;
    unsigned int nSerial;
    ECameraMode eMode;
    ECameraVideoMode eVideoMode;
    unsigned int nVideoFrequency;
    unsigned int nVideoExposure;      // Micro seconds
    unsigned int nVideoExposureMin;   // Micro seconds
    unsigned int nVideoExposureMax;   // Micro seconds
    unsigned int nVideoFlashTime;     // Micro seconds
    unsigned int nVideoFlashTimeMin;  // Micro seconds
    unsigned int nVideoFlashTimeMax;  // Micro seconds
    unsigned int nMarkerExposure;     // Micro seconds
    unsigned int nMarkerExposureMin;  // Micro seconds
    unsigned int nMarkerExposureMax;  // Micro seconds
    unsigned int nMarkerThreshold;
    unsigned int nMarkerThresholdMin;
    unsigned int nMarkerThresholdMax;
    float fPositionX;
    float fPositionY;
    float fPositionZ;
    float fPositionRotMatrix[3][3];
    unsigned int nOrientation;             // Degrees
    unsigned int nMarkerResolutionWidth;   // Sub pixels
    unsigned int nMarkerResolutionHeight;  // Sub pixels
    unsigned int nVideoResolutionWidth;    // Pixels
    unsigned int nVideoResolutionHeight;   // Pixels
    unsigned int nMarkerFOVLeft;           // Pixels
    unsigned int nMarkerFOVTop;            // Pixels
    unsigned int nMarkerFOVRight;          // Pixels
    unsigned int nMarkerFOVBottom;         // Pixels
    unsigned int nVideoFOVLeft;            // Pixels
    unsigned int nVideoFOVTop;             // Pixels
    unsigned int nVideoFOVRight;           // Pixels
    unsigned int nVideoFOVBottom;          // Pixels
    ESyncOutFreqMode eSyncOutMode[2];
    unsigned int nSyncOutValue[2];
    float fSyncOutDutyCycle[2];  // Percent
    bool bSyncOutNegativePolarity[3];
  };

  struct SSettingsGeneralCameraSystem {
    ECameraSystemType eType;
  };

  struct SSettingsGeneralExternalTimebase {
    bool bEnabled;
    ESignalSource eSignalSource;
    bool bSignalModePeriodic;
    unsigned int nFreqMultiplier;
    unsigned int nFreqDivisor;
    unsigned int nFreqTolerance;
    float fNominalFrequency;
    bool bNegativeEdge;
    unsigned int nSignalShutterDelay;
    float fNonPeriodicTimeout;
  };

  struct SSettingsGeneral {
    SSettingsGeneral()
        : nCaptureFrequency(0),
          fCaptureTime(0.0f),
          bStartOnExternalTrigger(false),
          bStartOnTrigNO(false),
          bStartOnTrigNC(false),
          bStartOnTrigSoftware(false),
          eProcessingActions(EProcessingActions::ProcessingNone),
          eRtProcessingActions(EProcessingActions::ProcessingNone),
          eReprocessingActions(EProcessingActions::ProcessingNone) {
      sExternalTimebase = {0};
      sCameraSystem = {Unknown};
    }

    unsigned int nCaptureFrequency;
    float fCaptureTime;
    bool bStartOnExternalTrigger;
    bool bStartOnTrigNO;
    bool bStartOnTrigNC;
    bool bStartOnTrigSoftware;
    SSettingsGeneralCameraSystem sCameraSystem;
    SSettingsGeneralExternalTimebase sExternalTimebase;
    EProcessingActions eProcessingActions;    // Binary flags.
    EProcessingActions eRtProcessingActions;  // Binary flags.
    EProcessingActions eReprocessingActions;  // Binary flags.
    std::vector<SSettingsGeneralCamera> vsCameras;
  };

  struct SSettingsBone {
    std::string fromName;
    std::string toName;
    unsigned int color;
  };

  struct SSettings6DOF {
    std::string oName;
    unsigned int nRGBColor;
    std::vector<SPoint> vsPoints;
  };

 public:
  CRTProtocol();
  ~CRTProtocol();

  bool Connect(char* pServerAddr, unsigned short nPort,
               int nMajorVersion = MAJOR_VERSION,
               int nMinorVersion = MINOR_VERSION, bool bBigEndian = BIG_ENDIAN);
  void Disconnect();
  bool Connected();
  bool SetVersion(int nMajorVersion, int nMinorVersion);
  bool GetVersion(unsigned int& nMajorVersion, unsigned int& nMinorVersion);
  bool GetQTMVersion(char* pVersion, unsigned int nVersionLen);
  bool StreamFrames(EStreamRate eRate, unsigned int nRateArg,
                    unsigned int nComponentType);
  bool StreamFramesStop();
  bool GetState(CRTPacket::EEvent& eEvent, bool bUpdate = true,
                int nTimeout = WAIT_FOR_DATA_TIMEOUT);

  static bool GetComponentString(char* pComponentStr, int nComponentStrLen,
                                 unsigned int nComponentType);

  int ReceiveRTPacket(
      CRTPacket::EPacketType& eType, bool bSkipEvents = true,
      int nTimeout = WAIT_FOR_DATA_TIMEOUT);  // nTimeout < 0 : Blocking receive
  CRTPacket* GetRTPacket();

  bool Read6DOFSettings(bool& bDataAvailable);

  unsigned int Get6DOFBodyCount() const;
  const char* Get6DOFBodyName(unsigned int nBodyIndex);
  unsigned int Get6DOFBodyColor(unsigned int nBodyIndex);
  unsigned int Get6DOFBodyPointCount(unsigned int nBodyIndex);
  bool Get6DOFBodyPoint(unsigned int nBodyIndex, unsigned int nMarkerIndex,
                        SPoint& sPoint);
  ECameraSystemType GetCameraSystemType();

  char* GetErrorString();

 private:
  bool SendString(const char* pCmdStr, int nType);
  bool SendCommand(const char* pCmdStr);
  bool SendCommand(const char* pCmdStr, char* pCommandResponseStr,
                   unsigned int nCommandResponseLen,
                   unsigned int timeout = WAIT_FOR_DATA_TIMEOUT);

 private:
  CNetwork* mpoNetwork;
  CRTPacket* mpoRTPacket;
  char maDataBuff[65536];
  CRTPacket::EEvent meLastEvent;
  CRTPacket::EEvent
      meState;  // Same as meLastEvent but without EventCameraSettingsChanged
  int mnMinorVersion;
  int mnMajorVersion;
  bool mbBigEndian;
  SSettingsGeneral msGeneralSettings;
  std::vector<SSettings6DOF> mvs6DOFBodySettings;
  char maErrorStr[1024];
  unsigned short mnBroadcastPort;
  FILE* mpFileBuffer;
  std::vector<SDiscoverResponse> mvsDiscoverResponseList;
};

#endif  // RTPROTOCOL_H
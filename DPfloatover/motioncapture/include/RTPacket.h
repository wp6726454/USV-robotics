#ifndef RTPACKET_H
#define RTPACKET_H

#include <arpa/inet.h>
#include <float.h>
#include <inttypes.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
//#ifdef EXPORT_DLL
//    #define DLL_EXPORT __declspec(dllexport)
//#else
//    #define DLL_EXPORT
//#endif

#define MAJOR_VERSION 1
#define MINOR_VERSION 13

#define MAX_CAMERA_COUNT 256
#define MAX_ANALOG_DEVICE_COUNT 64
#define MAX_FORCE_PLATE_COUNT 64
#define MAX_GAZE_VECTOR_COUNT 64

// class DLL_EXPORT CRTPacket
class CRTPacket {
 public:
  enum EPacketType {
    PacketError = 0,
    PacketCommand = 1,
    PacketXML = 2,
    PacketData = 3,
    PacketNoMoreData = 4,
    PacketC3DFile = 5,
    PacketEvent = 6,
    PacketDiscover = 7,
    PacketQTMFile = 8,
    PacketNone = 9
  };

  enum EComponentType {
    Component3d = 1,
    Component3dNoLabels = 2,
    ComponentAnalog = 3,
    ComponentForce = 4,
    Component6d = 5,
    Component6dEuler = 6,
    Component2d = 7,
    Component2dLin = 8,
    Component3dRes = 9,
    Component3dNoLabelsRes = 10,
    Component6dRes = 11,
    Component6dEulerRes = 12,
    ComponentAnalogSingle = 13,
    ComponentImage = 14,
    ComponentForceSingle = 15,
    ComponentGazeVector = 16,
    ComponentNone = 17
  };

  enum EEvent {
    EventConnected = 1,
    EventConnectionClosed = 2,
    EventCaptureStarted = 3,
    EventCaptureStopped = 4,
    EventCaptureFetchingFinished = 5,  // Not used in version 1.10 and later
    EventCalibrationStarted = 6,
    EventCalibrationStopped = 7,
    EventRTfromFileStarted = 8,
    EventRTfromFileStopped = 9,
    EventWaitingForTrigger = 10,
    EventCameraSettingsChanged = 11,
    EventQTMShuttingDown = 12,
    EventCaptureSaved = 13,
    EventReprocessingStarted = 14,
    EventReprocessingStopped = 15,
    EventNone = 16  // Must be the last. Not actually an event. Just used to
                    // cont number of events.
  };

 public:
  CRTPacket(int nMajorVersion = MAJOR_VERSION,
            int nMinorVersion = MINOR_VERSION, bool bBigEndian = false);
  void GetVersion(unsigned int &nMajorVersion, unsigned int &nMinorVersion);
  void SetVersion(unsigned int nMajorVersion, unsigned int nMinorVersion);
  bool GetEndianness();
  void SetEndianness(bool bBigEndian);
  void ClearData();
  void SetData(char *ptr);
  void GetData(char *&ptr, unsigned int &nSize);

  unsigned int GetSize();
  EPacketType GetType();
  uint64_t GetTimeStamp();
  unsigned int GetFrameNumber();
  static unsigned int GetSize(char *pData, bool bBigEndian = false);
  static EPacketType GetType(char *pData, bool bBigEndian = false);
  static uint64_t GetTimeStamp(char *pData, bool bBigEndian = false);
  static unsigned int GetFrameNumber(char *pData, bool bBigEndian = false);

  unsigned int GetComponentCount();
  unsigned int GetComponentSize(EComponentType eComponent);

  char *GetErrorString();
  char *GetCommandString();
  static char *GetCommandString(char *pData, bool bBigEndian = false);
  char *GetXMLString();
  bool GetEvent(EEvent &eEvent);
  static bool GetEvent(EEvent &eEvent, char *pData, bool bBigEndian = false);

  unsigned short GetDropRate();
  unsigned short GetOutOfSyncRate();

  unsigned int Get2DCameraCount();
  unsigned int Get2DMarkerCount(unsigned int nCameraIndex);
  unsigned char Get2DStatusFlags(unsigned int nCameraIndex);
  bool Get2DMarker(unsigned int nCameraIndex, unsigned int nMarkerIndex,
                   unsigned int &nX, unsigned int &nY,
                   unsigned short &nXDiameter, unsigned short &nYDiameter);

  unsigned int Get2DLinCameraCount();
  unsigned int Get2DLinMarkerCount(unsigned int nCameraIndex);
  unsigned char Get2DLinStatusFlags(unsigned int nCameraIndex);
  bool Get2DLinMarker(unsigned int nCameraIndex, unsigned int nMarkerIndex,
                      unsigned int &nX, unsigned int &nY,
                      unsigned short &nXDiameter, unsigned short &nYDiameter);

  unsigned int Get6DOFBodyCount();
  bool Get6DOFBody(unsigned int nBodyIndex, float &fX, float &fY, float &fZ,
                   float afRotMatrix[9]);

  unsigned int Get6DOFResidualBodyCount();
  bool Get6DOFResidualBody(unsigned int nBodyIndex, float &fX, float &fY,
                           float &fZ, float afRotMatrix[9], float &fResidual);

  unsigned int Get6DOFEulerBodyCount();
  bool Get6DOFEulerBody(unsigned int nBodyIndex, float &fX, float &fY,
                        float &fZ, float &fAng1, float &fAng2, float &fAng3);

  unsigned int Get6DOFEulerResidualBodyCount();
  bool Get6DOFEulerResidualBody(unsigned int nBodyIndex, float &fX, float &fY,
                                float &fZ, float &fAng1, float &fAng2,
                                float &fAng3, float &fResidual);

 private:
  float SetByteOrder(float *pfData);
  double SetByteOrder(double *pfData);
  short SetByteOrder(short *pnData);
  unsigned short SetByteOrder(unsigned short *pnData);
  long SetByteOrder(long *pnData);
  int SetByteOrder(int *pnData);
  unsigned int SetByteOrder(unsigned int *pnData);
  int64_t SetByteOrder(int64_t *pnData);
  uint64_t SetByteOrder(uint64_t *pnData);

 private:
  char *mpData;
  char *mpComponentData[ComponentNone];
  char *mp2DData[MAX_CAMERA_COUNT];
  char *mp2DLinData[MAX_CAMERA_COUNT];
  unsigned int mnComponentCount;
  EComponentType meComponentType;
  unsigned int mn2DCameraCount;
  unsigned int mn2DLinCameraCount;
  int mnMajorVersion;
  int mnMinorVersion;
  bool mbBigEndian;
};  // RTPacket

#endif  // RTPACKET_H
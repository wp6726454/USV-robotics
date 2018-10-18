#include "../include/RTPacket.h"

CRTPacket::CRTPacket(int nMajorVersion, int nMinorVersion, bool bBigEndian) {
  mnMajorVersion = nMajorVersion;
  mnMinorVersion = nMinorVersion;
  mbBigEndian = bBigEndian;

  ClearData();
}

void CRTPacket::GetVersion(unsigned int &nMajorVersion,
                           unsigned int &nMinorVersion) {
  nMajorVersion = mnMajorVersion;
  nMinorVersion = mnMinorVersion;
};

void CRTPacket::SetVersion(unsigned int nMajorVersion,
                           unsigned int nMinorVersion) {
  mnMajorVersion = nMajorVersion;
  mnMinorVersion = nMinorVersion;
};

bool CRTPacket::GetEndianness() { return mbBigEndian; };

void CRTPacket::SetEndianness(bool bBigEndian) { mbBigEndian = bBigEndian; };

void CRTPacket::ClearData() {
  mpData = NULL;
  mnComponentCount = 0;
  mn2DCameraCount = 0;
  mn2DLinCameraCount = 0;
  memset(mpComponentData, 0, ComponentNone * 4);
  memset(mp2DData, 0, MAX_CAMERA_COUNT * 4);
  memset(mp2DLinData, 0, MAX_CAMERA_COUNT * 4);
}

void CRTPacket::SetData(char *ptr) {
  unsigned int nComponent;
  unsigned int nCamera;

  mpData = ptr;

  mnComponentCount = 0;
  mn2DCameraCount = 0;
  mn2DLinCameraCount = 0;

  // Check if it's a data packet
  if (GetType() == PacketData) {
    // Reset all component data pointers
    for (nComponent = 1; nComponent < ComponentNone; nComponent++) {
      mpComponentData[nComponent - 1] = NULL;
    }

    char *pCurrentComponent = mpData + 24;
    unsigned int nComponentType =
        SetByteOrder((unsigned int *)(pCurrentComponent + 4));

    mnComponentCount = SetByteOrder((unsigned int *)(mpData + 20));

    for (nComponent = 1; nComponent <= mnComponentCount && nComponentType > 0 &&
                         nComponentType < ComponentNone;
         nComponent++) {
      mpComponentData[nComponentType - 1] = pCurrentComponent;

      if (nComponentType == Component2d) {
        mn2DCameraCount = SetByteOrder((unsigned int *)(pCurrentComponent + 8));

        mp2DData[0] = pCurrentComponent + 16;
        for (nCamera = 1; nCamera < mn2DCameraCount; nCamera++) {
          if (mnMajorVersion > 1 || mnMinorVersion > 7) {
            mp2DData[nCamera] =
                mp2DData[nCamera - 1] + 5 + Get2DMarkerCount(nCamera - 1) * 12;
          } else {
            mp2DData[nCamera] =
                mp2DData[nCamera - 1] + 4 + Get2DMarkerCount(nCamera - 1) * 12;
          }
        }
      }
      if (nComponentType == Component2dLin) {
        mn2DLinCameraCount =
            SetByteOrder((unsigned int *)(pCurrentComponent + 8));

        mp2DLinData[0] = pCurrentComponent + 16;
        for (nCamera = 1; nCamera < mn2DLinCameraCount; nCamera++) {
          if (mnMajorVersion > 1 || mnMinorVersion > 7) {
            mp2DLinData[nCamera] = mp2DLinData[nCamera - 1] + 5 +
                                   Get2DLinMarkerCount(nCamera - 1) * 12;
          } else {
            mp2DLinData[nCamera] = mp2DLinData[nCamera - 1] + 4 +
                                   Get2DLinMarkerCount(nCamera - 1) * 12;
          }
        }
      }

      pCurrentComponent += SetByteOrder((int *)pCurrentComponent);
      nComponentType = SetByteOrder((unsigned int *)(pCurrentComponent + 4));
    }
  }
}  // SetData

void CRTPacket::GetData(char *&ptr, unsigned int &nSize) {
  if (mpData == NULL) {
    nSize = 0;
  } else {
    ptr = mpData;
    nSize = *((int *)mpData);
  }
}

unsigned int CRTPacket::GetSize() {
  if (mpData == NULL) {
    return 0;
  }
  if (mbBigEndian || (mnMajorVersion == 1 && mnMinorVersion == 0)) {
    return ntohl(*((unsigned int *)mpData));
  }
  return *((unsigned int *)mpData);
}

CRTPacket::EPacketType CRTPacket::GetType() {
  if (GetSize() < 8) {
    return PacketNone;
  }
  if (mbBigEndian || (mnMajorVersion == 1 && mnMinorVersion == 0)) {
    return (EPacketType)ntohl(*(unsigned int *)(mpData + 4));
  }
  return (EPacketType) * ((unsigned int *)(mpData + 4));
}

uint64_t CRTPacket::GetTimeStamp() {
  if (GetType() == PacketData) {
    return SetByteOrder((int64_t *)(mpData + 8));
  }
  return 0;
}

unsigned int CRTPacket::GetFrameNumber() {
  if (GetType() == PacketData) {
    return SetByteOrder((unsigned int *)(mpData + 16));
  }
  return 0;
}

unsigned int CRTPacket::GetSize(char *pData, bool bBigEndian) {
  if (bBigEndian) {
    return ntohl(*((unsigned int *)pData));
  }
  return *((unsigned int *)pData);
}

CRTPacket::EPacketType CRTPacket::GetType(char *pData, bool bBigEndian) {
  if (GetSize(pData, bBigEndian) < 8) {
    return PacketNone;
  }
  if (bBigEndian) {
    return (EPacketType)ntohl(*(unsigned int *)(pData + 4));
  }
  return (EPacketType) * ((unsigned int *)(pData + 4));
}

uint64_t CRTPacket::GetTimeStamp(char *pData, bool bBigEndian) {
  if (GetType(pData, bBigEndian) == PacketData) {
    if (bBigEndian) {
      return ((uint64_t)(ntohl((long)*((int64_t *)(pData + 8)))) << 32) +
             ntohl(*((int64_t *)(pData + 8)) >> 32);
    }
    return *((int64_t *)(pData + 8));
  }
  return 0;
}

unsigned int CRTPacket::GetFrameNumber(char *pData, bool bBigEndian) {
  if (GetType(pData, bBigEndian) == PacketData) {
    if (bBigEndian) {
      return ntohl(*((unsigned int *)(pData + 16)));
    }
    return *((unsigned int *)(pData + 16));
  }
  return 0;
}

unsigned int CRTPacket::GetComponentCount() { return mnComponentCount; }

unsigned int CRTPacket::GetComponentSize(EComponentType eComponent) {
  if (eComponent < Component3d || eComponent >= ComponentNone) {
    return 0;
  }
  if (mpComponentData[eComponent - 1] == NULL) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(mpComponentData[eComponent - 1]));
}

char *CRTPacket::GetErrorString() {
  if (GetType() == PacketError) {
    return mpData + 8;
  }
  return NULL;
}

char *CRTPacket::GetCommandString() {
  if (GetType() == PacketCommand) {
    return mpData + 8;
  }
  return NULL;
}

char *CRTPacket::GetCommandString(char *pData, bool bBigEndian) {
  if (GetType(pData, bBigEndian) == PacketCommand) {
    return pData + 8;
  }
  return NULL;
}

char *CRTPacket::GetXMLString() {
  if (GetType() == PacketXML) {
    return mpData + 8;
  }
  return NULL;
}

bool CRTPacket::GetEvent(EEvent &eEvent) {
  if (GetType() == PacketEvent) {
    eEvent = (EEvent) * (mpData + 8);
    return true;
  }
  return false;
}

bool CRTPacket::GetEvent(EEvent &eEvent, char *pData, bool bBigEndian) {
  if (GetType(pData, bBigEndian) == PacketEvent) {
    eEvent = (EEvent) * (pData + 8);
    return true;
  }
  return false;
}

unsigned short CRTPacket::GetDropRate() {
  for (int i = 0; i <= 1; i++) {
    if (mpComponentData[i] != NULL) {
      return SetByteOrder((unsigned short *)(mpComponentData[i] + 12));
    }
  }
  for (int i = 4; i <= 11; i++) {
    if (mpComponentData[i] != NULL) {
      return SetByteOrder((unsigned short *)(mpComponentData[i] + 12));
    }
  }
  return 0;
}

unsigned short CRTPacket::GetOutOfSyncRate() {
  for (int i = 0; i <= 1; i++) {
    if (mpComponentData[i] != NULL) {
      return SetByteOrder((unsigned short *)(mpComponentData[i] + 14));
    }
  }
  for (int i = 4; i <= 11; i++) {
    if (mpComponentData[i] != NULL) {
      return SetByteOrder((unsigned short *)(mpComponentData[i] + 14));
    }
  }
  return 0;
}

//-----------------------------------------------------------
//                         2D
//-----------------------------------------------------------
unsigned int CRTPacket::Get2DCameraCount() { return mn2DCameraCount; }

unsigned int CRTPacket::Get2DMarkerCount(unsigned int nCameraIndex) {
  if (mn2DCameraCount <= nCameraIndex) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(mp2DData[nCameraIndex]));
}

unsigned char CRTPacket::Get2DStatusFlags(unsigned int nCameraIndex) {
  if (mn2DCameraCount > nCameraIndex &&
      (mnMajorVersion > 1 || mnMinorVersion > 7)) {
    return *((unsigned char *)(mp2DData[nCameraIndex] + 4));
  }
  return 0;
}

bool CRTPacket::Get2DMarker(unsigned int nCameraIndex,
                            unsigned int nMarkerIndex, unsigned int &nX,
                            unsigned int &nY, unsigned short &nXDiameter,
                            unsigned short &nYDiameter) {
  int nOffset;

  if (mn2DCameraCount <= nCameraIndex ||
      Get2DMarkerCount(nCameraIndex) <= nMarkerIndex) {
    return false;
  }

  if (mnMajorVersion > 1 || mnMinorVersion > 7) {
    nOffset = 5;
  } else {
    nOffset = 4;
  }
  nX = SetByteOrder(
      (unsigned int *)(mp2DData[nCameraIndex] + nOffset + nMarkerIndex * 12));
  nY = SetByteOrder((unsigned int *)(mp2DData[nCameraIndex] + nOffset + 4 +
                                     nMarkerIndex * 12));
  nXDiameter =
      SetByteOrder((unsigned short *)(mp2DData[nCameraIndex] + nOffset + 8 +
                                      nMarkerIndex * 12));
  nYDiameter =
      SetByteOrder((unsigned short *)(mp2DData[nCameraIndex] + nOffset + 10 +
                                      nMarkerIndex * 12));

  return true;
}

//-----------------------------------------------------------
//                      2D Linearized
//-----------------------------------------------------------
unsigned int CRTPacket::Get2DLinCameraCount() { return mn2DLinCameraCount; }

unsigned int CRTPacket::Get2DLinMarkerCount(unsigned int nCameraIndex) {
  if (mn2DLinCameraCount <= nCameraIndex) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(mp2DLinData[nCameraIndex]));
}

unsigned char CRTPacket::Get2DLinStatusFlags(unsigned int nCameraIndex) {
  if (mn2DLinCameraCount > nCameraIndex &&
      (mnMajorVersion > 1 || mnMinorVersion > 7)) {
    return *((unsigned char *)(mp2DLinData[nCameraIndex] + 4));
  }
  return 0;
}

bool CRTPacket::Get2DLinMarker(unsigned int nCameraIndex,
                               unsigned int nMarkerIndex, unsigned int &nX,
                               unsigned int &nY, unsigned short &nXDiameter,
                               unsigned short &nYDiameter) {
  int nOffset;

  if (mn2DLinCameraCount <= nCameraIndex ||
      Get2DLinMarkerCount(nCameraIndex) <= nMarkerIndex) {
    return false;
  }

  if (mnMajorVersion > 1 || mnMinorVersion > 7) {
    nOffset = 5;
  } else {
    nOffset = 4;
  }
  nX = SetByteOrder((unsigned int *)(mp2DLinData[nCameraIndex] + nOffset +
                                     nMarkerIndex * 12));
  nY = SetByteOrder((unsigned int *)(mp2DLinData[nCameraIndex] + nOffset + 4 +
                                     nMarkerIndex * 12));
  nXDiameter =
      SetByteOrder((unsigned short *)(mp2DLinData[nCameraIndex] + nOffset + 8 +
                                      nMarkerIndex * 12));
  nYDiameter =
      SetByteOrder((unsigned short *)(mp2DLinData[nCameraIndex] + nOffset + 10 +
                                      nMarkerIndex * 12));

  return true;
}

//-----------------------------------------------------------
//                           6DOF
//-----------------------------------------------------------
unsigned int CRTPacket::Get6DOFBodyCount() {
  char *pData = mpComponentData[Component6d - 1];

  if (pData == NULL) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(pData + 8));
}

bool CRTPacket::Get6DOFBody(unsigned int nBodyIndex, float &fX, float &fY,
                            float &fZ, float afRotMatrix[9]) {
  char *pData = mpComponentData[Component6d - 1];

  if (Get6DOFBodyCount() <= nBodyIndex) {
    return false;
  }

  if (mnMajorVersion > 1 || mnMinorVersion > 7) {
    fX = SetByteOrder((float *)(pData + 16 + nBodyIndex * 48));
    fY = SetByteOrder((float *)(pData + 20 + nBodyIndex * 48));
    fZ = SetByteOrder((float *)(pData + 24 + nBodyIndex * 48));
    for (int i = 0; i < 9; i++) {
      afRotMatrix[i] =
          SetByteOrder((float *)(pData + 28 + (i * 4) + nBodyIndex * 48));
    }
  } else {
    fX = (float)SetByteOrder((double *)(pData + 16 + nBodyIndex * 96));
    fY = (float)SetByteOrder((double *)(pData + 24 + nBodyIndex * 96));
    fZ = (float)SetByteOrder((double *)(pData + 32 + nBodyIndex * 96));
    for (int i = 0; i < 9; i++) {
      afRotMatrix[i] = (float)SetByteOrder(
          (double *)(pData + 40 + (i * 4) + nBodyIndex * 96));
    }
  }
  return true;
}

//-----------------------------------------------------------
//                      6DOF Residual
//-----------------------------------------------------------
unsigned int CRTPacket::Get6DOFResidualBodyCount() {
  char *pData = mpComponentData[Component6dRes - 1];

  if (pData == NULL) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(pData + 8));
}

bool CRTPacket::Get6DOFResidualBody(unsigned int nBodyIndex, float &fX,
                                    float &fY, float &fZ, float afRotMatrix[9],
                                    float &fResidual) {
  char *pData = mpComponentData[Component6dRes - 1];

  if (Get6DOFResidualBodyCount() <= nBodyIndex) {
    return false;
  }

  if (mnMajorVersion > 1 || mnMinorVersion > 7) {
    fX = SetByteOrder((float *)(pData + 16 + nBodyIndex * 52));
    fY = SetByteOrder((float *)(pData + 20 + nBodyIndex * 52));
    fZ = SetByteOrder((float *)(pData + 24 + nBodyIndex * 52));
    for (int i = 0; i < 9; i++) {
      afRotMatrix[i] =
          SetByteOrder((float *)(pData + 28 + (i * 4) + nBodyIndex * 52));
    }
    fResidual = SetByteOrder((float *)(pData + 64 + nBodyIndex * 52));
  } else {
    fX = (float)SetByteOrder((double *)(pData + 16 + nBodyIndex * 104));
    fY = (float)SetByteOrder((double *)(pData + 24 + nBodyIndex * 104));
    fZ = (float)SetByteOrder((double *)(pData + 32 + nBodyIndex * 104));
    for (int i = 0; i < 9; i++) {
      afRotMatrix[i] = (float)SetByteOrder(
          (double *)(pData + 40 + (i * 8) + nBodyIndex * 104));
    }
    fResidual = SetByteOrder((float *)(pData + 112 + nBodyIndex * 104));
  }
  return true;
}

//-----------------------------------------------------------
//                       6DOF Euler
//-----------------------------------------------------------
unsigned int CRTPacket::Get6DOFEulerBodyCount() {
  char *pData = mpComponentData[Component6dEuler - 1];

  if (pData == NULL) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(pData + 8));
}

bool CRTPacket::Get6DOFEulerBody(unsigned int nBodyIndex, float &fX, float &fY,
                                 float &fZ, float &fAng1, float &fAng2,
                                 float &fAng3) {
  char *pData = mpComponentData[Component6dEuler - 1];

  if (Get6DOFEulerBodyCount() <= nBodyIndex) {
    return false;
  }

  if (mnMajorVersion > 1 || mnMinorVersion > 7) {
    fX = SetByteOrder((float *)(pData + 16 + nBodyIndex * 24));
    fY = SetByteOrder((float *)(pData + 20 + nBodyIndex * 24));
    fZ = SetByteOrder((float *)(pData + 24 + nBodyIndex * 24));
    fAng1 = SetByteOrder((float *)(pData + 28 + nBodyIndex * 24));
    fAng2 = SetByteOrder((float *)(pData + 32 + nBodyIndex * 24));
    fAng3 = SetByteOrder((float *)(pData + 36 + nBodyIndex * 24));
  } else {
    fX = (float)SetByteOrder((double *)(pData + 16 + nBodyIndex * 48));
    fY = (float)SetByteOrder((double *)(pData + 24 + nBodyIndex * 48));
    fZ = (float)SetByteOrder((double *)(pData + 32 + nBodyIndex * 48));
    fAng1 = (float)SetByteOrder((double *)(pData + 40 + nBodyIndex * 48));
    fAng2 = (float)SetByteOrder((double *)(pData + 48 + nBodyIndex * 48));
    fAng3 = (float)SetByteOrder((double *)(pData + 56 + nBodyIndex * 48));
  }
  return true;
}

//-----------------------------------------------------------
//                    6DOF Euler Residual
//-----------------------------------------------------------
unsigned int CRTPacket::Get6DOFEulerResidualBodyCount() {
  char *pData = mpComponentData[Component6dEulerRes - 1];

  if (pData == NULL) {
    return 0;
  }
  return SetByteOrder((unsigned int *)(pData + 8));
}

bool CRTPacket::Get6DOFEulerResidualBody(unsigned int nBodyIndex, float &fX,
                                         float &fY, float &fZ, float &fAng1,
                                         float &fAng2, float &fAng3,
                                         float &fResidual) {
  char *pData = mpComponentData[Component6dEulerRes - 1];

  if (Get6DOFEulerResidualBodyCount() <= nBodyIndex) {
    return false;
  }

  if (mnMajorVersion > 1 || mnMinorVersion > 7) {
    fX = SetByteOrder((float *)(pData + 16 + nBodyIndex * 28));
    fY = SetByteOrder((float *)(pData + 20 + nBodyIndex * 28));
    fZ = SetByteOrder((float *)(pData + 24 + nBodyIndex * 28));
    fAng1 = SetByteOrder((float *)(pData + 28 + nBodyIndex * 28));
    fAng2 = SetByteOrder((float *)(pData + 32 + nBodyIndex * 28));
    fAng3 = SetByteOrder((float *)(pData + 36 + nBodyIndex * 28));
    fResidual = SetByteOrder((float *)(pData + 40 + nBodyIndex * 28));
  } else {
    fX = (float)SetByteOrder((double *)(pData + 16 + nBodyIndex * 56));
    fY = (float)SetByteOrder((double *)(pData + 24 + nBodyIndex * 56));
    fZ = (float)SetByteOrder((double *)(pData + 32 + nBodyIndex * 56));
    fAng1 = (float)SetByteOrder((double *)(pData + 40 + nBodyIndex * 56));
    fAng2 = (float)SetByteOrder((double *)(pData + 48 + nBodyIndex * 56));
    fAng3 = (float)SetByteOrder((double *)(pData + 56 + nBodyIndex * 56));
    fResidual = SetByteOrder((float *)(pData + 64 + nBodyIndex * 56));
  }
  return true;
}

float CRTPacket::SetByteOrder(float *pfData) {
  unsigned int nTmp;

  if (mbBigEndian) {
    nTmp = ntohl(*((unsigned int *)pfData));
    return *((float *)&nTmp);
  }
  return *pfData;
}  // SetByteOrder

double CRTPacket::SetByteOrder(double *pfData) {
  uint64_t nTmp;

  if (mbBigEndian) {
    nTmp = (((uint64_t)(ntohl((long)*((uint64_t *)pfData))) << 32) +
            ntohl(*((uint64_t *)pfData) >> 32));
    return *((double *)&nTmp);
  }
  return *pfData;
}  // SetByteOrder

short CRTPacket::SetByteOrder(short *pnData) {
  if (mbBigEndian) {
    return ntohs(*pnData);
  }
  return *pnData;
}  // SetByteOrder

unsigned short CRTPacket::SetByteOrder(unsigned short *pnData) {
  if (mbBigEndian) {
    return ntohs(*pnData);
  }
  return *pnData;
}  // SetByteOrder

long CRTPacket::SetByteOrder(long *pnData) {
  if (mbBigEndian) {
    return ntohl(*pnData);
  }
  return *pnData;
}  // SetByteOrder

int CRTPacket::SetByteOrder(int *pnData) {
  if (mbBigEndian) {
    return ntohl(*pnData);
  }
  return *pnData;
}  // SetByteOrder

unsigned int CRTPacket::SetByteOrder(unsigned int *pnData) {
  if (mbBigEndian) {
    return ntohl(*pnData);
  }
  return *pnData;
}  // SetByteOrder

int64_t CRTPacket::SetByteOrder(int64_t *pnData) {
  if (mbBigEndian) {
    return ((uint64_t)(ntohl((long)*pnData)) << 32) + ntohl(*pnData >> 32);
  }
  return *pnData;
}  // SetByteOrder

uint64_t CRTPacket::SetByteOrder(uint64_t *pnData) {
  if (mbBigEndian) {
    return ((uint64_t)(ntohl((long)*pnData)) << 32) + ntohl(*pnData >> 32);
  }
  return *pnData;
}  // SetByteOrder
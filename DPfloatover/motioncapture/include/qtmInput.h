#ifndef QTMINPUT_H
#define QTMINPUT_H

#include "RTProtocol.h"

class CInput {
 public:
  enum EOperation {
    DataTransfer = 1,
    Statistics = 2,
    Noise2D = 3,
    MonitorEvents = 4,
    DiscoverRTServers = 5,
    ControlQTM = 6,
    ViewSettings = 7,
    ChangeGeneralSystemSettings = 8,
    ChangeExtTimebaseSettings = 9,
    ChangeProcessingActionsSettings = 10,
    ChangeCameraSettings = 11,
    ChangeCameraSyncOutSettings = 12,
    ChangeImageSettings = 13,
    ChangeForceSettings = 14,
  };

  enum EQTMCommand {
    New = 1,
    Close = 2,
    Start = 3,
    StartRTFromFile = 4,
    Stop = 5,
    Load = 6,
    Save = 7,
    LoadProject = 8,
    GetCaptureC3D = 9,
    GetCaptureQTM = 10,
    Trig = 11,
    SetQTMEvent = 12,
    Reprocess = 13
  };

 private:
  unsigned int mnMajorVersion;
  unsigned int mnMinorVersion;
};

#endif  // INPUT_H
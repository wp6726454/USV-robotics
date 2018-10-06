#ifndef GLOBALVAR_H
#define GLOBALVAR_H
//#include "../include/Loop.h"
#include "../include/threaded_Loop.h"
#include "../motioncapture/include/motioncapture.h"
#include "dataviewer.h"
#include "gamepadmonitor.h"
#include <QString>

namespace globalvar {
extern dataviewer _dataviewer;
extern QString pwd;
// extern looop _loop;  // select-based concurrent socket
extern threadloop _threadloop; // thread-based concurrent socket
//extern GamepadMonitor mygamepad;
extern QVector<double> vesselshape_x;
extern QVector<double> vesselshape_y;

} // namespace globalvar

#endif // GLOBALVAR_H

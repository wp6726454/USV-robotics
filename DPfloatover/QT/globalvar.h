#ifndef GLOBALVAR_H
#define GLOBALVAR_H
#include <QString>
#include "../include/threaded_Loop.h"

namespace globalvar {
extern QString pwd;
extern threadloop _threadloop;  // thread-based concurrent socket
extern QVector<double> vesselshape_x;
extern QVector<double> vesselshape_y;

}  // namespace globalvar

#endif  // GLOBALVAR_H

#include "dataviewer.h"

void updatetest(dataviewer &t_dataviewer) {

  while (1) {
    if (t_dataviewer.angle < 360) {
      t_dataviewer.angle++;
      t_dataviewer.vessel1_rev.setConstant(t_dataviewer.angle);
      t_dataviewer.vessel2_rev.setConstant(t_dataviewer.angle);
      t_dataviewer.vessel3_rev.setConstant(t_dataviewer.angle);
    } else {
      t_dataviewer.vessel1_rev.setZero();
      t_dataviewer.vessel2_rev.setZero();
      t_dataviewer.vessel3_rev.setZero();
      t_dataviewer.angle = 0;
    }
    t_dataviewer.vessel1_rangle.setConstant(1);
    t_dataviewer.vessel2_rangle.setConstant(1);
    t_dataviewer.vessel3_rangle.setConstant(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

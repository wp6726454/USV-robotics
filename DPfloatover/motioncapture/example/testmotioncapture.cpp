#include "motioncapture.h"

int main() {
  realtimevessel_first _realtimevessel_first;
  realtimevessel_second _realtimevessel_second;
  realtimevessel_third _realtimevessel_third;
  motioncapture m_motioncapture;
  // call thread-based function for update motion data
  for (int i = 0; i != 20; ++i)
    m_motioncapture.RequestPositionandVelocity(
        _realtimevessel_first, _realtimevessel_second, _realtimevessel_third);
  test();
}

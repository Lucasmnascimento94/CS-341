#include "isp.h"

int main() {
  ispPowerUp();
  ispInit();
  START_ISP;
  ispProgrammingEnable();
  ispReadFuseBits();
  STOP_ISP;
  while (1)
    ;
}

#include "test.h"
#include "search.h"
#include "digital.h"
#include "dorobo32.h"

int main (){
  //Call dorobo_init() function to initialize HAL, Clocks, Timers etc.
  dorobo_init();

  // If DIP switch 1 is ON after reset, the robot is set to test mode.
  if (digital_get_dip(DD_DIP1) == DD_DIP_ON) {
    test();
  }
  else
  {
    search();
  }
  return 0;
}

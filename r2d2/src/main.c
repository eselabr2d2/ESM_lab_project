#include "test.h"
#include "search.h"
#include "digital.h"
#include "dorobo32.h"

int main (){
  //Call dorobo_init() function to initialize HAL, Clocks, Timers etc.
  dorobo_init();

  DD_DIP_STATE_T test_mode = digital_get_dip(DD_DIP1);
  if (test_mode == DD_DIP_ON) {
    test();
  }
  else
  {
    search();
  }
  return 0;
}

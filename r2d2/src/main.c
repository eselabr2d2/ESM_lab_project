#include "test.c"
#include "search.c"

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
}

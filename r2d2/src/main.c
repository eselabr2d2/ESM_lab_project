#include <stdlib.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "adc.h"
#include "fft.h"
#include "motor.h"
#include "driver.h"
#include <stdint.h>

static void blinky(void *pvParameters);
static void adc_tester(void *pvParameters);
static void ir_tester(void *pvParameters);

int main()
{
  dorobo_init();      //Call dorobo_init() function to initialize HAL, Clocks, Timers etc.
    
  xTaskCreate(blinky, "BLINKYTASK", 128, NULL, 2, NULL);  //create microswitches tester task
  xTaskCreate(adc_tester, "ADCTASK", 128, NULL, 1, NULL); //  creat adc tester task
  xTaskCreate(ir_tester, "IRTASK", 216, NULL, 2, NULL);

  vTaskStartScheduler();  //start the freertos scheduler

  return 0;       //should not be reached!
}

static void blinky(void *pvParameters) {

  // microswitch tester
  digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP);
  digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);

  DD_PINLEVEL_T sw01, sw02 ;

  enum DM_MOTORS_E motors[] =  {DM_MOTOR0};
   int8_t speeds[] = {95};
   int8_t steps[] = {5};
   int a_size = 1;
   TickType_t a_delay= 20;
   for(int i = 0; i < 2; i++)
     {
       motor_acc(motors, speeds, steps, a_size, a_delay);
       vTaskDelay(200);
       speeds[0] = -80;
       motor_acc(motors, speeds, steps, a_size, a_delay);
       vTaskDelay(200);
       speeds[0] = 0;
       motor_acc(motors, speeds, steps, a_size, a_delay);
       speeds[0] = 90;
     }

  uint32_t delay = 200;
  while (1) {
    traces("Task 1");
    sw01 = digital_get_pin(DD_PIN_PC13);
    sw02 = digital_get_pin(DD_PIN_PA8);

    if (sw01 == DD_LEVEL_HIGH )
      led_red_toggle();
    if (sw02 == DD_LEVEL_HIGH)
      led_green_toggle();

    vTaskDelay(delay);        //delay the task for 20 ticks (1 ticks = 50 ms)
  }
}

static void adc_tester(void *pvParameters) {

  // adc tester
  trace_init();
  adc_init();

  uint32_t left, right ;
  uint32_t delay = 200;

  while (1) {
    traces("Task 2");
    left  = adc_get_value(DA_ADC_CHANNEL0);
    right   = adc_get_value(DA_ADC_CHANNEL8);

    tracef("L = %i\n", left);
    vTaskDelay(delay);        //delay the task for 20 ticks (1 ticks = 50 ms)
  //  tracef("R = %i\n", right);
  //  vTaskDelay(delay);        //delay the task for 20 ticks (1 ticks = 50 ms)
  }
}

static void ir_tester(void *pvParameters){

  trace_init();
  uint32_t delay = 100;
  uint16_t fft01, fft02;
  digital_configure_pin( DD_PIN_PA15, DD_CFG_INPUT_NOPULL );

  // Not necessary
  //ft_init();
  while (1){
    ft_start_sampling(DD_PIN_PA15);
    while (!ft_is_sampling_finished) {}

    fft01 = ft_get_transform (DFT_FREQ125);
    fft02 = ft_get_transform (DFT_FREQ100);
    tracef("freq125  %i \t freq100 %i \n", fft01, fft02);
    vTaskDelay(delay);        //delay the task for 20 ticks (1 ticks = 50 ms)
  }

}



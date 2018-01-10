#include <stdlib.h>
#include <stdbool.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor.h"
#include "fft.h"
#include "adc.h"
#include "trace.h"
#include "driver.h"
#include "test.h"

void test() {

  xTaskCreate(test_blink, "BLINKTASK", 128, NULL, 1, NULL);
  xTaskCreate(test_motor, "MOTORTASK", 128, NULL, 1, NULL);
  xTaskCreate(test_hit, "HITTASK", 128, NULL, 1, NULL);
  xTaskCreate(test_distance, "TASK", 128, NULL, 1, NULL);
  xTaskCreate(test_ir, "IRTASK", 128, NULL, 1, NULL);

  vTaskStartScheduler();  //start the freeRTOS scheduler
  //should not be reached!
}

static void test_blink(void *pvParameters) {

  while(1) {
      led_red_toggle();
      led_green_toggle();
      vTaskDelay(100);
      led_red_toggle();
      led_green_toggle();
      vTaskDelay(100);
      led_green_toggle();
      vTaskDelay(100);
      led_green_toggle();
      vTaskDelay(100);
  }
}

static void test_motor(void *pvParameters) {

  enum DM_MOTORS_E motors[] =  {DM_MOTOR0, DM_MOTOR1};
  int array_size = 2;
  while(1)
  {
    int8_t speeds[] = {90, -90};
    accelerator(motors, speeds, array_size);
    vTaskDelay(200);
    speeds[0] = -90;
    speeds[1] = 90;
    accelerator(motors, speeds, array_size);

    vTaskDelay(200);
  }

}

static void test_hit(void *pvParameters) {

  //Micro switches
  digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP);
  digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);

  DD_PINLEVEL_T hit_left, hit_right;

  while (1) {
    hit_left  = digital_get_pin(DD_PIN_PC13);
    hit_right = digital_get_pin(DD_PIN_PA8);

    tracef("Hit left  = %i\t Hit right = %i\n", hit_left, hit_right);

    vTaskDelay(200);
   }

}

static void test_distance(void *pvParameters) {

  uint32_t dist_left, dist_right ;

  while (1) {
    dist_left  = adc_get_value(DA_ADC_CHANNEL0);
    dist_right = adc_get_value(DA_ADC_CHANNEL1);

    tracef("Distance left  = %i\t Distance right = %i\n", dist_left, dist_right);

    vTaskDelay(200);  //delay the task for n ticks (1 ticks = 50 ms)
  }
}

static void test_ir(void *pvParameters){

  uint16_t ir_ftt125_left, ir_fft125_right;

  digital_configure_pin(DD_PIN_PA15, DD_CFG_INPUT_NOPULL);
  digital_configure_pin(DD_PIN_PB3, DD_CFG_INPUT_NOPULL);

  while (1){
    ft_start_sampling(DD_PIN_PA15);
    while (!ft_is_sampling_finished);
    ir_ftt125_left = ft_get_transform (DFT_FREQ100);

    ft_start_sampling(DD_PIN_PB3);
    while (!ft_is_sampling_finished);
    ir_fft125_right = ft_get_transform (DFT_FREQ100);

    tracef("IR left  = %i\t IR right = %i\n", ir_ftt125_left, ir_fft125_right);

    vTaskDelay(200);
  }
}

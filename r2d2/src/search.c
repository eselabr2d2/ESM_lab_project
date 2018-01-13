#include <stdlib.h>
#include <stdint.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "adc.h"
#include "fft.h"
#include "motor.h"
#include "driver.h"
#include "search.h"

// TODO: Refine the threshold.
#define DIST_THR 1100

static void control_motors(void *pvParameters);
static void watch_hit(void *pvParameters);
static void watch_distance(void *pvParameters);
static void watch_tower(void *pvParameters);

enum SENSOR_STATUS
{
  SENSOR_NONE,
  SENSOR_LEFT,
  SENSOR_RIGHT,
  SENSOR_BOTH
};

volatile enum SENSOR_STATUS hit_status;
volatile enum SENSOR_STATUS dist_status;
volatile enum SENSOR_STATUS tower_status;
volatile uint8_t cancel_acc;

void search(){

  xTaskCreate(control_motors, "CONTROLMOTORS", 128, NULL, 1, NULL);
  xTaskCreate(watch_hit, "WATCHHIT", 128, NULL, 1, NULL);
  xTaskCreate(watch_distance, "WATCHDIST", 128, NULL, 1, NULL);
  xTaskCreate(watch_tower, "WATCHTOWER", 128, NULL, 1, NULL);

  vTaskStartScheduler();  //Start the freeRTOS scheduler.
  //should not be reached!
}

static void control_motors(void *pvParameters){

    // TODO: Increase velocity.
  int8_t drive_fwd[] = {60,-60,0};
  int8_t drive_fwd_left[] = {30,-60,-30};
  int8_t drive_fwd_right[] = {60,-30,30};

  int8_t drive_fwd_left_slow[] = {10,-30,-10};
  int8_t drive_fwd_right_slow[] = {30,-10,10};

  int8_t drive_bwd[] = {-60,60,0};
  //TODO: Check the lateral hit behavior.
  int8_t drive_bwd_left[] = {-30,60,30};
  int8_t drive_bwd_right[] = {-60,30,-30};

  while(1){
    // If DIP switch 4 is on after reset, the motors are stopped.
    if (digital_get_dip(DD_DIP4) == DD_DIP_ON) {
      int8_t drive_stop[] = {0, 0, 0};
      move(drive_stop, 0);
      while (digital_get_dip(DD_DIP4) == DD_DIP_ON) {
       vTaskDelay(200);
      }
    }

    // Control HIT behavior first.
    if (hit_status != SENSOR_NONE) {
      if(hit_status == SENSOR_LEFT)  {
        move(drive_bwd_left, 0);
      }
      else if (hit_status == SENSOR_RIGHT){
        move(drive_bwd_left, 0);
      }
      else if (hit_status == SENSOR_BOTH){
        move(drive_bwd, 0);
      }
      // Time to go back far enough to get rid of the obstacle.
      // TODO: Check this delay.
      vTaskDelay(100);
    }
    else if (dist_status == SENSOR_LEFT){ // Control safe distance.
      move(drive_fwd_right_slow, cancel_acc);
    }
    else if (dist_status == SENSOR_RIGHT){
      move(drive_fwd_left_slow, cancel_acc);
    }
    else if (tower_status == SENSOR_LEFT){ // Control TOWER target.
      move(drive_fwd_left, cancel_acc);
    }
    else if (tower_status == SENSOR_RIGHT){
      move(drive_fwd_right, cancel_acc);
    }
    else {
      move(drive_fwd, cancel_acc);
    }
  }
}

static void watch_hit(void *pvParameters) {

  digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP);
  digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);

  uint8_t hit_left, hit_right;

  //If hit only one side, still checking the other side another time.
  while (1) {
    for (uint8_t i=0; i<2 || (hit_left && hit_right); i++) {
      hit_left  |= !digital_get_pin(DD_PIN_PC13);
      hit_right |= !digital_get_pin(DD_PIN_PA8);
      // TODO: Check this delay .
      vTaskDelay(10);
    }
    if (hit_left && hit_right) {
      hit_status = SENSOR_BOTH;
    }
    else if (hit_left) {
      hit_status = SENSOR_LEFT;
    }
    else if (hit_right) {
      hit_status = SENSOR_RIGHT;
    }
    else {
      hit_status = SENSOR_NONE;
    }

    //cancel_acc = (hit_status == SENSOR_NONE) ? 0 : 1;
    cancel_acc = hit_status != SENSOR_NONE; 
    vTaskDelay(10);
    hit_left = 0;
    hit_right = 0;
  }
}

static void watch_distance(void *pvParameters) {

  uint32_t dist_left, dist_right;

  while (1) {
    // TODO: Check if there are values out of the average
    // If yes a "for loop" would solve the issue.
      dist_left  = adc_get_value(DA_ADC_CHANNEL0);
      dist_right = adc_get_value(DA_ADC_CHANNEL1);

    //For simplification: If both sensors surpass the threshold,
    //the left sensor has priority.
    if (dist_left > DIST_THR) {
      dist_status = SENSOR_LEFT;
    }
    else if (dist_right > DIST_THR) {
      dist_status = SENSOR_RIGHT;
    }
    else {
      dist_status = SENSOR_NONE;
    }

    vTaskDelay(10);
  }
}

static void watch_tower(void *pvParameters){

  // TODO: Change the names for the similar variables in test.c
  uint16_t ir_left, ir_right;

  digital_configure_pin(DD_PIN_PA15, DD_CFG_INPUT_NOPULL);
  digital_configure_pin(DD_PIN_PF10, DD_CFG_INPUT_NOPULL);

  while (1){
    ft_start_sampling(DD_PIN_PA15);
      while (!ft_is_sampling_finished());
      ir_left = ft_get_transform (DFT_FREQ100);

      ft_start_sampling(DD_PIN_PF10);
      while (!ft_is_sampling_finished());
      ir_right = ft_get_transform (DFT_FREQ100);

    if (ir_left==0 && ir_right==0) {
      tower_status = SENSOR_NONE;
    }
    // TODO: Check the percentage tolerance of -30%
    else if (ir_left > ir_right*70/100) {
      tracef("SENSOR_LEFT => IR left  = %i\t IR right = %i(%i)\n", ir_left, ir_right*70/100, ir_right);
      tower_status = SENSOR_LEFT;
    }
    // TODO: Check the percentage tolerance of -30%
    else if (ir_right > ir_left*70/100) {
      tracef("SENSOR_RIGHT => IR right  = %i\t IR left = %i\n", ir_right, ir_left*70/100, ir_left);
      tower_status = SENSOR_RIGHT;
    }
    else {
      tower_status = SENSOR_BOTH;
    }
    vTaskDelay(10);
  }
}

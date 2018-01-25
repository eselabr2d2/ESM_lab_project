/* ------------------------------------------- */
/*   INCLUDES AND DEFINES                      */
/* ------------------------------------------- */
#include "search.h"
#include "FreeRTOS.h"

// TODO: Refine the threshold.
#define DIST_THR_LEFT 1500//1100
#define DIST_THR_RIGHT 700//550

/* ------------------------------------------- */
/*   PRIVATE FUNCTIONS DECLARATIONS            */
/* ------------------------------------------- */
static void control_motors(void *pvParameters);
static void watch_hit(void *pvParameters);
static void watch_distance(void *pvParameters);
static void watch_tower(void *pvParameters);

/* ------------------------------------------- */
/*   GLOBAL VARIABLES                          */
/*   For data communication between threads    */
/* ------------------------------------------- */
enum SENSOR_STATUS
{
  SENSOR_NONE,
  SENSOR_LEFT,
  SENSOR_RIGHT,
  SENSOR_BOTH
};

volatile enum SENSOR_STATUS hit_status = SENSOR_NONE;
volatile enum SENSOR_STATUS dist_status;
volatile enum SENSOR_STATUS tower_status;

/* ------------------------------------------- */
/*   PUBLIC FUNCTIONS                          */
/* ------------------------------------------- */
void search(){

  xTaskCreate(control_motors, "CONTROLMOTORS", 128, NULL, 1, NULL);
  xTaskCreate(watch_hit, "WATCHHIT", 128, NULL, 1, NULL);
  xTaskCreate(watch_distance, "WATCHDIST", 128, NULL, 1, NULL);
  xTaskCreate(watch_tower, "WATCHTOWER", 128, NULL, 1, NULL);

  vTaskStartScheduler();  //Start the freeRTOS scheduler.
}

static void control_motors(void *pvParameters){
  
  int8_t drive_stop[] = {0, 0, 0};

    // TODO: Increase velocity.
  int8_t drive_fwd[] = {60,-60,0};
  int8_t drive_fwd_left[] = {50,-70,0};
  int8_t drive_fwd_right[] = {70,-50,0};

  int8_t drive_fwd_left_slow[] = {10,-50,-30};
  int8_t drive_fwd_right_slow[] = {50,-10,30};

  int8_t drive_bwd[] = {-70,70,0};
  int8_t drive_bwd_left[] = {-50,60,10};
  int8_t drive_bwd_right[] = {-60,50,-10};

  while(1){
    // If DIP switch 4 is on the motors are stopped.
    if (digital_get_dip(DD_DIP4) == DD_DIP_ON) {
      move(drive_stop);
      while (digital_get_dip(DD_DIP4) == DD_DIP_ON) {
       vTaskDelay(200);
      }
    }

    //The control flow below verify which movement the robot should perform.
    //Analyzing, in order of priority, the following status:
    //hit_status, dist_status, tower_status.
    switch (hit_status){
        case SENSOR_LEFT:
            move(drive_bwd_left);
            vTaskDelay(150);
            break;
        case SENSOR_RIGHT:
            move(drive_bwd_right);
            vTaskDelay(150);
            break;
        case SENSOR_BOTH:
            move(drive_bwd);
            // Time to go back far enough to get rid of the obstacle.
            // TODO: Check this delay. 
            vTaskDelay(100);
            break;
        default:
            if (dist_status == SENSOR_LEFT){ // Control safe distance.
              move(drive_fwd_right_slow);
            }
            else if (dist_status == SENSOR_RIGHT){
              move(drive_fwd_left_slow);
            }
            else if (tower_status == SENSOR_LEFT){ // Control TOWER target.
              move(drive_fwd_left);
            }
            else if (tower_status == SENSOR_RIGHT){
              move(drive_fwd_right);
            }
            else {
              move(drive_fwd);
            }
    }
  }
}

/* ------------------------------------------- */
/*   PRIVATE FUNCTIONS DEFINITIONS             */
/* ------------------------------------------- */

/**
    watch_hit: 
        read digital switches 
*/
static void watch_hit(void *pvParameters) {

  digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP);
  digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);

  uint8_t hit_left, hit_right;

  while (1) {
    hit_left = !digital_get_pin(DD_PIN_PC13);
    hit_right = !digital_get_pin(DD_PIN_PA8);

   vTaskDelay(10); 

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

  }
}

/**
    watch_distance: 
        read distance sensors and analyze
        if there is an obstacle and in 
        which direction
*/
static void watch_distance(void *pvParameters) {

  uint32_t dist_left, dist_right;

  while (1) {
      dist_left  = adc_get_value(DA_ADC_CHANNEL0);
      dist_right = adc_get_value(DA_ADC_CHANNEL1);

    //For simplification: If both sensors surpass the threshold,
    //the left sensor has priority.
    if (dist_left > DIST_THR_LEFT) {
      dist_status = SENSOR_LEFT;
    }
    else if (dist_right > DIST_THR_RIGHT) {
      dist_status = SENSOR_RIGHT;
    }
    else {
      dist_status = SENSOR_NONE;
    }

    vTaskDelay(10);
  }
}

/**
    watch_tower: 
        check if we detect the signal from
        the tower
*/
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

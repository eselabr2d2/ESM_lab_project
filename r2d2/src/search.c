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
volatile enum SENSOR_STATUS dist_status = SENSOR_NONE;
volatile enum SENSOR_STATUS tower_status = SENSOR_NONE;

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
  int8_t drive_fwd_left[] = {45,-65,-10};
  int8_t drive_fwd_right[] = {65,-45,10};

  int8_t drive_fwd_slow[] = {50,-50,0};
  int8_t drive_fwd_left_slow[] = {10,-40,-30};
  int8_t drive_fwd_right_slow[] = {40,-10,30};

  int8_t drive_bwd[] = {-50,50,0};
  int8_t drive_bwd_left[] = {-10,50,20};
  int8_t drive_bwd_right[] = {-50,10,-20};

  int8_t drive_rotate_right[] = {40,40,40};

  uint8_t stuck_counter = 0;


  while(1){
    // If DIP switch 4 is on the motors are stopped.
    if (digital_get_dip(DD_DIP4) == DD_DIP_ON) {
      move(drive_stop);
      while (digital_get_dip(DD_DIP4) == DD_DIP_ON) {
       vTaskDelay(200);
      }
    }

    //TODO: Check the stuck counter limit.
    if (stuck_counter <= 64)
    {
      //The control flow below verify which movement the robot should perform.
      //Analyzing, in order of priority, the following status:
      //hit_status, dist_status, tower_status.
      switch (hit_status){
        case SENSOR_LEFT:
          move(drive_bwd_left);
          stuck_counter++;
          vTaskDelay(120);
          break;
        case SENSOR_RIGHT:
          move(drive_bwd_right);
          stuck_counter++;
          vTaskDelay(120);
          break;
        case SENSOR_BOTH:
          move(drive_bwd);
          stuck_counter++;
          // Time to go back far enough to get rid of the obstacle.
          // TODO: Check this delay.
          vTaskDelay(120);
          break;
        default:
          if (dist_status == SENSOR_LEFT){ // Control safe distance.
            move(drive_fwd_right_slow);
            stuck_counter++;
          }
          else if (dist_status == SENSOR_RIGHT){
            move(drive_fwd_left_slow);
            stuck_counter++;
          }
          else if (tower_status == SENSOR_LEFT){ // Control TOWER target.
            move(drive_fwd_left);
            stuck_counter=0;
          }
          else if (tower_status == SENSOR_RIGHT){
            move(drive_fwd_right);
            stuck_counter=0;
          }
          else if (tower_status == SENSOR_BOTH){
            move(drive_fwd);
            stuck_counter=0;
          }
          else {
            move(drive_fwd_slow);
            stuck_counter=0;
          }
          vTaskDelay(10);
      }
    }
    else {
      move(drive_bwd);
      vTaskDelay(100);
      move(drive_stop);
      vTaskDelay(50);
      move(drive_rotate_right);
      // Delay to rotate enough to get out of the stuck situation in the
      //corners.
      vTaskDelay(200);
      stuck_counter=0;
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

     vTaskDelay(10);
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
      led_red(DD_LED_OFF);
      led_green(DD_LED_OFF);
    }
    // TODO: Check the percentage tolerance of -40%
    else if (ir_left*60/100 > ir_right) {
      tracef("SENSOR_LEFT => IR left  = %i\t IR right = %i(%i)\n", ir_left, ir_right*70/100, ir_right);
      tower_status = SENSOR_LEFT;
      led_red(DD_LED_ON);
    }
    // TODO: Check the percentage tolerance of -40%
    else if (ir_right*60/100 > ir_left) {
      tracef("SENSOR_RIGHT => IR right  = %i\t IR left = %i\n", ir_right, ir_left*70/100, ir_left);
      tower_status = SENSOR_RIGHT;
      led_green(DD_LED_ON);
    }
    else {
      tower_status = SENSOR_BOTH;
      led_red(DD_LED_ON);
      led_green(DD_LED_ON);
    }
    vTaskDelay(10);
  }
}

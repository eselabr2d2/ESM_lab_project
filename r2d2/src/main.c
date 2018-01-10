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

static void drive_robot(void *pvParameters);
static void get_distance(void *pvParameters);
static void tower_sensing(void *pvParameters);

volatile uint32_t leftEye;
volatile uint32_t rightEye;

volatile uint16_t tower01;
volatile uint16_t tower02;

int main(){
    //Call dorobo_init() function to initialize HAL, Clocks, Timers etc.
    dorobo_init();
    
    xTaskCreate(get_distance, "ADCTASK", 128, NULL, 1, NULL);
    xTaskCreate(tower_sensing, "IRTASK", 128, NULL, 1, NULL);
    xTaskCreate(drive_robot, "DRIVETASK", 128, NULL, 1, NULL);

    vTaskStartScheduler();  //start the freertos scheduler

    return 0;       //should not be reached!
}


static void go_to_tower(){
    // Turn off motors
    // TODO: turn_off();
    uint16_t threshold = 11314;
    // rotate motor until we got the strongest signal
    while( tower02 < threshold){
       // TODO: rotate();
    }
}

static void avoid_obstacle(){
    // Turn off motors
    // TODO: turn_off();

    // rotate motor until
    // the rightEye and leftEye > noObject
    uint32_t noSafetyDistance = 2131567;
    while( leftEye < noSafetyDistance){
        // TODO: rotate()
    }

}

static void drive_robot(void *pvParameters){
    /* some number , we need to calibrate in order to get
       the right number
       */

    uint32_t secure_distance = 15165156;
    uint32_t detected = 1121; // some numebre

    // L, R, B
    enum DM_MOTORS_E motors[] = { DM_MOTOR0, DM_MOTOR1, DM_MOTOR2 };
 
    int8_t go[] = {50,-50,0};
    int8_t scape[] = {20,20,20};
    while(1){
 /*
	    // TODO: go_straight() .. need this function
        if ( tower01 >= detected ) {
            // change direction such as
            // we got the strongest signal
            go_to_tower();
        }
        else {
            // if d01 < thrs means that
            // there is an obstacle in front
            if (leftEye < secure_distance){
                // so far avoid obstacle is
                // just changing direction
                avoid_obstacle();
            }
        }
	*/
	  if ( tower01 > 100 )
	      accelerator( motors, scape, 3);
	  else
	    accelerator( motors, go , 3);
    }
    
}

static void get_distance(void *pvParameters) {
  adc_init();

  while (1) {
    leftEye  = adc_get_value(DA_ADC_CHANNEL0);
    rightEye  = adc_get_value(DA_ADC_CHANNEL8);
  }
}


static void tower_sensing(void *pvParameters){
  digital_configure_pin( DD_PIN_PA15, DD_CFG_INPUT_NOPULL );

  // Not necessary
  //ft_init();
  while (1){
    ft_start_sampling(DD_PIN_PA15);
    while (!ft_is_sampling_finished) {}

    tower01 = ft_get_transform (DFT_FREQ125);
    tower02 = ft_get_transform (DFT_FREQ100);
  }

}

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

volatile uint32_t distance01;
volatile uint32_t distance02;

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
    // the distance02 and distance01 > noObject
    uint32_t noSafetyDistance = 2131567;
    while( distance01 < noSafetyDistance){
        // TODO: rotate()
    }

}

static void drive_robot(void *pvParameters){
    /* some number , we need to calibrate in order to get 
       the right number
       */

    uint32_t secure_distance = 15165156; 
    uint32_t detected = 1121; // some numebre 
    while(1){ 
        // TODO: go_straight() .. need this function
        if ( tower01 >= detected ) {
            // change direction such as 
            // we got the strongest signal
            go_to_tower();
        }
        else {
            // if d01 < thrs means that 
            // there is an obstacle in front
            if (distance01 < secure_distance){
                // so far avoid obstacle is
                // just changing direction
                avoid_obstacle();
            }
        }
    }
}

static void get_distance(void *pvParameters) {
  adc_init();

  while (1) {
    distance01  = adc_get_value(DA_ADC_CHANNEL0);
    distance02  = adc_get_value(DA_ADC_CHANNEL8);
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



#include <stdlib.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "adc.h"
#include "fft.h"
#include "motor.h"
#include "driver.h"
#include "search.h"
#include <stdint.h>

#define OBSTACLE_THR 1000
#define obstacleDetected(X) ((X >= OBSTACLE_THR)?1:0)

#define ROTATE_LEFT                     \
    while( obstacleDetected(leftEye))   \
        accelerator(motors, turnLeft, 3);

#define ROTATE_RIGHT                    \
    while( obstacleDetected(rightEye))  \
        accelerator(motors, turnRight, 3);
 
static void drive_robot(void *pvParameters);
static void get_distance(void *pvParameters);
static void tower_sensing(void *pvParameters);
static void sense_laterals(void *pvParameters);

volatile uint32_t leftEye;
volatile uint32_t rightEye;

volatile uint16_t tower01;
volatile uint16_t tower02;

volatile DD_PINLEVEL_T leftWall;
volatile DD_PINLEVEL_T rightWall;

void search(){
    
    xTaskCreate(get_distance, "ADCTASK", 32, NULL, 1, NULL);
    xTaskCreate(tower_sensing, "IRTASK", 128, NULL, 1, NULL);
    xTaskCreate(sense_laterals, "SWITCHESTASK", 32, NULL, 2, NULL);
    xTaskCreate(drive_robot, "DRIVETASK", 128, NULL, 1, NULL);

    vTaskStartScheduler();  //start the freeRTOS scheduler
    //should not be reached!
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
    uint32_t detected = 1121; // some numebre

    // L, R, B
    enum DM_MOTORS_E motors[] = { DM_MOTOR0, DM_MOTOR1, DM_MOTOR2 };

   // TODO: find the speed that allow a good rotation
   //       no obstacle detection for none of the sensors 
    int8_t go[] = {50,-50,0};
    int8_t turnRight[] = {-50,50,-50};
    int8_t turnLeft[] = {-50,50,50};
  
    uint8_t rotateLeft ;
    uint8_t rotateRight ; 

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
        accelerator(motors, go, 3);

        /** OBSTACLE AVOIDANCE **/
        rotateLeft = obstacleDetected(leftEye);
        rotateRight = obstacleDetected(rightEye);

        /* if both sensor are detecting something near*/
        if ( rotateRight && rotateLeft) {
            if( leftWall == 1) {
                ROTATE_LEFT; }
            else if( rightWall == 1){
                ROTATE_RIGHT; }
            else if( leftEye >=rightEye){
                ROTATE_LEFT;}
            else ROTATE_RIGHT;
        }
        /* if just one of the sensors detect something*/ 
        if (rotateRight)    ROTATE_RIGHT; 
        if (rotateLeft)     ROTATE_LEFT;
    }
    
}

static void get_distance(void *pvParameters) {
  adc_init();

  while (1) {
    leftEye  = adc_get_value(DA_ADC_CHANNEL0);
    vTaskDelay(20);
    rightEye  = adc_get_value(DA_ADC_CHANNEL1);
    vTaskDelay(20);
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

static void sense_laterals(void *pvParameters) {

  //Micro switches
  digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP);
  digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);

  while (1) {
    leftWall  = digital_get_pin(DD_PIN_PC13);
    rightWall = digital_get_pin(DD_PIN_PA8);

    vTaskDelay(20);
   }

}



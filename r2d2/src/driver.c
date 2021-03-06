/* ------------------------------------------- */
/*   INCLUDES AND DEFINES                      */
/* ------------------------------------------- */
#include "driver.h"

// TODO: Check the step and its delay.
#define STEP 5
#define STEP_DELAY 10

/* ------------------------------------------- */
/*   FUNCTION DEFINITIONS - driver.h           */
/*      full description of the function in    */
/*      driver.h                               */
/* ------------------------------------------- */

//Function to change the speed of the motors depending on the size of the array.
void accelerator(enum DM_MOTORS_E motor[], int8_t aim_speed[], int8_t size) 
{
 
  int8_t speed_diff[size];
  int8_t speed[size];
  uint8_t any_diff = 0;

  for(uint8_t i = 0; i < size; i++)
  {
    speed[i] = motor_get_speed(motor[i]);
    speed_diff[i] = aim_speed[i] - motor_get_speed(motor[i]);
    any_diff = any_diff || (speed_diff[i] != 0);
  }
  while(any_diff)
  {
    any_diff = 0;
    for(uint8_t i = 0; i < size; i++)
    {
      if (speed_diff[i] != 0)
      {
        if (abs(speed_diff[i]) <= STEP)
        {
          motor_set(motor[i], aim_speed[i]);
          speed_diff[i] = 0;
        }
        else if (speed_diff[i] > 0)
        {
          motor_set(motor[i], speed[i] + STEP);
          speed_diff[i] = speed_diff[i] - STEP;
        }
        else
        {
          motor_set(motor[i], speed[i] - STEP);
          speed_diff[i] = speed_diff[i] + STEP;
        }
        speed[i] = aim_speed[i] - speed_diff[i];
        any_diff = any_diff || (speed_diff[i] != 0);
      }
    }
    vTaskDelay(STEP_DELAY);
   }
}

// Function to change the speed of the three motors.
void move(int8_t aim_speed[3]){
  enum DM_MOTORS_E motors[] = { DM_MOTOR0, DM_MOTOR1, DM_MOTOR2 };
  accelerator(motors, aim_speed,  3);
}



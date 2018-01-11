#include <stdlib.h>
#include <stdbool.h>
#include "motor.h"
#include "FreeRTOS.h"
#include "task.h"

#define STEP 5
#define STEP_DELAY 20

void accelerator3(int8_t aim_speed[], _Bool *cancel)
{
  enum DM_MOTORS_E motors[] = { DM_MOTOR0, DM_MOTOR1, DM_MOTOR2 };
  accelerator(motors, aim_speed, 3, cancel);
}

void accelerator(enum DM_MOTORS_E motor[], int8_t aim_speed[], int8_t size, _Bool *cancel)
{
  int speed_diff[size];
  int speed[size];
  _Bool any_diff = false;

  for(int i = 0; i < size && !(*cancel); i++)
  {
    speed[i] = motor_get_speed(motor[i]);
    speed_diff[i] = aim_speed[i] - motor_get_speed(motor[i]);
    any_diff = any_diff || (speed_diff[i] != 0);
  }
  while(any_diff && !(*cancel))
  {
    any_diff = false;
    for(int i = 0; i < size; i++)
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

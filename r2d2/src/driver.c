#include <stdlib.h>
#include <stdbool.h>
#include "motor.h"
#include "FreeRTOS.h"
#include "task.h"

void motor_acc(enum DM_MOTORS_E motor[], int8_t aim_speed[], int8_t step[], int8_t size, TickType_t delay)
{
  int speed_diff[size];
  int speed[size];
  _Bool any_diff = false;

  for(int i = 0; i < size; i++)
  {
    speed[i] = motor_get_speed(motor[i]);
    speed_diff[i] = aim_speed[i] - motor_get_speed(motor[i]);
    any_diff = any_diff || (speed_diff[i] != 0);
  }
  while(any_diff)
  {
    any_diff = false;
    for(int i = 0; i < size; i++)
    {
      if (speed_diff[i] != 0)
      {
        if (abs(speed_diff[i]) <= step[i])
        {
          motor_set(motor[i], aim_speed[i]);
          speed_diff[i] = 0;
        }
        else if (speed_diff[i] > 0)
        {
          motor_set(motor[i], speed[i] + step[i]);
          speed_diff[i] = speed_diff[i] - step[i];
        }
        else
        {
          motor_set(motor[i], speed[i] - step[i]);
          speed_diff[i] = speed_diff[i] + step[i];
        }
        speed[i] = aim_speed[i] - speed_diff[i];
        any_diff = any_diff || (speed_diff[i] != 0);
      }
    }
    vTaskDelay(delay);
  }
}

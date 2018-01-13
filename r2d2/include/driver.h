/*
 * driver.h
 *
 *  Functions for driving the motor
 *      Authors: Group 10
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "motor.h"
#include "task.h"

void accelerator(enum DM_MOTORS_E motor[], int8_t aim_speed[], int8_t size);
void move(int8_t aim_speed[]);

#endif /* INCLUDE_DRIVER_H_ */

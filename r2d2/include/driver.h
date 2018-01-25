/**
 * @file
 * driver.h
 *
 * Functions of the motor driver.
 *
 * @author
 * Group 10 - Daniel Paredes, Pedro Oliveira.
 */
#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "motor.h"
#include "task.h"

/**
 * @brief   Changes the speed of the chosen motors.
 *
 * @param   motor
 *          Chosen motors.
 *
 * @param   aim_speed
 *          Intended speed of the motors.
 *
 * @param   size
 *          Size of the motor and aim_speed arrays (max. 3).
 */
void accelerator(enum DM_MOTORS_E motor[], int8_t aim_speed[], int8_t size);

/**
 * @brief   Moves the robot by changing the speed of the three motors.
 *
 * @param   aim_speed
 *          The speed of the motors 0, 1 and 2.
 */
void move(int8_t aim_speed[3]);

#endif /* _DRIVER_H_ */

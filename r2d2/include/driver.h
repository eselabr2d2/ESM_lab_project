/*
 * test.h
 *
 *  Functions for driving the motor
 *      Authors: Group 10
 */

#ifndef INCLUDE_DRIVER_H_
#define INCLUDE_DRIVER_H_

void move(int8_t aim_speed[], uint8_t cancel);

void accelerator(enum DM_MOTORS_E motor[], int8_t aim_speed[], int8_t size, uint8_t  cancel);

#endif /* INCLUDE_DRIVER_H_ */

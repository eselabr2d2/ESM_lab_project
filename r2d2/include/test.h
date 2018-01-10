/*
 * test.h
 *
 *  Functions for testing the sensors operation,
 *  leds, and DIP switch as tasks.
 *
 */

#ifndef INCLUDE_TEST_H_
#define INCLUDE_TEST_H_

void test();
static void test_blink(void *pvParameters);
static void test_motor(void *pvParameters);
static void test_hit(void *pvParameters);
static void test_distance(void *pvParameters);
static void test_ir(void *pvParameters);


#endif /* INCLUDE_TEST_H_ */

/**
 * @file
 * search.h
 *
 * Functions for searching the tower.
 *
 * @author
 * Group 10 - Daniel Paredes, Pedro Oliveira.
 */
#ifndef _SEARCH_H_
#define _SEARCH_H_

#include "dorobo32.h"
#include "trace.h"
#include "adc.h"
#include "fft.h"
#include "driver.h"

/**
 * @brief   Searches the tower.
 *
 * Searches the tower through a set of internal tasks that analyze the sensors
 * and switches.
 */
void search();

#endif /* _SEARCH_H_ */

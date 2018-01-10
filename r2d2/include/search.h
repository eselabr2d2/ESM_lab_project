/*
 * search.h
 *
 *  Created on: 10 de jan de 2018
 *      Author: Pedro
 */

#ifndef INCLUDE_SEARCH_H_
#define INCLUDE_SEARCH_H_

void search();
static void drive_robot(void *pvParameters);
static void get_distance(void *pvParameters);
static void tower_sensing(void *pvParameters);

#endif /* INCLUDE_SEARCH_H_ */

#include <stdlib.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include <stdint.h>

static void blinky(void *pvParameters);

int main()
{
    dorobo_init();			//Call dorobo_init() function to initialize HAL, Clocks, Timers etc.	
	
	xTaskCreate(blinky, "BLINKYTASK", 512, NULL, 2, NULL);	//create blinky task

	vTaskStartScheduler();	//start the freertos scheduler

	return 0;				//should not be reached!
}

static void blinky(void *pvParameters) 
{
	trace_init();
  uint32_t delay = 500;
	while (1) 
	{
		led_green_toggle();
		led_red_toggle();
		traces("toogle led");		//print debug message
		vTaskDelay(delay);				//delay the task for 20 ticks (1 ticks = 50 ms)
	}
}

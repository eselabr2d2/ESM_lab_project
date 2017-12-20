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

static void blinky(void *pvParameters) {

	// microswitch tester
	trace_init();
	digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP); 
	digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);	

	DD_PINLEVEL_T sw01, sw02 ;
	
	uint32_t delay = 100;
	while (1) 
	{
		sw01 = digital_get_pin(DD_PIN_PC13);
		sw02 = digital_get_pin(DD_PIN_PA8);
		
		if (sw01 == DD_LEVEL_HIGH ) 
			led_red_toggle();
		if (sw02 == DD_LEVEL_HIGH)
			led_green_toggle();

		vTaskDelay(delay);				//delay the task for 20 ticks (1 ticks = 50 ms)
	}
}

#include <stdlib.h>
#include "dorobo32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "adc.h"
#include <stdint.h>

static void blinky(void *pvParameters);
static void adc_tester(void *pvParameters);
static void ir_tester(void *pvParameters);


int main()
{
	dorobo_init();			//Call dorobo_init() function to initialize HAL, Clocks, Timers etc.	
    
	xTaskCreate(blinky, "BLINKYTASK", 216, NULL, 2, NULL);	//create microswitches tester task
	xTaskCreate(adc_tester, "ADCTASK", 216, NULL, 1, NULL); //  creat adc tester task
	//xTaskCreate(ir_tester, "IRTASK", 512, NULL, 2, NULL); 

	vTaskStartScheduler();	//start the freertos scheduler

	return 0;				//should not be reached!
}

static void blinky(void *pvParameters) {

	// microswitch tester
	digital_configure_pin( DD_PIN_PC13, DD_CFG_INPUT_PULLUP); 
	digital_configure_pin( DD_PIN_PA8, DD_CFG_INPUT_PULLUP);	

	DD_PINLEVEL_T sw01, sw02 ;
	
	uint32_t delay = 200;
	while (1) {
		traces("Task 1");
		sw01 = digital_get_pin(DD_PIN_PC13);
		sw02 = digital_get_pin(DD_PIN_PA8);
		
		if (sw01 == DD_LEVEL_HIGH ) 
			led_red_toggle();
		if (sw02 == DD_LEVEL_HIGH)
			led_green_toggle();

		vTaskDelay(delay);				//delay the task for 20 ticks (1 ticks = 50 ms)
	}
}

static void adc_tester(void *pvParameters) {

	// adc tester
	trace_init();
	adc_init();

	uint32_t left, right ;
	uint32_t delay = 200;

	while (1) {
		traces("Task 2");
		left 	= adc_get_value(DA_ADC_CHANNEL0);
		right 	= adc_get_value(DA_ADC_CHANNEL8);
		
		tracef("L = %i\n", left);
		vTaskDelay(delay);				//delay the task for 20 ticks (1 ticks = 50 ms)
	//	tracef("R = %i\n", right);
	//	vTaskDelay(delay);				//delay the task for 20 ticks (1 ticks = 50 ms)
	}
}

static void ir_tester(void *pvParameters){
	
	trace_init();
	uint32_t delay = 100;
//	ft_int();

//	ft_start_sampling( DD_PIN_PD14);
	
	digital_configure_pin( DD_PIN_PA15, DD_CFG_INPUT_PULLUP );
	digital_configure_pin( DD_PIN_PF10, DD_CFG_INPUT_PULLDOWN);

	DD_PINLEVEL_T ir01, ir02;

	while(1){
		ir01 = digital_get_pin(DD_PIN_PA15);
		ir02 = digital_get_pin(DD_PIN_PF10);
		tracef("ir01 = %i \t ir02 = %i \n", ir01, ir02);
		vTaskDelay(delay);
	}
}

#ifndef __LED_H
#define __LED_H

#include "typedefs.h"

//#define LED_ALL			0
//#define LED_INDEX1		1
//#define LED_INDEX2		2
//#define LED_INDEX3		3
typedef enum 
{
	valve_1=1,
	valve_2=2,
	valve_3=3,
	valve_4=4

}output_inx;
void outputs_init(void);
void output_on(output_inx index);
void output_off(output_inx index);
void test_outputs(void);

//void led_reverse(u8 index);
u8 ouput_is_on(output_inx index);

//void led_onAutoOff(u8 index, u32 ms);


#endif

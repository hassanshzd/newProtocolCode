#include "led.h"
#include "gpio_defs.h"
#include "common.h"
#include "time_server.h"
#include "stm32f10x_conf.h"
#include "cmt2300a_hal.h"

//Change leds to valves cntr gpios 
void outputs_init(void)
{
    SET_GPIO_L(PENO2_GPIO);
    SET_GPIO_L(PENO1_GPIO);
    SET_GPIO_L(EV4_GPIO);
		SET_GPIO_L(EV3_GPIO);
    SET_GPIO_OUT(PENO2_GPIO);
    SET_GPIO_OUT(PENO1_GPIO);
    SET_GPIO_OUT(EV4_GPIO);
		SET_GPIO_OUT(EV3_GPIO);
}

void output_on(output_inx index)
{
    switch(index)
    {
    case valve_1:   SET_GPIO_H(PENO1_GPIO);    break;
    case valve_2:   SET_GPIO_H(PENO2_GPIO);    break;
    case valve_3:   SET_GPIO_H(EV3_GPIO);    break;
	case valve_4:	SET_GPIO_H(EV4_GPIO);	break;
        
    }
}

void output_off(output_inx index)
{
    switch(index)
    {
    case valve_1:   SET_GPIO_L(PENO1_GPIO);    break;
    case valve_2:   SET_GPIO_L(PENO2_GPIO);    break;
    case valve_3:   SET_GPIO_L(EV3_GPIO);    break;
	case valve_4:	SET_GPIO_L(EV4_GPIO);	break;
        
    }
}
void test_outputs(void)
{
	u8 i=0;
	for(i=0;i<2;i++)
	{
		SET_GPIO_H(PENO1_GPIO);
		SET_GPIO_H(PENO2_GPIO);
		SET_GPIO_H(EV3_GPIO);
		SET_GPIO_H(EV4_GPIO);
		CMT2300A_DelayMs(1000);
		SET_GPIO_L(PENO1_GPIO);
		SET_GPIO_L(PENO2_GPIO);
		SET_GPIO_L(EV3_GPIO);
		SET_GPIO_L(EV4_GPIO);
		CMT2300A_DelayMs(1000);
	}
	
	
}
//void led_reverse(u8 index)
//{
//    if(index==LED_ALL || index==LED_INDEX1)
//    {
//        if(READ_GPIO_PIN(LED1_GPIO))
//            SET_GPIO_L(LED1_GPIO);
//        else
//            SET_GPIO_H(LED1_GPIO);
//    }

//    if(index==LED_ALL || index==LED_INDEX2)
//    {
//        if(READ_GPIO_PIN(LED2_GPIO))
//            SET_GPIO_L(LED2_GPIO);
//        else
//            SET_GPIO_H(LED2_GPIO);
//    }

//    if(index==LED_ALL || index==LED_INDEX3)
//    {
//        if(READ_GPIO_PIN(LED3_GPIO))
//            SET_GPIO_L(LED3_GPIO);
//        else
//            SET_GPIO_H(LED3_GPIO);
//    }
//}

u8 ouput_is_on(output_inx index)
{
    u8 bRet = 0;

    switch(index)
    {
    case valve_1:    bRet = READ_GPIO_PIN(PENO1_GPIO);    break;
    case valve_2:    bRet = READ_GPIO_PIN(PENO2_GPIO);    break;
    case valve_3:    bRet = READ_GPIO_PIN(EV3_GPIO);    break;
	case valve_4:    bRet = READ_GPIO_PIN(EV4_GPIO);    break;
    }

    return bRet>0 ?1 :0;
}

//void led1_onAutoOff_proc(void)
//{
//    led_off(LED_INDEX1);
//}

//void led2_onAutoOff_proc(void)
//{
//    led_off(LED_INDEX2);
//}

//void led3_onAutoOff_proc(void)
//{
//    led_off(LED_INDEX3);
//}

//void led_onAutoOff(u8 index, u32 ms)
//{
//    switch(index)
//    {
//    case LED_INDEX1:    time_server_setDelayRunTask(DELAY_RUN_TASK_LED1, led1_onAutoOff_proc, ms);    break;    
//    case LED_INDEX2:    time_server_setDelayRunTask(DELAY_RUN_TASK_LED2, led2_onAutoOff_proc, ms);    break;
//    case LED_INDEX3:    time_server_setDelayRunTask(DELAY_RUN_TASK_LED3, led3_onAutoOff_proc, ms);    break;
//    }

//    led_on(index);
//}

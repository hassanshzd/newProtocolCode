#include "typedefs.h"
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "system.h"
#include "time_server.h"
#include "interrupt_server.h"
//#include "key_server.h"

#include "gpio_defs.h"
#include "led.h"
//#include "lcd12864.h"
//#include "buzzer.h"

#include "radio.h"
#include "uart.h"

/*******************************************************
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 433.920 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = GFSK
; AGC                       = On
; Data Rate                 = 9.6 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +20 dBm
; Gaussian BT               = 0.5
; Bandwidth                 = Auto-Select kHz
; Data Mode                 = Packet
; Whitening                 = Disable
; Packet Type               = Fixed Length
; Payload Length            = 32
*********************************************************/

#define RF_RX_TIMEOUT    1000*60*60      //60min
#define RF_PACKET_SIZE   12               /* Define the payload size here */
#define TEST_DBG	false
	
static u8 g_rxBuffer[RF_PACKET_SIZE];   /* RF Rx buffer */
//static u8 g_txBuffer[RF_PACKET_SIZE];   /* RF Tx buffer */

char str[100];

uint32_t g_nRecvCount=0,g_nSendCount=0;

rfpacket *tpms_pckt=(rfpacket*)&g_rxBuffer[0];

float batt_v,prsure_kPa;

int8_t temp_centigrt;

u8 tpms_data_rdy=0;

#if (TEST_DBG)
RCC_ClocksTypeDef RCC_Clocks;
#endif
void Mcu_Init(void)
{
    /* system init */
    SystemInit();
		//SystemCoreClockUpdate();
    GPIO_Config();
    NVIC_Config();
    SystemTimerDelay_Config();
  	ADC1_Init();
    Timer5_Config();
	Timer7_Config();
	Timer3_Config();

//printf("SYSCLK Frequency: %lu Hz\n", RCC_Clocks.SYSCLK_Frequency);
//printf("HCLK Frequency: %lu Hz\n", RCC_Clocks.HCLK_Frequency);
//printf("PCLK1 Frequency: %lu Hz\n", RCC_Clocks.PCLK1_Frequency);
//printf("PCLK2 Frequency: %lu Hz\n", RCC_Clocks.PCLK2_Frequency);

//    lcd12864_init();
//    buzzer_init();
//    
//    lcd12864_led_on();
}

u8 Radio_Recv_FixedLen(u8 pBuf[],u8 len)
{
#ifdef ENABLE_ANTENNA_SWITCH
	      if(CMT2300A_ReadGpio3())  /* Read INT2, PKT_DONE */
#else
				if(CMT2300A_ReadGpio1()) /* Read INT1, SYNC OK */
				{
				  /******/
				}
        if(CMT2300A_ReadGpio2())  /* Read INT2, PKT_DONE */
#endif	
		   {
//        if(CMT2300A_MASK_PKT_OK_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_FLAG))  /* Read PKT_OK flag */
				 {
						CMT2300A_GoStby();
						CMT2300A_ReadFifo(pBuf,len);
//					 if(*pBuf==0x01)
//						 SET_GPIO_H(LED1_GPIO);
						CMT2300A_ClearRxFifo();
						CMT2300A_ClearInterruptFlags();
						CMT2300A_GoRx();
						
						return 1;
				 }
		   }

		return 0;
}

/* Main application entry point */
int main(void) //��������
{
	 // u8 i=0;
	
    Mcu_Init();
    RF_Init();
	USART3_Init();
	#if (TEST_DBG)
	USART3_SendString("hello test");
	//test_outputs();//test pe9-pe12
	RCC_GetClocksFreq(&RCC_Clocks);
	#endif
	  if(FALSE==CMT2300A_IsExist()) {
        views_print_line(0, "CMT2300A not found!");
        while(1);
    }
    else {
        views_print_line(0, "CMT2300A ready RX");
    }
	
	  CMT2300A_GoStby();
	
		/* Must clear FIFO after enable SPI to read or write the FIFO */
    CMT2300A_EnableReadFifo();
		CMT2300A_ClearInterruptFlags();
		CMT2300A_ClearRxFifo();
  	CMT2300A_GoRx();
	
	  while(1)
		{
//			if(!READ_GPIO_PIN(micro_sw1_GPIO) && !READ_GPIO_PIN(micro_sw2_GPIO))
//	{
//		output_on(valve_1);
//		champer_state=champer_close;
//		req_state=idl;
//	}
//	else
//	{
//				output_off(valve_1);
//		champer_state=champer_open;
//		req_state=idl;
//		//Start_Triggering=0;
//	}

if(!READ_GPIO_PIN(micro_sw2_GPIO))
{	
	system_delay_ms(200);

	if (!READ_GPIO_PIN(micro_sw1_GPIO))
	{
		output_on(valve_3);
		champer_state=champer_close;
		//req_state=idl;
	}
}
		   if(Radio_Recv_FixedLen(g_rxBuffer,RF_PACKET_SIZE))
			 {
				  g_nRfRxtimeoutCount=0;  //��� Time Out����
				  g_nRecvCount++;

				 if(tpms_pckt->start_frm==0xb0)
				 {
						u32 tpms_id=tpms_pckt->id;
						u32 tpms_id_chpk=0;
						tpms_id_chpk=(tpms_id&0xff)<<24|((tpms_id>>8 &0xff)<<16)|((tpms_id>>16 &0xff)<<8)|((tpms_id>>24 &0xff));
						prsure_kPa=(float)(tpms_pckt->prs_raw*3.13 +101.35);
						if(tpms_pckt->tmp_raw <= 0x7d)
						{
							temp_centigrt =tpms_pckt->tmp_raw;
							
						}
						else
						{
							temp_centigrt= tpms_pckt->tmp_raw - 256;
						}
						batt_v=(float)(tpms_pckt->bat_raw*0.01 +1.22);
						#if (TEST_DBG)
						//sprintf(str,"$Id= 0x%lX, BAT= %d, PRS= %d, TMP= %d $\r\n",tpms_id, (u16)(batt_v*100),(u16)(prsure_kPa*100),temp_centigrt*100);// cuz float val, *100
						sprintf(str,"$Id= 0x%lX, BAT= %d, PRS= %d, TMP= %d $\r\n",tpms_id_chpk, (u16)(batt_v*100),(u16)(prsure_kPa*100),temp_centigrt*100);
						USART3_SendString(str);
						#endif
						//data received from sensor
						tpms_data_rdy=1;
				 }
//			    for(i=0;i<RF_PACKET_SIZE;i++) //Clear Buff
//				   g_rxBuffer[i]=0;
				 
			 }

        
       if(g_nRfRxtimeoutCount>RF_RX_TIMEOUT) //����ʵ��Ӧ�ÿ��Ե���Time Out
			 {
				  g_nRfRxtimeoutCount=0;
					CMT2300A_GoSleep();
					CMT2300A_GoStby();
					CMT2300A_ClearInterruptFlags();
					CMT2300A_ClearRxFifo();
					CMT2300A_GoRx();
			 }	
			if(int_flg)
			{
				int_flg=0;
				handle_cmd();
				read_prs_sensor();
			}
		}		
}




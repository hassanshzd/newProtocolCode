/****** user functions*******/
/******2025				*****/

#include "user.h"
#include "gpio_defs.h"
#include "uart.h"
#include "led.h"
#include "cmt2300a_hal.h"

#define ADC_cof 3.3 / 4096
#define shunt_res_ohm 100
#define adc_resolution 4095
#define adc_ref_voltage 3.3
#define average_cnt 50
#define average_delay 2 // ms

float sensor1_voltage, sensor2_voltage;

// float presure_voltage, presure_current;

reply_pckt serial_rply_pkt;
rec_pkt *received_pkt = 0;

request_cmnd req_state = not_cnct;
Champer_state champer_state;

u8 int_flg;
u16 frame_cnt = 0;
u16 over_flow_timer = 0;
// Variables which are used for simulating LF driver data
//u8 Data[15] = {0xAA, 0xAA, 0xAA, 0xAA, 0xE2, 0xCC, 0xA6, 0x59, 0xA6, 0x59, 0xAA, 0x5A, 0x65, 0x65, 0x40};
//u8 Ref = 0x01;
//u8 BYTE_number = 0;
//u8 BIT_number = 0;
//u8 SHIFTER_BYTE = 0x40;
//u8 Time_Interval = 0;
//u8 Start_Triggering = 0;
//u8 timer_cnt = 0;
u8 out_state = 0;

//u16 Enable_Time_Interval = 0;

// ===============================================================
// LF Trigger function (multi-ID Manchester encoder, ASK via GPIO)
// Compatible with existing timer interrupt structure
// ===============================================================

<<<<<<< Updated upstream
uint8_t Data[12] = {
    0x00, 0x00, 0x00, 0x00,+     // 32-bit preamble (Manchester “0”)
    0xE5, 0x99,                 // 9-bit synchronization pattern (approximation of 3+1+2+2+1)
   // 0x0F, 0x0F,                 // Wake-Up ID
 	// 0x61, 0x5e,                 // Wake-Up ID
	0x7a, 0x86,                 // Wake-Up ID
    0x13, 0xC6, 0x6C
	, 0x39      // LF Data (MLF1)
};
=======
//uint8_t Data[12] = {
//      0x00, 0x00, 0x00, 0x00,    // 32-bit preamble (Manchester “0”)
//      0xE5, 0x99,                 // 9-bit synchronization pattern (approximation of 3+1+2+2+1)
//      0x0F, 0x0F,                 // Wake-Up ID  
//      0x13, 0xC6, 0x6C, 0x39      // LF Data (MLF1)
//};
>>>>>>> Stashed changes

//uint8_t Ref = 0x01;
//uint8_t BYTE_number = 0;
//uint8_t BIT_number = 0;
//uint8_t SHIFTER_BYTE = 0x00;
//uint8_t Time_Interval = 0;
uint8_t Start_Triggering = 0;
//uint8_t timer_cnt = 0;
//uint8_t out_state = 0;
//uint16_t Enable_Time_Interval = 0;

// ============================
// Manchester 256us per bit
// ============================

#define HALF_TICKS   32     // 128us = 32 * 4us
#define FULL_TICKS   64
#define SYNC_BITS    9

// ----------------------------
// ???? ??
// ----------------------------
uint8_t preamble[4] = {0x00,0x00,0x00,0x00};      // 32 ??? ???
uint8_t sync_b1     = 0xE5;                       // 8 ??? ??? ????
uint8_t sync_b2     = 0x80;                       // ??? ??? MSB ???? = ??? ???
uint8_t wakeup[2]   = {0x0F,0x0F};
uint8_t payload[4]  = {0x13,0xC6,0x6C,0x39};

// ----------------------------
// ???????? ????
// ----------------------------
uint8_t cur_byte      = 0;
uint8_t cur_bit       = 0;
uint8_t half_phase    = 0;
uint8_t section       = 0;   // 0=preamble,1=sync,2=wakeup,3=payload
uint16_t timer_cnt    = 0;

uint8_t current_data;        // ????? ?? ???? ????? ?????????


// ----------------------------
// ???? ?????????? ???? ?????
// ----------------------------
void start_LF(void)
{
    section = 0;
    cur_byte = 0;
    cur_bit  = 0;
    half_phase = 0;
    timer_cnt = 0;
    current_data = preamble[0];
}
/*************************************************************************/

void handle_cmd(void)
{

	serial_rply_pkt.header = rx_header;
	serial_rply_pkt.footer = rx_footer;

	switch (req_state)
	{
	case (idl):
	{

		serial_rply_pkt.t_state = champer_state;
		serial_rply_pkt.ch_state = idle_data;
		serial_rply_pkt.le_data = idle_data;

		u8 id_byte[] = {idle_data, idle_data, idle_data, idle_data};
		serial_rply_pkt.id = *(u32 *)&id_byte;

		serial_rply_pkt.prs_data = idle_data;
		serial_rply_pkt.temp_data = idle_data;
		serial_rply_pkt.ref_prs = idle_data;
		serial_rply_pkt.cnt = idle_data;
		frame_cnt = 0;
	}
	break;
	case (start_test):
	{
		// tester_init();
		if (!over_flow_timer)
			over_flow_timer = 1; // start of counter.....moddat zaman paieen Amadan mahfazeh
		// output_on(valve_3);//pressure
		//	Start_Triggering=1;
		//			if(champer_state==champer_open)
		//			{
		//				tester_init();
		//			//	req_state=start_test;
		//			}
		if ((champer_state == champer_close) || (champer_state == champer_close_and_test))
		{
			output_on(valve_4);
			//	req_state=start_test;
			start_LF();
			Start_Triggering = 1;
			if (tpms_data_rdy)
			{
				tpms_data_rdy = 0;
				champer_state = champer_close_and_test;
				// serial_rply_pkt.t_state=0x0f;
				serial_rply_pkt.id = tpms_pckt->ID;
				serial_rply_pkt.prs_data = tpms_pckt->prsur;
				serial_rply_pkt.temp_data = tpms_pckt->tempreture;
<<<<<<< Updated upstream
<<<<<<< HEAD
				serial_rply_pkt.tpms_battery = (tpms_pckt->status& 0x04)>>2;
=======
<<<<<<< HEAD
				//serial_rply_pkt.tpms_battery = (tpms_pckt->status& 0x04)>>2;
				serial_rply_pkt.tpms_battery = 178;
=======
				serial_rply_pkt.tpms_battery = 200;//(tpms_pckt->status& 0x04)>>2;
>>>>>>> 14b7119842a16b7a7e06e4cacd7ff02b2b84bbb2
>>>>>>> parent of 4513e14 (trigger function edited)
=======
				serial_rply_pkt.tpms_battery = (tpms_pckt->status& 0x04)>>2;
>>>>>>> Stashed changes
				serial_rply_pkt.cnt = ++frame_cnt;
			}
			else
			{
				// age dataye sensor amade nabud chiro neshun bede?
				champer_state = champer_close_and_test;
				serial_rply_pkt.id = 0;
				serial_rply_pkt.prs_data = 0;
				serial_rply_pkt.temp_data = 0;
				serial_rply_pkt.cnt = 0;
				serial_rply_pkt.tpms_battery = 0;
			}
			serial_rply_pkt.t_state = champer_state;
			serial_rply_pkt.ch_state = champer_state;
			serial_rply_pkt.le_data = (sensor1_voltage * 100);
			serial_rply_pkt.ref_prs = (sensor2_voltage * 100);
		}
	}
	break;
	case (stop_test):
		
	{
		tester_stop();
		req_state = idl;
	}
	break;
	case (not_cnct):
	{
		serial_rply_pkt.t_state = champer_state;
		serial_rply_pkt.ch_state = disconnect_data;
		serial_rply_pkt.le_data = disconnect_data;

		u8 id_byte[] = {disconnect_data, disconnect_data, disconnect_data, disconnect_data};
		serial_rply_pkt.id = *(u32 *)&id_byte;

		serial_rply_pkt.prs_data = disconnect_data;
		serial_rply_pkt.temp_data = disconnect_data;
		serial_rply_pkt.ref_prs = disconnect_data;
		serial_rply_pkt.cnt = disconnect_data;
	}
	break;
	case (idl_sync_test):
	{

		serial_rply_pkt.t_state = champer_state;
		serial_rply_pkt.ch_state = idle_data;
		serial_rply_pkt.le_data = idle_data;

		u8 id_byte[] = {idle_data, idle_data, idle_data, idle_data};
		serial_rply_pkt.id = *(u32 *)&id_byte;

		serial_rply_pkt.prs_data = idle_data;
		serial_rply_pkt.temp_data = idle_data;
		serial_rply_pkt.ref_prs = idle_data;
		serial_rply_pkt.cnt = idle_data;
		frame_cnt = 0;
		tester_init();
	}
	break;
	}
	/**********/
	USART3_SendArray(&serial_rply_pkt.header, sizeof(serial_rply_pkt));
	if (over_flow_timer)
		over_flow_timer++;
	if (over_flow_timer > OF_in_seconds)
	{
		over_flow_timer = 0;
		req_state = stop_test;
	}
}

void tester_init(void) // tester rutine
{
	// u8 input_state=0xff;
	// input_state=READ_GPIO_PIN(micro_sw1_GPIO);
	//	input_state&=READ_GPIO_PIN(micro_sw2_GPIO);
	if (!READ_GPIO_PIN(micro_sw1_GPIO))
	{
		system_delay_ms(50);
		if (!READ_GPIO_PIN(micro_sw2_GPIO))
		{
			output_on(valve_3);
			champer_state = champer_close;
			start_LF();
			Start_Triggering = 1;
			req_state = start_test;
		}
	}
}

void tester_stop(void)
{
	output_off(valve_3);
	output_off(valve_4);
	// output_on(valve_2);
	//	CMT2300A_DelayMs(champer_delay);
	// output_off(valve_2);
	champer_state = champer_open;
	//	output_off(valve_3);//pressure
	Start_Triggering = 0;    //1
}

/*void trigger_lf(void)
{
	timer_cnt++;
	if ((out_state == 0) && (Start_Triggering == 1))
	{
		SET_GPIO_H(LF_Clk_GPIO);
		out_state = 1;
	}
	else
	{
		SET_GPIO_L(LF_Clk_GPIO);
		out_state = 0;
	}
	if (timer_cnt >= 32)
	{
		timer_cnt = 0;
		if ((Enable_Time_Interval == 0) && (Start_Triggering == 1))
		{

			Ref = SHIFTER_BYTE & 0x80;
			SHIFTER_BYTE = SHIFTER_BYTE << 1;

			BIT_number++;

			if (BIT_number == 8)
			{
				//	SHIFTER_BYTE= Data;
				BIT_number = 0;
				BYTE_number++;
				SHIFTER_BYTE = Data[BYTE_number];
				if (BYTE_number == 15)
				{
					Enable_Time_Interval = 1;
					BYTE_number = 0;
				}
			}

			if (Ref == 0x80)

			{

				SET_GPIO_H(LF_Data_GPIO);
				//					SET_GPIO_H(LF_Clk_GPIO);
			}
			else if (Ref == 0x00)
			{

				SET_GPIO_L(LF_Data_GPIO);
				//					SET_GPIO_L(LF_Clk_GPIO);
			}
		}

		if ((Enable_Time_Interval == 1) && (Time_Interval < 39))
		{
			Time_Interval++;
			// HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
			// HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
		}
		else if (Time_Interval == 39)
		{
			Enable_Time_Interval = 0;
			Time_Interval = 0;
			SHIFTER_BYTE = Data[0];
		}
	}
}*/

u16 ADC1_ReadChannel(u8 channel)
{
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_71Cycles5);

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		;

	return ADC_GetConversionValue(ADC1);
}

u16 val1 = 0, val2 = 0;
void read_prs_sensor(void)
{
	u32 raw_data1 = 0, raw_data2 = 0;
	// float diff_v1=0,diff_v2=0;//,diff=0;

	// read adcchannel A1-8(PB0)
	for (u8 i = 0; i < average_cnt; i++)
	{
		raw_data1 += ADC1_ReadChannel(ADC_Channel_8);
		// system_delay_ms(average_delay);
		for (u16 j = 0; j < 1000; j++)
			;
	}
	raw_data1 /= average_cnt;

	// read adc channel A1-15(PC5)
	for (u8 i = 0; i < average_cnt; i++)
	{
		raw_data2 += ADC1_ReadChannel(ADC_Channel_15);
		// system_delay_ms(average_delay);
		for (u16 j = 0; j < 1000; j++)
			;
	}
	raw_data2 /= average_cnt;

	sensor1_voltage = (float)(raw_data1) / adc_resolution * adc_ref_voltage;
	sensor2_voltage = (float)(raw_data2) / adc_resolution * adc_ref_voltage;

	// diff_v1= ADC_cof * val1;
	// diff_v2= ADC_cof * val2;
	// presure_voltage=diff_v2-diff_v1;//volt
	// presure_current= (presure_voltage*1000)/shunt_res_ohm;//ma
}

u8 crc8_calc(u8* _data, uint8_t len) 
{
    uint8_t crc = 0xAA; 
    for (uint8_t i = 0; i < len; i++) {
        crc ^= _data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

//trig lf

<<<<<<< Updated upstream
void trigger_lf(void)
{
    timer_cnt++;
=======
//void trigger_lf(void)
//{
//    timer_cnt++;
//    if ((out_state == 0) && (Start_Triggering == 1))
//    {
//        SET_GPIO_H(LF_Clk_GPIO);
//        out_state = 1;
//    }
//    else
//    {
//        SET_GPIO_L(LF_Clk_GPIO);
//        out_state = 0;
//    }

//    if (timer_cnt >= 32)
//    {
//        timer_cnt = 0;

//        if ((Enable_Time_Interval == 0) && (Start_Triggering == 1))
//        {
//            Ref = SHIFTER_BYTE & 0x80;
//            SHIFTER_BYTE <<= 1;
//            BIT_number++;

//            if (BIT_number == 8)
//            {
//                BIT_number = 0;
//                BYTE_number++;
//                SHIFTER_BYTE = Data[BYTE_number];
//                if (BYTE_number >= sizeof(Data))
//                {
//                    Enable_Time_Interval = 1;
//                    BYTE_number = 0;
//                }
//            }

//            if (Ref == 0x80)
//                SET_GPIO_H(LF_Data_GPIO);
//            else
//                SET_GPIO_L(LF_Data_GPIO);
//        }

//        if ((Enable_Time_Interval == 1) && (Time_Interval < 39))
//        {
//            Time_Interval++;
//        }
//        else if (Time_Interval == 39)
//        {
//            Enable_Time_Interval = 0;
//            Time_Interval = 0;
//            SHIFTER_BYTE = Data[0];
//        }
//    }
//}



// ----------------------------
// ???? ????? ?????? – ?? 4us
// ----------------------------
void trigger_lf(void)
{
    // ??? ???? ????? ???? ???
    if(!Start_Triggering) return;

    // Toggle ???? ASK
>>>>>>> Stashed changes
    if ((out_state == 0) && (Start_Triggering == 1))
    {
        SET_GPIO_H(LF_Clk_GPIO);
        out_state = 1;
    }
    else
    {
        SET_GPIO_L(LF_Clk_GPIO);
        out_state = 0;
    }

<<<<<<< Updated upstream
    if (timer_cnt >= 32)
    {
        timer_cnt = 0;

        if ((Enable_Time_Interval == 0) && (Start_Triggering == 1))
        {
            Ref = SHIFTER_BYTE & 0x80;
            SHIFTER_BYTE <<= 1;
            BIT_number++;

            if (BIT_number == 8)
            {
                BIT_number = 0;
                BYTE_number++;
                SHIFTER_BYTE = Data[BYTE_number];
                if (BYTE_number >= sizeof(Data))
                {
                    Enable_Time_Interval = 1;
                    BYTE_number = 0;
                }
            }

            if (Ref == 0x80)
                SET_GPIO_H(LF_Data_GPIO);
            else
                SET_GPIO_L(LF_Data_GPIO);
        }

        if ((Enable_Time_Interval == 1) && (Time_Interval < 39))
        {
            Time_Interval++;
        }
        else if (Time_Interval == 39)
        {
            Enable_Time_Interval = 0;
            Time_Interval = 0;
            SHIFTER_BYTE = Data[0];
        }
    }
=======
    timer_cnt++;

    // ??? ??? 128us
    if(timer_cnt < HALF_TICKS)
        return;

    timer_cnt = 0;

    // ----------------------------
    // ??? ???/??? ??????
    // ----------------------------
    uint8_t bit_val = (current_data & 0x80) ? 1 : 0;

    if(half_phase == 0)
    {
        // ??? ??? ??? = ????? ???
        if(bit_val)
            SET_GPIO_H(LF_Data_GPIO);
        else
            SET_GPIO_L(LF_Data_GPIO);

        half_phase = 1;
        return;
    }
    else
    {
        // ??? ??? ??? = ?????
        if(bit_val)
            SET_GPIO_L(LF_Data_GPIO);
        else
            SET_GPIO_H(LF_Data_GPIO);

        half_phase = 0;  // ????? ??? ????
    }

    // ----------------------------
    // ???? ?? ??? ????
    // ----------------------------
    current_data <<= 1;
    cur_bit++;

    // ----------------------------
    // ????? ? ????
    // ----------------------------
    if(section == 1)   // ??? ????
    {
        if(cur_byte == 0)
        {
            // ???? ??? ???? = 8 ??? ????
            if(cur_bit >= 8)
            {
                cur_bit = 0;
                cur_byte = 1;
                current_data = sync_b2;     // ??? ??? MSB ??? ???
            }
        }
        else
        {
            // ???? ??? ???? = ??? ? ???!
            if(cur_bit >= 1)
            {
                // ???? ???? ??
                section = 2;   // ??? wakeup
                cur_byte = 0;
                cur_bit = 0;
                current_data = wakeup[0];
            }
        }
        return;
    }

    // ----------------------------
    // ???? ?????? (8 ??? ????)
    // ----------------------------
    if(cur_bit >= 8)
    {
        cur_bit = 0;
        cur_byte++;

        if(section == 0)  // ???????
        {
            if(cur_byte >= 4)
            {
                section = 1;  // ????
                cur_byte = 0;
                current_data = sync_b1;
                return;
            }
            current_data = preamble[cur_byte];
        }

        else if(section == 2)  // Wake-up
        {
            if(cur_byte >= 2)
            {
                section = 3;  // Payload
                cur_byte = 0;
                current_data = payload[0];
                return;
            }
            current_data = wakeup[cur_byte];
        }

        else if(section == 3) // Payload
        {
            if(cur_byte >= 4)
            {
                // ???? ??
                Start_Triggering = 0;
                return;
            }
            current_data = payload[cur_byte];
        }
    }
>>>>>>> Stashed changes
}


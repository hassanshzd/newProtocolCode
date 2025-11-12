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
//u8 out_state = 0;

//u16 Enable_Time_Interval = 0;

// ===============================================================
// LF Trigger function (multi-ID Manchester encoder, ASK via GPIO)
// Compatible with existing timer interrupt structure
// ===============================================================

uint8_t Data[8] = {
//    0x00, 0x00, 0x00, 0x00,     // 32-bit preamble (Manchester “0”)
    0xE5, 0x99,                 // 9-bit synchronization pattern (approximation of 3+1+2+2+1)
    0x0F, 0x0F,                 // Wake-Up ID
    0x13, 0xC6, 0x6C, 0x39      // LF Data (MLF1)
};

uint8_t Ref = 0x01;
uint8_t BYTE_number = 0;
uint8_t BIT_number = 0;
uint8_t SHIFTER_BYTE = 0x00;
uint8_t Time_Interval = 0;
uint8_t Start_Triggering = 0;
uint8_t timer_cnt = 0;
uint8_t out_state = 0;
uint16_t Enable_Time_Interval = 0;
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
			Start_Triggering = 1;
			if (tpms_data_rdy)
			{
				tpms_data_rdy = 0;
				champer_state = champer_close_and_test;
				// serial_rply_pkt.t_state=0x0f;
				serial_rply_pkt.id = tpms_pckt->ID;
				serial_rply_pkt.prs_data = tpms_pckt->prsur;
				serial_rply_pkt.temp_data = tpms_pckt->tempreture;
				serial_rply_pkt.tpms_battery = 200;//(tpms_pckt->status& 0x04)>>2;
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

void trigger_lf(void)
{
    static uint8_t clk_div = 0;       // Duty 40%
    static uint16_t tick = 0;         // 
    static uint8_t halfbit = 0;       // 
    static uint8_t bitval = 0;        // 
    static uint8_t byte_idx = 0;
    static uint8_t bit_idx = 0;
    static uint8_t SHIFTER_BYTE = 0;
    static uint8_t preamble_bits = 32; // 

    // ==================================================
    //  125kHz  Duty  40%
    // ==================================================
    if (Start_Triggering)
    {
        clk_div++;
        // 2 High  3 Low  40%
        if (clk_div == 1)
            SET_GPIO_H(LF_Clk_GPIO);
        else if (clk_div == 3)
            SET_GPIO_L(LF_Clk_GPIO);
        else if (clk_div >= 5)
            clk_div = 0;
    }
    else
    {
        SET_GPIO_L(LF_Clk_GPIO);
        SET_GPIO_L(LF_Data_GPIO);
        return;
    }

    // ==================================================
    //  Manchester timing:   ˜128µs  32 int
    // ==================================================
    tick++;
    if (tick < 32) return;
    tick = 0;

    // ==================================================
    //  preamble
    // ==================================================
    if (preamble_bits > 0)
    {
        // Manchester ‘0’ = High ? Low
        if (halfbit == 0)
            SET_GPIO_H(LF_Data_GPIO);
        else
            SET_GPIO_L(LF_Data_GPIO);

        halfbit ^= 1;
        if (halfbit == 0)
            preamble_bits--;  // 
        return;
    }

    // ==================================================
    //  (Sync + WakeUp + Payload)
    // ==================================================
    if (halfbit == 0)
    {
        //  Data
        SHIFTER_BYTE = Data[byte_idx];
        bitval = (SHIFTER_BYTE >> (7 - bit_idx)) & 1;

        // (Manchester)
        if (bitval == 0)
            SET_GPIO_H(LF_Data_GPIO);   // '0'  High/Low
        else
            SET_GPIO_L(LF_Data_GPIO);   // '1'  Low/High
    }
    else
    {
        // 
        if (bitval == 0)
            SET_GPIO_L(LF_Data_GPIO);
        else
            SET_GPIO_H(LF_Data_GPIO);

        // 
        bit_idx++;
        if (bit_idx >= 8)
        {
            bit_idx = 0;
            byte_idx++;
            if (byte_idx >= sizeof(Data))
            {
                byte_idx = 0;
                preamble_bits = 32;  // 
            }
        }
    }

    halfbit ^= 1;
}



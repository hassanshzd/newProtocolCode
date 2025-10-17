#ifndef __USER_H
#define __USER_H

#include "typedefs.h"

#define rx_header	0xAA
#define rx_footer	0x55
#define rec_pkt_len	5
#define disconnect_data 0xf3
#define idle_data	0xfc
#define champer_delay	3*1000 //ms
#define OF_timer	10//s (timer int every 200 ms)
#define OF_in_seconds	OF_timer*(1000/200)

#pragma pack(push)
#pragma pack(1)
typedef enum{
	no_data =0,
	cmplted	=1,
	receiving	=2
}rx_enum;
typedef enum 
{
	idl=0,
	start_test=1,
	stop_test=2,
	not_cnct=0xf3,
	idl_sync_test=0xfc
}request_cmnd;
typedef enum{
	champer_open=0,
	champer_close=0x01,
	champer_close_and_test=0x11
}Champer_state;

//typedef enum{
//	idle=0,
//	champer_close=0x01,
//	champer_close_and_test=0x11,
//	not_connected=0xf3,
//	idle_synch=0xfc
//}tester_state;
typedef struct
{
	u8 header;
	u8 t_state;
	u8 ch_state;
	u8 le_data;
	u32 id;
	u8 prs_data;
	u8 temp_data;
	u8 ref_prs;
	u8 tpms_battery;
	u8 cnt;
	u8 footer;
	
}reply_pckt;
typedef struct 
{
	u8 header;
	u8 req;
	u8 sync1;
	u8 sync2;
	u8 footer;
	
}rec_pkt;

typedef struct 
{
	u8 start_frm;
	u8 bat_raw;
	u32 id;
	u8 prs_raw;
	u8 tmp_raw;
	u8 frm_cnt;
	u8 wam_raw;
	u16 crc_16;
	
}rfpacket;
#pragma pack(pop)
extern Champer_state champer_state;
extern rfpacket *tpms_pckt;
extern rx_enum rx_flg;
extern request_cmnd req_state;
extern rec_pkt *received_pkt;
extern u8 int_flg;
extern u8 tpms_data_rdy;

void handle_cmd(void);
void tester_init(void);
void trigger_lf(void);
void tester_stop(void);
u16 ADC1_ReadChannel(u8 channel);
void read_prs_sensor(void);

#endif

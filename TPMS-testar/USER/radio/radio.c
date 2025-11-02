/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */
 
#include "radio.h"
#include "cmt2300a_params.h"

#include <string.h>
u8     CMT2219B_setting[200] =
        {
            0x00, 0x02, 0x01, 0x66, 0x02, 0xEC, 0x03, 0x1C, 0x04, 0x70, 0x05,
            0x80, 0x06, 0x14, 0x07, 0x08, 0x08, 0x91, 0x09, 0x02, 0x0A, 0x02,
            0x0B, 0xD0,/*cmt bank end*/ 0x0C, 0xAE, 0x0D, 0xFA, 0x0E, 0x50, 0x0F, 0x00, 0x10,
            0x00, 0x11, 0xF4, 0x12, 0x10, 0x13, 0xE2, 0x14, 0x42, 0x15, 0x2B,
            0x16, 0x00, 0x17, 0x01,/**/ 0x18, 0x42, 0x19, 0x71, 0x1A, 0xCE, 0x1B,
            0x1C, 0x1C, 0x42, 0x1D, 0x5B,/**/ 0x1E, 0x1C, 0x1F, 0x1C, 0x20, 0x95,
            0x21, 0xC1, 0x22, 0x20, 0x23, 0x20, 0x24, 0xf1, 0x25, 0x36, 0x26,
            0x2c, 0x27, 0x0A, 0x28, 0x9F, 0x29, 0x38, 0x2A, 0x29, 0x2B, 0x29,
            0x2C, 0xC0, 0x2D, 0x4A, 0x2E, 0x05, 0x2F, 0x53, 0x30, 0x10, 0x31,
            0x00, 0x32, 0xB4, 0x33, 0x00, 0x34, 0x00, 0x35, 0x01, 0x36, 0x00,
            0x37, 0x00, 0x38, 0x12, /*0: direct mode.  2: packet mode*/
            0x39, 0x08, 0x3A, 0x00, 0x3B, 0x01, 0x3C, 0x03, 0x3D, 0x00, 0x3E,
            0x00, 0x3F, 0x00, 0x40, 0x00, 0x41, 0x00, 0x42, 0x00, 0x43, 0x01,
            0x44, 0x00, 0x45, 0x00, 0x46, 0x0B, 0x47, 0x00, 0x48, 0x00, 0x49,
            0x00, 0x4A, 0x00, 0x4B, 0x00, 0x4C, 0x00, 0x4D, 0x00, 0x4E, 0x00,
            0x4F, 0x63, 0x50, 0xFF, 0x51, 0x01, 0x52, 0x00, 0x53, 0x1F, 0x54,
            0x10, /*FIFO input threshold (unit: byte). Once unread data exceeds
                     the threshold*/
            0x55, 0x50, 0x56, 0x34, 0x57, 0x19, 0x58, 0x00, 0x59, 0x07, 0x5A,
            0x70, 0x5B, 0x00, 0x5C, 0x01, 0x5D, 0x01, 0x5E, 0x3F, 0x5F, 0x7F,
            0x89, 0x02, 0x09, 0x02, 0x65, 0x15, /* GPIO status setting */
};

void send_spi(u8 adrr,u8 data_val)
{
	CMT2300A_WriteReg(adrr, data_val);
}
void RF_Init(void)
{
    u8 tmp;
//    u8 i=0;
		CMT2300A_InitGpio();
		CMT2300A_Init();
    
    /* Config registers */
    CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR       , g_cmt2300aCmtBank       , CMT2300A_CMT_BANK_SIZE       );
    CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR    , g_cmt2300aSystemBank    , CMT2300A_SYSTEM_BANK_SIZE    );
    CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , g_cmt2300aFrequencyBank , CMT2300A_FREQUENCY_BANK_SIZE );
    CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , g_cmt2300aDataRateBank  , CMT2300A_DATA_RATE_BANK_SIZE );
    CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR  , g_cmt2300aBasebandBank  , CMT2300A_BASEBAND_BANK_SIZE  );
    CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , g_cmt2300aTxBank        , CMT2300A_TX_BANK_SIZE        );
//		for(u8 i = 0; i < 14; i += 2)
//    send_spi(stng[i], stng[i + 1]);
    
    // xosc_aac_code[2:0] = 2
    tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
    CMT2300A_WriteReg(CMT2300A_CUS_CMT10, tmp|0x02);
    
	  RF_Config();
	//int i;

//  CMT2300A_DelayMs(200);
//  CMT2300A_DelayMs(20);
//  for( i = 0; i <= 6; i++)
//		CMT2300A_DelayMs(1);
//  send_spi(0x7F, 0xFF);//software reset

//  CMT2300A_DelayMs(1);
//  send_spi(0x60, 0x02);//go stnby

//  CMT2300A_DelayMs(1);
//  send_spi(0xE1, 0xFF);

//  CMT2300A_DelayMs(1);

//  send_spi(0xE1, 0xFF);
//  send_spi(0xE1, 0xFF);
//  send_spi(0x61, 0x52);
//  send_spi(0xE2, 0xFF);
//  send_spi(0x62, 0x20);
//  send_spi(0x8D, 0xFF);
//  send_spi(0x0D, 0x00);
//  send_spi(0xE6, 0xFF);
//  send_spi(0xED, 0xFF);
//  send_spi(0xEA, 0xFF);
//  send_spi(0x6A, 0x00);
//  send_spi(0x6B, 0x00);

//  for(i = 0; i < 198-8; i += 2)
//    send_spi(CMT2219B_setting[i], CMT2219B_setting[i + 1]);

////  send_spi(0xE6, 0xFF);
////  send_spi(0x66, 0x03);
////  send_spi(0xE7, 0xFF);
////  send_spi(0x67, 0x07);
////  send_spi(0x68, 0x11);
////  send_spi(0x8D, 0xFF);
////  send_spi(0x0D, 0x1A);
////  send_spi(0x60, 0x10);
////  CMT2300A_DelayMs(1);
//	RF_Config();
//  for(i = 0; i < 6; i++)
//  {
//    send_spi(0xE1, 0xFF);
//    CMT2300A_DelayMs(1);
//  }

//  send_spi(0xE1, 0xFF);
//  send_spi(0xC8, 0xFF);
//  send_spi(0x48, 0xAA);
//  send_spi(0xC8, 0xFF);
//  send_spi(0x48, 0x00);
//  send_spi(0x60, 0x02);
//  CMT2300A_DelayMs(1);

//  send_spi(0xE1, 0xFF);
//  send_spi(0xE9, 0xFF);
//  send_spi(0x69, 0x00);
//  send_spi(0xE6, 0xFF);
//  send_spi(0xED, 0xFF);
//  send_spi(0xEA, 0xFF);
//  send_spi(0x6A, 0x00);
//  send_spi(0x6B, 0x00);
//  send_spi(0xEE, 0xFF);
//  send_spi(0x6C, 0x02);
//  send_spi(0x60, 0x10);
//  CMT2300A_DelayMs(1);

//  for(i = 0; i <= 7; i++)
//  {
//    send_spi(0xE1, 0xFF);
//    CMT2300A_DelayMs(1);
//  }
}

void RF_Config(void)
{
#ifdef ENABLE_ANTENNA_SWITCH
	  u8 nInt2Sel;
    /* If you enable antenna switch, GPIO1/GPIO2 will output RX_ACTIVE/TX_ACTIVE,
       and it can't output INT1/INT2 via GPIO1/GPIO2 */
    CMT2300A_EnableAntennaSwitch(0);
    
    /* Config GPIOs */
    CMT2300A_ConfigGpio(CMT2300A_GPIO3_SEL_INT2);   /* INT2 > GPIO3 */    
    /* Config interrupt */
    nInt2Sel=	CMT2300A_INT_SEL_PKT_DONE; /* Config INT2 */
    nInt2Sel &= CMT2300A_MASK_INT2_SEL;
    nInt2Sel |= (~CMT2300A_MASK_INT2_SEL) & CMT2300A_ReadReg(CMT2300A_CUS_INT2_CTL);
    CMT2300A_WriteReg(CMT2300A_CUS_INT2_CTL, nInt2Sel); 
#else
	  CMT2300A_ConfigGpio( CMT2300A_GPIO1_SEL_INT1 |   /* INT1 > GPIO1 */
	                       CMT2300A_GPIO2_SEL_INT2 |   /* INT2 > GPIO2 */
                         CMT2300A_GPIO3_SEL_DOUT 	                      
	                      );
   
	  CMT2300A_ConfigInterrupt( CMT2300A_INT_SEL_SYNC_OK,  /* GPIO1 > SYNC_OK */
	                            CMT2300A_INT_SEL_PKT_DONE  /* GPIO2 > PKT_DONE*/
	                          );
      
#endif

    /* Enable interrupt */
      CMT2300A_EnableInterrupt(
				CMT2300A_MASK_PKT_DONE_EN | 
        CMT2300A_MASK_PREAM_OK_EN |
        CMT2300A_MASK_SYNC_OK_EN  
//        CMT2300A_MASK_NODE_OK_EN  |
//        CMT2300A_MASK_CRC_OK_EN   |
//        CMT2300A_MASK_TX_DONE_EN 
        );

    
    /* Disable low frequency OSC calibration */
    CMT2300A_EnableLfosc(FALSE);
    
    /* Use a single 64-byte FIFO for either Tx or Rx */
    //CMT2300A_EnableFifoMerge(TRUE);
    
    //CMT2300A_SetFifoThreshold(16); // FIFO_TH
    
    /* This is optional, only needed when using Rx fast frequency hopping */
    /* See AN142 and AN197 for details */
    //CMT2300A_SetAfcOvfTh(0x27);
    
    /* Go to sleep for configuration to take effect */
		
		
		
    CMT2300A_GoSleep();
}



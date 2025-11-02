#ifndef __CMT2300A_PARAMS_H
#define __CMT2300A_PARAMS_H
/*******************************************************
;---------------------------------------
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
; CDR Type                  = Counting
; CDR DR Range              = NA
; AFC                       = On
; AFC Method                = Auto-Select
; Data Representation       = 0:F-low 1:F-high
; Rx Duty-Cycle             = Off
; Tx Duty-Cycle             = Off
; Sleep Timer               = Off
; Sleep Time                = NA
; Rx Timer                  = Off
; Rx Time T1                = NA
; Rx Time T2                = NA
; Rx Exit State             = STBY
; Tx Exit State             = STBY
; SLP Mode                  = Disable
; RSSI Valid Source         = PJD
; PJD Window                = 8 Jumps
; LFOSC Calibration         = On
; Xtal Stable Time          = 155 us
; RSSI Compare TH           = NA
; Data Mode                 = Packet
; Whitening                 = Disable
; Whiten Type               = NA
; Whiten Seed Type          = NA
; Whiten Seed               = NA
; Manchester                = Disable
; Manchester Type           = NA
; FEC                       = Disable
; FEC Type                  = NA
; Tx Prefix Type            = 0
; Tx Packet Number          = 1
; Tx Packet Gap             = 32
; Packet Type               = Fixed Length
; Node-Length Position      = NA
; Payload Bit Order         = Start from msb
; Preamble Rx Size          = 2
; Preamble Tx Size          = 8
; Preamble Value            = 170
; Preamble Unit             = 8-bit
; Sync Size                 = 3-byte
; Sync Value                = 11152852
; Sync Tolerance            = None
; Sync Manchester           = Disable
; Node ID Size              = NA
; Node ID Value             = NA
; Node ID Mode              = None
; Node ID Err Mask          = Disable
; Node ID Free              = Disable
; Payload Length            = 32
; CRC Options               = None
; CRC Seed                  = NA
; CRC Range                 = NA
; CRC Swap                  = NA
; CRC Bit Invert            = NA
; CRC Bit Order             = NA
; Dout Mute                 = Off
; Dout Adjust Mode          = Disable
; Dout Adjust Percentage    = NA
; Collision Detect          = Off
; Collision Detect Offset   = NA
; RSSI Detect Mode          = Always
; RSSI Filter Setting       = No Filtering
; RF Performance            = High
; LBD Threshold             = 2.4 V
; RSSI Offset               = 26
; RSSI Offset Sign          = 1

;---------------------------------------
********************************************************/
/* [CMT Bank] */
const u8 g_cmt2300aCmtBank[CMT2300A_CMT_BANK_SIZE] = {
    0x02,
    0x66,
    0xEC,
    0x1c,
    0x70,
    0x80,
    0x14,
    0x08,
    0x91,
    0x02,
    0x02,
    0xD0,
};

/* [System Bank] */
const u8 g_cmt2300aSystemBank[CMT2300A_SYSTEM_BANK_SIZE] = {
    0xAE,
    0xFA,
    0x50,
    0x00,
    0x00,
    0xF4,
    0x10,
    0xE2,
    0x42,
    0x2B,
    0x00,
    0x01,
};

/* [Frequency Bank] */
const u8 g_cmt2300aFrequencyBank[CMT2300A_FREQUENCY_BANK_SIZE] = {
    0x42,
    0x71,
    0xCE,
    0x1C,
    0x42,
    0x5B,
    0x1C,
    0x1C,
};

/* [Data Rate Bank] */
const u8 g_cmt2300aDataRateBank[CMT2300A_DATA_RATE_BANK_SIZE] = {
    0x95,
    0xC1,
    0x20,
    0x20,
    0xE2,
    0x36,
    0x19,
    0x0A,
    0x9F,
    0x4B,
    0x29,
    0x28,
    0xC0,
    0x4A,
    0x05,
    0x53,
    0x10,
    0x00,
    0xB4,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
};

///* [Baseband Bank] */
//const u8 g_cmt2300aBasebandBank[CMT2300A_BASEBAND_BANK_SIZE] = {
//    0x12,
//    0x08,
//    0x00,
//    0x55,
//    0x03,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x01,
//    0x00,
//    0x00,
//    0x0B,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x00,
//    0x63,
//    0xFF,
//    0x01,
//    0x00,
//    0x1F,
//    0x10,
//};

///* [Tx Bank] */
//const u8 g_cmt2300aTxBank[CMT2300A_TX_BANK_SIZE] = {
//    0x50,
//    0x34,
//    0x19,
//    0x00,
//    0x07,
//    0x70,
//    0x00,
//    0x01,
//    0x01,
//    0x3F,
//    0x7F,

//};
/* [Baseband Bank] */
//no preamble
//sync 2 byte: 0x0001
const u8 g_cmt2300aBasebandBank[CMT2300A_BASEBAND_BANK_SIZE] = {
    0x02,
    0x08,
    0x00,
    0x00,
    0x03,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
    0x09,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x63,
    0xFF,
    0x01,
    0x00,
    0x1F,
    0x10,
};

/* [Tx Bank] */
const u8 g_cmt2300aTxBank[CMT2300A_TX_BANK_SIZE] = {
    0x50,
    0x0E,
    0x16,
    0x00,
    0x0F,
    0x90,
    0x00,
    0x1F,
    0x04,
    0x3F,
    0x7F,
};

#endif

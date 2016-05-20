 /********************************************************************
*   A7129CONFIG.h
*   RF Chip-A7129 Configure Definitions
*
*   This file provides the constants associated with the
*   AMICCOM A7129 device.
*
********************************************************************/
#ifndef _A7129CONFIG_h_
#define _A7129CONFIG_h_

#define DR_100Kbps_100KIFBW     //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz

#ifdef DR_100Kbps_100KIFBW

const  uint16_t A7129Config[]=        //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0021,     //SYSTEM CLOCK register,
    0x0A21,     //PLL1 register,
    0xDA05,     //PLL2 register,    433.301MHz
    0x0000,     //PLL3 register,
    0x0A20,     //PLL4 register,
    0x0024,     //PLL5 register,
    0x0000,     //PLL6 register,
    0x0011,     //CRYSTAL register,
    0x0000,     //PAGEA,
    0x0000,     //PAGEB,
    0x18D4,     //RX1 register,     IFBW=100KHz    odd:0x18D4
    0x7009,     //RX2 register,     by preamble
    0x4000,     //ADC register,     
    0x0800,     //PIN CONTROL register,     Use Strobe CMD
    0x4C45,     //CALIBRATION register,
    0x20C0      //MODE CONTROL register,    Use FIFO mode
};

const  uint16_t A7129Config_PageA[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0xF706,     //TX1 register,     Fdev = 37.5kHz
    0x0000,     //WOR1 register,
    0xF800,     //WOR2 register,
    0x1107,     //RFI register,     Enable Tx Ramp up/down  
    0x1970,     //PM register,
    0x0302,     //RTH register,
    0x400F,     //AGC1 register,    
    0x0AC0,     //AGC2 register, 
    0x0045,     //GIO register,     GIO2=WTR, GIO1=FSYNC
    0xD181,     //CKO register
    0x0004,     //VCB register,
    0x0A21,     //CHG1 register,    430MHz
    0x0022,     //CHG2 register,    435MHz
    0x003F,     //FIFO register,    FEP=63+1=64bytes
    0x1507,     //CODE register,    Preamble=4bytes, ID=4bytes
    0x0000      //WCAL register,
};

const  uint16_t A7129Config_PageB[]=   //433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz), Crystal=12.8MHz
{
    0x0337,     //TX2 register,
    0x8400,     //IF1 register,     Enable Auto-IF, IF=200KHz
    0x0000,     //IF2 register,
    0x0000,     //ACK register,
    0x0000      //ART register,
};

#endif


#ifdef DR_100Kbps_915

const uint16_t A7129Config[16]=  
{
    0x0021,   //SYSTEM CLOCK register,
    0x0A47,   //PLL1 register,
    0x7C05,   //PLL2 register,    915.001MHz
    0x0000,   //PLL3 register,
    0x1A20,   //PLL4 register,
    0x0024,   //PLL5 register,
    0x0000,   //PLL6 register,
    0x0011,   //CRYSTAL register,
    0x0000,   //PAGEA,
    0x0000,   //PAGEB,
    0x18D4,   //RX1 register,   IFBW=100KHz, ETH=1  
    0x7009,   //RX2 register,   by preamble
    0x4000,   //ADC register,
    0x0800,   //PIN CONTROL register,   Use Strobe CMD
    0x4C45,   //CALIBRATION register,
    0x20C0    //MODE CONTROL register,  Use FIFO mode
}; 
const uint16_t A7129Config_PageA[16]=  
{
    0xF706,   //TX1 register,   Fdev = 37.5kHz
    0x0000,   //WOR1 register,
    0xF800,   //WOR2 register,
    0x1107,   //RFI register,   Enable Tx Ramp up/down  
    0x0970,   //PM register,
    0x0302,   //RTH register,
    0x400F,   //AGC1 register,  
    0x2AC0,   //AGC2 register, 
    0x0041,   //GIO register      GIO1->WTR GIO2->WTR 
    0xD181,   //CKO register
    0x0004,   //VCB register,
    0x0247,   //CHG1 register,  910MHz
    0x0E47,   //CHG2 register,  920MHz  
    0x003F,   //FIFO register,  FEP=63+1=64bytes
    0x1507,   //CODE register,  Preamble=4bytes, ID=4bytes
    0x0000    //WCAL register,

};
const uint16_t A7129Config_PageB[5]= 
{
    0x0337,   //TX2 register,   
    0x8400,   //IF1 register,   Enable Auto-IF, IF=200KHz
    0x0000,   //IF2 register,
    0x0000,   //ACK register,
    0x0000    //ART register,
};
#endif



#endif
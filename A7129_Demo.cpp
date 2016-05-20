
#include "A7129_Demo.h"

const Uint8 ID_Tab[8]={0x34,0x75,0xC5,0x8C,0xC7,0x33,0x45,0xE7};   //ID code
Uint16 RxCnt;

/*********************************************************************
** constructor
*********************************************************************/
A7129::A7129(uint8_t _scs, uint8_t _sck, uint8_t _sdio, uint8_t _cko, uint8_t _gio1, uint8_t _gio2) {
	init(_scs, _sck, _sdio, _cko, _gio1, _gio2);
}

A7129::A7129() {
	init(9, 8, 7, 6, 5, 4);
}

void A7129::init(uint8_t _scs, uint8_t _sck, uint8_t _sdio, uint8_t _cko, uint8_t _gio1, uint8_t _gio2) {
	A7129_SCS = _scs;
	A7129_SCK = _sck;
	A7129_SDIO = _sdio;
	A7129_CKO = _cko;
	A7129_GIO1 = _gio1;
	A7129_GIO2 = _gio2;
}

/*********************************************************************
** begin
*********************************************************************/
void A7129::begin(void) {
	RxCnt = 0;
	pinMode(A7129_SCS, OUTPUT);
	pinMode(A7129_SCK, OUTPUT);
	pinMode(A7129_SDIO, OUTPUT);
	pinMode(A7129_CKO, OUTPUT);
	pinMode(A7129_GIO1, OUTPUT);
	pinMode(A7129_GIO2, OUTPUT);
}

/*********************************************************************
** A7129_POR
*********************************************************************/
void A7129::A7129_POR(void) {
	//power on only
    delay(10);   			//for regulator settling time (power on only)
    StrobeCMD(CMD_RF_RST);  	//reset A7139 chip
    while(A7129_WriteID())		//check SPI
    {
		StrobeCMD(CMD_RF_RST);  //reset A7139 chip
		// Serial.print("?");   	
    }
    A7129_WritePageA(PM_PAGEA, A7129Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    delay(2);
    
    entry_deep_sleep_mode();		//deep sleep
    delay(2);
    wake_up_from_deep_sleep_mode();	//wake up
    
    StrobeCMD(CMD_RF_RST);  	//reset A7139 chip
    while(A7129_WriteID())		//check SPI
    {
		StrobeCMD(CMD_RF_RST);  //reset A7139 chip    	
    }
    A7129_WritePageA(PM_PAGEA, A7129Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    delay(2);
}

/*********************************************************************
** InitRF
*********************************************************************/
Uint8 A7129::InitRF(void) {
	//initial pin
	digitalWrite(A7129_SCS, HIGH);
	digitalWrite(A7129_SCK, LOW);
	digitalWrite(A7129_SDIO, HIGH);
	digitalWrite(A7129_CKO, HIGH);
	digitalWrite(A7129_GIO1, HIGH);
	digitalWrite(A7129_GIO2, HIGH);

    delay(1);            //delay 1ms for regulator stabilized
    StrobeCMD(CMD_RF_RST);  //reset A7129 chip
    delay(1);
    
    if(A7129_Config())      //config A7129 chip
        return 1;
    delayMicroseconds(800);          //delay 800us for crystal stabilized

    if(A7129_WriteID())     //write ID code
        return 1;
    if(A7129_Cal())         //IF and VCO Calibration
        return 1;

    return 0;
}

/*********************************************************************
** A7129_Config
*********************************************************************/
Uint8 A7129::A7129_Config(void) {
	Uint8 i;
    Uint16 tmp;

    for(i=0; i<8; i++)
        A7129_WriteReg(i, A7129Config[i]);

    for(i=10; i<16; i++)
        A7129_WriteReg(i, A7129Config[i]);

    for(i=0; i<16; i++)
        A7129_WritePageA(i, A7129Config_PageA[i]);

    for(i=0; i<5; i++)
        A7129_WritePageB(i, A7129Config_PageB[i]);

    //for check 
    tmp = A7129_ReadReg(SYSTEMCLOCK_REG);    
    if(tmp != A7129Config[SYSTEMCLOCK_REG]) { return 1; }
    
    return 0;
}

/*********************************************************************
** wait_txrx_completed
*********************************************************************/
void A7129::wait_txrx_completed(void) {
	pinMode(A7129_GIO2, INPUT);
	while(digitalRead(A7129_GIO2));
	pinMode(A7129_GIO2, OUTPUT);
}

/*********************************************************************
** A7129_Cal
*********************************************************************/
Uint8 A7129::A7129_Cal(void) {
    Uint8 fb, fcd, fbcf;    //IF Filter
    Uint8 vb,vbcf;          //VCO Current
    Uint8 vcb, vccf;        //VCO Band
    Uint16 tmp;
    
    StrobeCMD(CMD_STBY);

    //IF calibration procedure @STB state
    A7129_WriteReg(MODE_REG, A7129Config[MODE_REG] | 0x0802);           //IF Filter & VCO Current Calibration
    do{
        tmp = A7129_ReadReg(MODE_REG);
    }while(tmp & 0x0802);
    
    //for check(IF Filter)
    tmp = A7129_ReadReg(CALIBRATION_REG);
    fb = tmp & 0x0F;
    fcd = (tmp>>11) & 0x1F;
    fbcf = (tmp>>4) & 0x01;
    if(fbcf)
    {
    	Serial.println("fbcf calibration failed");
    	return 1;
    }

    //for check(VCO Current)
    tmp = A7129_ReadPageA(VCB_PAGEA);
    vcb = tmp & 0x0F;
    vccf = (tmp>>4) & 0x01;
    if(vccf)
    {
    	Serial.println("vccf calibration failed");
        return 1;
    }
    
    
    //RSSI Calibration procedure @STB state
    A7129_WriteReg(ADC_REG, 0x4C00);                                    //set ADC average=64
    A7129_WriteReg(MODE_REG, A7129Config[MODE_REG] | 0x1000);           //RSSI Calibration
    do{
        tmp = A7129_ReadReg(MODE_REG);
    }while(tmp & 0x1000);
    A7129_WriteReg(ADC_REG, A7129Config[ADC_REG]);


    //VCO calibration procedure @STB state
    A7129_WriteReg(PLL1_REG, A7129Config[PLL1_REG]);
    A7129_WriteReg(PLL2_REG, A7129Config[PLL2_REG]);
    A7129_WriteReg(MODE_REG, A7129Config[MODE_REG] | 0x0004);       //VCO Band Calibration
    do{
        tmp = A7129_ReadReg(MODE_REG);
    }while(tmp & 0x0004);
    
    //for check(VCO Band)
    tmp = A7129_ReadReg(CALIBRATION_REG);
    vb = (tmp >>5) & 0x07;
    vbcf = (tmp >>8) & 0x01;
    if(vbcf) {
    	Serial.println("vbcf calibration failed");
        return 1;
    }
    
    return 0;   
}

/************************************************************************
**  WriteID
************************************************************************/
Uint8 A7129::A7129_WriteID(void) {
    Uint8 i;
    Uint8 d1, d2, d3, d4;

    digitalWrite(A7129_SCS, LOW);

    ByteSend(CMD_ID_W);
    for(i=0; i<4; i++)
        ByteSend(ID_Tab[i]);

    digitalWrite(A7129_SCS, HIGH);
    delayMicroseconds(4);
    digitalWrite(A7129_SCS, LOW);

    ByteSend(CMD_ID_R);
    d1=ByteRead();
    d2=ByteRead();
    d3=ByteRead();
    d4=ByteRead();
    digitalWrite(A7129_SCS, HIGH);
    
    if((d1!=ID_Tab[0]) || (d2!=ID_Tab[1]) || (d3!=ID_Tab[2]) || (d4!=ID_Tab[3]))
    {
        return 1;
    }
    
    return 0;
}

/*********************************************************************
** Strobe Command
*********************************************************************/
void A7129::StrobeCMD(Uint8 cmd) {
    Uint8 i;
    digitalWrite(A7129_SCS, LOW);
    for(i=0; i<8; i++)
    {
        if(cmd & 0x80)
            digitalWrite(A7129_SDIO, HIGH);
        else
            digitalWrite(A7129_SDIO, LOW);

        _nop_();
        digitalWrite(A7129_SCK, HIGH);
        _nop_();
        digitalWrite(A7129_SCK, LOW);
        cmd<<=1;
    }
    digitalWrite(A7129_SCS, HIGH);
}

/************************************************************************
**  ByteSend
************************************************************************/
void A7129::ByteSend(Uint8 src) {
	Uint8 i;

    for(i=0; i<8; i++)
    {
        if(src & 0x80)
            digitalWrite(A7129_SDIO, HIGH);
        else
            digitalWrite(A7129_SDIO, LOW);

        _nop_();
        digitalWrite(A7129_SCK, HIGH);
        _nop_();
        digitalWrite(A7129_SCK, LOW);
        src<<=1;
    }
}

/************************************************************************
**  ByteRead
************************************************************************/
Uint8 A7129::ByteRead(void) {
    Uint8 i,tmp;

    //read data code
    digitalWrite(A7129_SDIO, HIGH);
    pinMode(A7129_SDIO, INPUT);
    for(i=0; i<8; i++)
    {
        if(digitalRead(A7129_SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        digitalWrite(A7129_SCK, HIGH);
        _nop_();
        digitalWrite(A7129_SCK, LOW);
        _nop_();
    }
    pinMode(A7129_SDIO, OUTPUT);
    return tmp;
}

/************************************************************************
**  A7129_WriteReg
************************************************************************/
void A7129::A7129_WriteReg(Uint8 address, Uint16 dataWord) {
    Uint8 i;

    digitalWrite(A7129_SCS, LOW);
    address |= CMD_Reg_W;
    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            digitalWrite(A7129_SDIO, HIGH);
        else
            digitalWrite(A7129_SDIO, LOW);

		_nop_();
        digitalWrite(A7129_SCK, HIGH);
        _nop_(); 
        digitalWrite(A7129_SCK, LOW);
        address<<=1;
    }
    _nop_();

    //send data word
    for(i=0; i<16; i++)
    {
        if(dataWord & 0x8000)
            digitalWrite(A7129_SDIO, HIGH);
        else
            digitalWrite(A7129_SDIO, LOW);

		_nop_();
        digitalWrite(A7129_SCK, HIGH);
        _nop_();
        digitalWrite(A7129_SCK, LOW);
        dataWord<<=1;
    }
    digitalWrite(A7129_SCS, HIGH);
}

/************************************************************************
**  A7129_ReadReg
************************************************************************/
Uint16 A7129::A7129_ReadReg(Uint8 address) {
    Uint8 i;
    Uint16 tmp;

    digitalWrite(A7129_SCS, LOW);
    address |= CMD_Reg_R;

    for(i=0; i<8; i++)
    {
        if(address & 0x80)
            digitalWrite(A7129_SDIO, HIGH);
        else
            digitalWrite(A7129_SDIO, LOW);

        _nop_(); 
        digitalWrite(A7129_SCK, HIGH);
        _nop_();
        digitalWrite(A7129_SCK, LOW);
        address<<=1;
    }
    _nop_();
    
    //read data code
    digitalWrite(A7129_SDIO, HIGH); //SDIO pull high
    pinMode(A7129_SDIO, INPUT);     
    for(i=0; i<16; i++)
    {
        if(digitalRead(A7129_SDIO))
            tmp = (tmp << 1) | 0x01;
        else
            tmp = tmp << 1;

        digitalWrite(A7129_SCK, HIGH);
        _nop_();
        digitalWrite(A7129_SCK, LOW);
        _nop_();
    }

    digitalWrite(A7129_SCS, HIGH);
    pinMode(A7129_SDIO, OUTPUT);    
    return tmp;
}

/************************************************************************
**  A7129_WritePageA
************************************************************************/
void A7129::A7129_WritePageA(Uint8 address, Uint16 dataWord) {
    Uint16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    A7129_WriteReg(PAGEA_REG, dataWord);
}

/************************************************************************
**  A7129_ReadPageA
************************************************************************/
Uint16 A7129::A7129_ReadPageA(Uint8 address) {
    Uint16 tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7129_ReadReg(PAGEA_REG);
    return tmp;
}

/************************************************************************
**  A7129_WritePageB
************************************************************************/
void A7129::A7129_WritePageB(Uint8 address, Uint16 dataWord) {
    Uint16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    A7129_WriteReg(PAGEB_REG, dataWord);
}

/************************************************************************
**  A7129_ReadPageB
************************************************************************/
Uint16 A7129::A7129_ReadPageB(Uint8 address) {
    Uint16 tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7129Config[CRYSTAL_REG]);
    A7129_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7129_ReadReg(PAGEB_REG);
    return tmp;
}


/*********************************************************************
** A7129_WriteFIFO
*********************************************************************/
void A7129::A7129_WriteFIFO(uint8_t * data, uint8_t len) {
    Uint8 i, max_len = 64;
    if(len > max_len) { len = max_len; }
    StrobeCMD(CMD_TFR);     //TX FIFO address pointer reset
    digitalWrite(A7129_SCS, LOW);
    ByteSend(CMD_FIFO_W);   //TX FIFO write command
    // ByteSend(len);
    for(i = 0; i < len; i++)
        ByteSend(data[i]);
    digitalWrite(A7129_SCS, HIGH);
    digitalWrite(A7129_GIO2, HIGH);
}

/*********************************************************************
** RxPacket
*********************************************************************/
uint8_t A7129::RxPacket(char * out, uint8_t len) {
    Uint8 i, out_len;
    Uint8 recv;
    Uint8 tmp;
    RxCnt++;
    StrobeCMD(CMD_RFR);     //RX FIFO address pointer reset

    digitalWrite(A7129_SCS, LOW);
    ByteSend(CMD_FIFO_R);   //RX FIFO read command

    for(i = 0; i < len; i++)
    {
        out[i] = ByteRead();
    }
    
    digitalWrite(A7129_SCS, HIGH);
    // TODO:
    // for(i = 0; i < out_len; i++)
    // {
    //     recv = tmpbuf[i];
    //     tmp = recv ^ PN9_Tab[i];
    //     if(tmp!=0)
    //     {
    //         Err_ByteCnt++;
    //         Err_BitCnt += (BitCount_Tab[tmp>>4] + BitCount_Tab[tmp & 0x0F]);
    //     }
    // }
    return len;
}


/*********************************************************************
** entry_deep_sleep_mode
*********************************************************************/
void A7129::entry_deep_sleep_mode(void) {
    StrobeCMD(CMD_RF_RST);              //RF reset
    A7129_WriteReg(PIN_REG, A7129Config[PIN_REG] | 0x0800);             //SCMDS=1
    A7129_WritePageA(PM_PAGEA, A7129Config_PageA[PM_PAGEA] | 0x0010);   //QDS=1
    StrobeCMD(CMD_SLEEP);               //entry sleep mode
    delayMicroseconds(600);                      //delay 600us for VDD_A shutdown, C load=0.1uF    
    StrobeCMD(CMD_DEEP_SLEEP);          //entry deep sleep mode
    delayMicroseconds(200);                      //delay 200us for VDD_D shutdown, C load=0.1uF
}

/*********************************************************************
** wake_up_from_deep_sleep_mode
*********************************************************************/
void A7129::wake_up_from_deep_sleep_mode(void) {
    StrobeCMD(CMD_STBY);    //wake up
    delay(2);            //delay 2ms for VDD_D stabilized
}

// TODO:
// void Err_State(void);


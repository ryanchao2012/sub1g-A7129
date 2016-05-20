#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "define.h"
#include "a7129reg.h"
#include "a7129config.h"

const uint16_t PLL2_Chanels[] {
	0xDA05, // 433.301MHz
	0xDE05, // 433.501MHz
	0xE005, // 433.601MHz
	0xE405, // 433.801MHz
	0xE805, // 434.001MHz
	0xEC05, // 434.201MHz
};

const char A7129_ErrorMassage[][30] {
	"no error",
	"fbcf calibration failed",
	"vccf calibration failed",
	"vbcf calibration failed",
	"register config failed",
	"write id failed",
};


class A7129 {
	public:
		A7129();
		A7129(uint8_t _scs, uint8_t _sck, uint8_t _sdio, uint8_t _cko, uint8_t _gio1, uint8_t _gio2, uint8_t _ch);
		void begin(void);
		void powerON(void);
		Uint8 initRF(void);
		void StrobeCMD(Uint8);
		void writeFIFO(uint8_t * data, uint8_t len);
		uint8_t RxPacket(char * out, uint8_t len);
		void entryDeepSleepMode(void);
		void wakeUpFromDeepSleepMode(void);
		bool waitTxRxCompleted(unsigned long _time_out);
		uint8_t switchChannel(uint8_t ch);
		// void Err_State(void);

	private:
		void init(uint8_t _scs, uint8_t _sck, uint8_t _sdio, uint8_t _cko, uint8_t _gio1, uint8_t _gio2, uint8_t _ch);
		Uint8 A7129_Config(void);
		Uint8 A7129_WriteID(void);
		Uint8 A7129_Cal(void);
		
		void ByteSend(Uint8);
		Uint8 ByteRead(void);
		void A7129_WriteReg(Uint8, Uint16);
		Uint16 A7129_ReadReg(Uint8);
		void A7129_WritePageA(Uint8, Uint16);
		Uint16 A7129_ReadPageA(Uint8);
		void A7129_WritePageB(Uint8, Uint16);
		Uint16 A7129_ReadPageB(Uint8);
		void _nop_(void) { asm volatile("nop"); }
		volatile uint8_t A7129_SCS;
		volatile uint8_t A7129_SCK;
		volatile uint8_t A7129_SDIO;
		volatile uint8_t A7129_CKO;
		volatile uint8_t A7129_GIO1;
		volatile uint8_t A7129_GIO2;
		uint8_t A7129_CH;
};


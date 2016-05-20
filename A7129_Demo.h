#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "define.h"
#include "a7129reg.h"
#include "a7129config.h"

class A7129 {
	public:
		A7129();
		A7129(uint8_t _scs, uint8_t _sck, uint8_t _sdio, uint8_t _cko, uint8_t _gio1, uint8_t _gio2);
		void begin(void);
		void A7129_POR(void);
		Uint8 InitRF(void);
		void StrobeCMD(Uint8);
		void A7129_WriteFIFO(uint8_t * data, uint8_t len);
		uint8_t RxPacket(char * out, uint8_t len);
		void entry_deep_sleep_mode(void);
		void wake_up_from_deep_sleep_mode(void);
		void wait_txrx_completed(void);		
		// void Err_State(void);

	private:
		void init(uint8_t _scs, uint8_t _sck, uint8_t _sdio, uint8_t _cko, uint8_t _gio1, uint8_t _gio2);
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
};


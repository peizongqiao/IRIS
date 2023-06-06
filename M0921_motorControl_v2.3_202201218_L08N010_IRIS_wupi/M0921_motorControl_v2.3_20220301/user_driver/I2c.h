#include <atmel_start.h>
#include <stdint-gcc.h>
#include <string.h>

#ifndef _I2C_H_

typedef unsigned char   BYTE;
typedef unsigned short  WORD;

#define _I2C_WRITE_SIZE                 	16
#define _I2C_TIMEOUT_LIMIT                  255

#define _I2C_BUSY                       	0
#define _I2C_OK                         	1

#define _I2C_WR                         	0
#define _I2C_RD                         	1

#define _FAIL      0
#define _SUCCESS   1
//----------------------------------------------------------------------------------------------------

#define  SCL   2 //PA2
#define  SDA   1 //PA1

#define NACK 0
#define ACK  1

#define INPUTI2CSDA()				PORTA_set_pin_dir(SDA,0)
#define OUTPUTI2CSDA()				PORTA_set_pin_dir(SDA,1)

#define SETI2CSDA()                 PORTA_set_pin_level(SDA,1)
#define CLRI2CSDA()                 PORTA_set_pin_level(SDA,0)
            
#define INPUTI2CSCL()				PORTA_set_pin_dir(SCL, 0)
#define OUTPUTI2SCL()                  PORTA_set_pin_dir(SCL,1)
#define SETI2CSCL()                  PORTA_set_pin_level(SCL,1)
#define CLRI2CSCL()                  PORTA_set_pin_level(SCL,0)

#define GETI2CSDA()					PORTA_get_pin_level(SDA)
#define GETI2CSCL()					PORTA_get_pin_level(SCL)
  
#define PULLUPSCL()		  PORTA_set_pin_pull_mode(SCL,PORT_PULL_OFF)
#define PULLUPSDA()		   PORTA_set_pin_pull_mode(SDA,PORT_PULL_OFF)


#define  SDA_1()        {PORTA_set_pin_dir(SDA,1);PORTA_set_pin_level(SDA,1);}
#define  SDA_0()        {PORTA_set_pin_dir(SDA,1);PORTA_set_pin_level(SDA,0);}
	
#define SCL_1()                  {PORTA_set_pin_dir(SCL,1);PORTA_set_pin_level(SCL,1);} 
#define SCL_0()                  {PORTA_set_pin_dir(SCL,1);PORTA_set_pin_level(SCL,0);}    
	
	
	
	
#define IIC_SCL 
extern void __delay_cycles(volatile uint16_t cnt);

extern void Delay5us(void);
extern void i2c_init(void);
extern void __delay_ms(volatile uint16_t cnt);
int8_t i2c_write_block(uint8_t ucSlaveDeviceAddr,uint8_t numBytes,unsigned char multi, void* TxData);
int8_t i2c_read_block(uint8_t ucSlaveDeviceAddr,uint8_t numBytes,void* RxData);
//extern uint8_t read_vol_IIC(uint8_t ucSlaveDeviceAddr,uint8_t rx_numBytes,void* RxData,uint8_t tx_numBytes,void* TxData);
extern int8_t CI2cRead_Block_TEST(uint8_t ucSlaveDeviceAddr,uint8_t cmd, uint8_t numBytes,void* RxData);
#endif


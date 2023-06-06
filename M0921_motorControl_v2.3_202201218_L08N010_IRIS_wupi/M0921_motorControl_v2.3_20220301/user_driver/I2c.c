#include "i2c.h"


BYTE CI2cSendByte(BYTE ucValue);
uint8_t errorflag = 0;

//----------------------------------------------------------------------------------------------------
// I2C communication basic function
//----------------------------------------------------------------------------------------------------
void __delay_cycles(uint16_t cnt)
{
	while(cnt--)
	{
		__asm__ __volatile__("nop");
		//__asm__ __volatile__("nop");
	}
}

void __delay_ms(uint16_t cnt)
{
	int i,j;
	
	for(i=cnt;i>0;i--)
	for(j=1100;j>0;j--);
}
void Delay4us(void)
{
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	__asm__ __volatile__("nop");
	
  
}

BYTE CHECKI2CSDA(void)  
{
	 return  PORTB_get_pin_level(SDA);
}

//校验IIC-SCL的状态
BYTE CHECKI2CSCL(void)
{
	BYTE level = 0;
	PORTB_set_pin_dir(SCL,0);
	level = PORTB_get_pin_level(SCL);
	
	return level;
}

//初始化IIC
void IIC_Init(void)
{			
  OUTPUTI2CSDA();
  OUTPUTI2SCL();
  
  PULLUPSCL();
  PULLUPSDA();
  
  SETI2CSCL();
  SETI2CSDA();

}
//产生IIC起始信号
void IIC_Start(void)
{
	OUTPUTI2CSDA();     //sda线输出
	//__delay_cycles(1);	
	SETI2CSDA();	 	 	  
	//SETI2CSCL();
	//__delay_cycles(5);	
 	CLRI2CSDA();//START:when CLK is high,DATA change form high to low 
	__delay_cycles(20);	
	CLRI2CSCL();//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	OUTPUTI2CSDA();//sda线输出
	__delay_cycles(1);
	CLRI2CSCL();
	CLRI2CSDA();//STOP:when CLK is high DATA change form low to high
 	Delay4us();
	SETI2CSCL();
	Delay4us();		
	SETI2CSDA();//发送I2C总线结束信号
	Delay4us();						   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	 
	 INPUTI2CSDA();      //change
	//__delay_cycles(5);
	CLRI2CSCL();
	//__delay_cycles(5);
	__delay_cycles(12);	
	SETI2CSCL();//时钟输出0 			 
	while(CHECKI2CSDA())
	{
		ucErrTime++;
		if(ucErrTime> 10)
		{
			IIC_Stop();
			return 1;
		}
		//__delay_cycles(10);
	}   
	
	__delay_cycles(10);
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	CLRI2CSCL();
	OUTPUTI2CSDA();
	CLRI2CSDA();
	__delay_cycles(10);
	SETI2CSCL();
	__delay_cycles(10);
	
	//INPUTI2CSDA();
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	CLRI2CSCL();
	OUTPUTI2CSDA();
	__delay_cycles(10);
	SETI2CSDA();
	__delay_cycles(10);
	SETI2CSCL();
	__delay_cycles(10);
	CLRI2CSCL();
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	OUTPUTI2CSDA(); 
	__delay_cycles(6);   
    for(t=0;t<8;t++)
    {              
		
		 CLRI2CSCL();//拉低时钟开始数据传输
        if((txd&0x80))
		{
			SETI2CSDA();
		}
		else
		{
			CLRI2CSDA();
		}
        txd<<=1; 	  
		__delay_cycles(15);   //对TEA5767这三个延时都是必须的
		SETI2CSCL();
		__delay_cycles(15); 
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,dat = 0;
	CLRI2CSCL();
	__delay_cycles(5);//import切换到输入注意延时一定的时间
	INPUTI2CSDA();//SDA设置为输入
    for(i = 0;i < 8;i++ )
	{
        CLRI2CSCL();
        __delay_cycles(20);
		SETI2CSCL();
        dat <<= 1;
        if(CHECKI2CSDA()) dat++;   
		__delay_cycles(20); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return dat;
}



int8_t CI2cWrite_Block(uint8_t ucSlaveDeviceAddr,uint8_t numBytes,unsigned char multi, void* TxData)
{
	uint8_t  i;
	uint8_t *temp;
	BYTE control;

	temp = (uint8_t *)TxData;           // Initialize array pointer
	control = ucSlaveDeviceAddr & (~0x01);
	IIC_Start();
	IIC_Send_Byte(control);
	if(IIC_Wait_Ack())
	{
		return _FAIL;
	}
	
	for (i = 0; i < numBytes ; i++)
	{
		IIC_Send_Byte(*(temp));      // Send data and ack
		
		if(IIC_Wait_Ack())//如果没有收到应答
		{
			return _FAIL;
		}
		CLRI2CSCL();
		temp ++;                                 // Increment pointer to next element
	}
	
	if (multi == 0)
	{	
		IIC_Stop();                              // Quick delay
	}
	
	return _SUCCESS;
}

int8_t CI2cRead_Block(uint8_t ucSlaveDeviceAddr,uint8_t numBytes,void* RxData)
{
	uint8_t  i;
	uint8_t* temp;
	BYTE control;

	temp = (uint8_t *)RxData;           // Initialize array pointer
	control = ucSlaveDeviceAddr | 0x01;
	IIC_Start();
	IIC_Send_Byte(control);
	if(IIC_Wait_Ack())
	{
		return _FAIL;
	}
	
	
	for (i = 0; i < numBytes ; i++)
	{
		Delay4us();
		if(i == numBytes -1)
		{
			*(temp) = IIC_Read_Byte(NACK);// Read last 8-bit data with no ACK
		}
		else
		{
			*(temp) = IIC_Read_Byte(ACK);// Read last 8-bit data with no ACK
		}
		

		temp++;                                 // Increment pointer to next element
	}
	
	IIC_Stop();                   // Send Stop condition
	
	return _SUCCESS;
}

int8_t CI2cRead_Block_TEST(uint8_t ucSlaveDeviceAddr,uint8_t cmd, uint8_t numBytes,void* RxData)
{
	uint8_t  i;
	uint8_t* temp;
	BYTE control;
	IIC_Start();
	IIC_Send_Byte(ucSlaveDeviceAddr);
	if(IIC_Wait_Ack())
	{
		return _FAIL;
	}
	IIC_Send_Byte(cmd);
	if(IIC_Wait_Ack())
	{
		return _FAIL;
	}
	
	CLRI2CSCL();
	OUTPUTI2CSDA();
	SETI2CSDA();
	__delay_cycles(50);
	SETI2CSCL();
	temp = (uint8_t *)RxData;           // Initialize array pointer
	control = ucSlaveDeviceAddr | 0x01;
	IIC_Start();
	IIC_Send_Byte(control);
	if(IIC_Wait_Ack())
	{
		return _FAIL;
	}
	
	
	for (i = 0; i < numBytes ; i++)
	{
		Delay4us();
		if(i == numBytes -1)
		{
			*(temp) = IIC_Read_Byte(NACK);// Read last 8-bit data with no ACK
		}
		else
		{
			*(temp) = IIC_Read_Byte(ACK);// Read last 8-bit data with no ACK
		}
		

		temp++;                                 // Increment pointer to next element
	}
	
	IIC_Stop();                   // Send Stop condition
	
	return _SUCCESS;
}

uint8_t read_vol_IIC(uint8_t ucSlaveDeviceAddr,uint8_t rx_numBytes,void* RxData,uint8_t tx_numBytes,void* TxData)
{
	uint8_t  i;
	//uint8_t *temp;
	BYTE control;

	//temp = (uint8_t *)TxData;           // Initialize array pointer
	control = ucSlaveDeviceAddr & (~0x01);
	IIC_Start();
	IIC_Send_Byte(control);
	if(IIC_Wait_Ack())
	{
		errorflag = 1;
		return 0;
	}
	for (i = 0; i < tx_numBytes ; i++)
	{
		IIC_Send_Byte(0x0d);      // Send data and ack
		if(IIC_Wait_Ack())
		{
			errorflag = 1;
			return 0;
		}
		TxData ++;                                 // Increment pointer to next element
	}
	
	control = ucSlaveDeviceAddr | 0x01;
	IIC_Start();
	IIC_Send_Byte(control);
	if(IIC_Wait_Ack())
	{
		errorflag = 1;
		return 0;
	}
	
	
	for (i = 0; i < rx_numBytes ; i++)
	{
		Delay4us();
		if(i == rx_numBytes -1)
		{
			*((uint8_t *)RxData) = IIC_Read_Byte(0);// Read last 8-bit data with no ACK
			//CI2cSendNoAck();
		}
		else
		{
			*((uint8_t *)RxData) = IIC_Read_Byte(1);// Read last 8-bit data with no ACK
			//CI2cSendAck();
		}
		

		RxData++;                                 // Increment pointer to next element
	}
	
	IIC_Stop();                   // Send Stop condition
	return true;
	
}






















//==============================================================================
/*

void SWI2CMST_writeBlock(uint8_t ucSlaveDeviceAddr, uint8_t numBytes, unsigned char multi, void* TxData)
{

	uint8_t  i;
	uint8_t *temp;
	BYTE control;

	temp = (uint8_t *)TxData;           // Initialize array pointer
	control = ucSlaveDeviceAddr | _I2C_WR;
	if(CI2cStart(control) != _I2C_OK)  // Send Start condition
	return _FAIL;

	for (i = 0; i < numBytes ; i++)
	{
		CI2cSendByte(*(temp));      // Send data and ack
		temp ++;                                 // Increment pointer to next element
	}
	
	if (multi == 0)
	{
		CI2cStop();                              // Quick delay
	}
	

	return _SUCCESS;
}


int8_t SWI2CMST_readBlock(uint8_t ucSlaveDeviceAddr,uint8_t numBytes,void* RxData)
{
	uint8_t  i;
	uint8_t* temp;
	BYTE control;

	temp = (uint8_t *)RxData;           // Initialize array pointer
	control = ucSlaveDeviceAddr | _I2C_RD;
	if(CI2cStart(control) != _I2C_OK)  // Send Start condition
	return _FAIL;
	
	for (i = 0; i < numBytes - 1; i++)
	{
		*(temp) = CI2cGetByte();// Read last 8-bit data with no ACK
		CI2cSendAck();
		temp++;                                 // Increment pointer to next element
	}
	*(temp) = CI2cGetByte();// Read last 8-bit data with no ACK
	CI2cSendNoAck();
	CI2cStop();                   // Send Stop condition
	
	return _SUCCESS;
}

*/
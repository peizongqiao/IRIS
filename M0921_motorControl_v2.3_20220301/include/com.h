/**
 * \file
 *
 * \brief Tiny System Related Support
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_utils_mcu_init MCU Init
 * \ingroup doc_driver_utils
 *
 * \section doc_driver_system_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

#ifndef COM_INCLUDED
#define COM_INCLUDED

void SendChar(unsigned char InData);
void SendString(unsigned char *ptr);
void SendHexChar(unsigned char n);
void SendNumb(unsigned int n,unsigned char number_bit);
void CT(void);//chang tab==enter
unsigned char Rece_Wait(void);//wait until the RI=1;
unsigned char Rece_Timer(void);//wait for RI=1 for a little time;
void SendBitSeries(unsigned short data_in);

//------------------------------------------------------------------
//发送一个字节
//------------------------------------------------------------------
void SendChar(unsigned char InData)
{
	while (!(USART0.STATUS & USART_DREIF_bm))
	{
		;
	}
	USART0.TXDATAL=InData;
}
//------------------------------------------------------------------
//发送一串字符
//------------------------------------------------------------------
void SendString(unsigned char *ptr)
{
	unsigned char i,l=0;
	while(ptr[l]>31) l++;//get length
	for(i=0;i<l;i++) SendChar(*(ptr+i));
}
//-------------------------------------------------------------------
//发送一组数字，位数可变
//--------------------------------------------------------------------------
void SendNumb(unsigned int n,unsigned char number_bit)
{	
	unsigned char digi[6];
	unsigned char i;
	digi[0]=n/100000;n=n-digi[0]*100000;	
	digi[1]=n/10000;n=n-digi[1]*10000;	
	digi[2]=n/1000;n=n-digi[2]*1000;
	digi[3]=n/100;n=n-digi[3]*100;
	digi[4]=n/10;n=n-digi[4]*10;
	digi[5]=n;
	for(i=0;i<number_bit;i++) SendChar(digi[6-number_bit+i]+0x30);
}
//-------------------------------------------------------------------
//回车、换行
//-------------------------------------------------------------------
void CT(void)
{	
	SendChar(0x0d);SendChar(0x0a);
}
//-------------------------------------------------------------------
//发送一个字节按16进制显示
//--------------------------------------------------------------------------
void SendHexChar(unsigned char n)
{	
	unsigned char h,l;

	SendChar('0');SendChar('x');

	h=(n&0xf0)>>4;if(h>9) h+=0x57;else h+=0x30;
	l=(n&0x0f);if(l>9) l+=0x57;else l+=0x30;
	SendChar(h);SendChar(l);

	SendChar(',');
}/*
//--------------------------------------------------------------------
//查询接收（一直查询）
//--------------------------------------------------------------------
unsigned char Rece_Wait(void)//wait until the RI=1;
{   unsigned char a;
	while(RI!=1);
	RI=0;
	a=SBUF;
	return(a);
}
//--------------------------------------------------------------------
//查询接收（查询一段时间）
//--------------------------------------------------------------------
unsigned char Rece_Timer(void)//wait for RI=1 for a little time;
{   unsigned char a=0;
	unsigned int i;
	for(i=0;i<100;i++)
	{	if(RI) 
		{	RI=0;
			a=SBUF;
			break;
		}
	}
	return(a);
}
void SendBitSeries(unsigned short data_in)
{
	unsigned char i;
	for(i=0; i<16; i++)
	{
		if(data_in&0x8000)
			{SendChar('1');}
		else
			{SendChar('0');}
		data_in = data_in << 1;
		if((i==3)||(i==7)||(i==11))
		{
			SendChar('_');
		}
	}
	CT();
}*/

#endif /* COM_INCLUDED */

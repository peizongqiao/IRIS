/*============================================================================
Filename : timer.h
Dis:This file is for timer
Author:TF
Verison；v1.0
Date:2018-4-23
History:
------------------------------------------------------------------------------
Copyright (c) 2017 Microchip. All rights reserved.
------------------------------------------------------------------------------
============================================================================*/
#include "timer.h"

uint16_t gSysTick10ms = 0;
uint16_t gForce_status_change_timer = 0;

uint16_t get_system_ticker(void);
/**************************************************************************************************
** Func  :  get_system_ticker
** Des    : 得到系统时钟
** Arg     : None
**
** Return: 当前时间
***************************************************************************************************/
uint16_t get_system_ticker(void)
{
	uint16_t ticker = 0;
	uint8_t tmp = 0;

	
	if(SREG & 0x80 )
	{
		tmp = 1;
		SREG &= ~0x80;
	}
	ticker = gSysTick10ms;
	if (tmp)
	{
		SREG |= 0x80;
	}
	
	
	return ticker;
}


/**************************************************************************************************
** Func  : GetTimeDelta
** Des    : 得到时间差
** Arg     : 前一次时间 现在时间
** Return: 时间差
***************************************************************************************************/
uint16_t GetTimeDelta(uint16_t oldTime, uint16_t nowTime)
{
	uint16_t timeDelta = 0;
	timeDelta = (nowTime >= oldTime )? (nowTime -oldTime):(nowTime + (0xFFFF - oldTime));
	return timeDelta;
}

void time_polling(void)
{
	if((gI2cMemory[iic_addr_tx_Keystatus] == 0) && (gInput_signal == 0 )&& ( (gSoc_state == SOC_STATE_MUSIC) ||  (gSoc_state== SOC_STATE_PROJECTOR) || (gSoc_state == SOC_STATE_LEDOFF) ))
	{
		set_drv_iic_tx_flg();
	}
}

ISR(TCA0_OVF_vect)
{
	TCA0.SINGLE.INTFLAGS |= 1; //clear flag.
	TaskRemarks();	// 长按时间计数器
	gSysTick10ms ++ ;//systicks
	gForce_status_change_timer ++;//状态切换强制转化时间
	
}
/**************************************************************************************************
** File Name   : timer.h
** Description : This file is for timer
** Author      : fu.tang
** Verison     : V1.0
** History     : 2018-4-23
**
**
**
**************************************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "utils.h"

extern uint16_t get_system_ticker(void);
extern uint16_t gForce_status_change_timer;
extern uint16_t GetTimeDelta(uint16_t oldTime, uint16_t nowTime);
extern void time_polling(void);

#endif /* TIMER_H_ */
/**
 * \file utils.h
 *
 * \brief Different macros.
 *
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */

/**
 * \defgroup doc_driver_utils AVR Code utility functions
 *
 * Compiler abstraction layer and code utilities for AVR.
 * This module provides various abstraction layers and utilities
 * to make code compatible between different compilers.
 *
 * \{
 */

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <atmel_start.h>
#include <stdint-gcc.h>
#include <string.h>


//#define G08X300
#define G10N000

//#define MOTOR_PROTECTED		//注释掉则禁用保护功能

#if defined(G05) 
	#define P47
#endif

#if defined(G08_X100) || defined(G08_X200) || defined(G08X300)
	#define P33
#endif

#if defined(P33_MS) ||  defined(P33_MP)
    #define P33
#endif

#if defined(G10N000)
	#define P23
#endif

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

/**
 * \brief Retrieve array size
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/** @} */

#define container_of(ptr, type, member) ({      \
 const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
  (type *)( (char *)__mptr - offsetof(type,member) );})  

struct list_head { 
	struct list_head *next, *prev; 
};

static inline void init_list_head(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)
	
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)
	
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

#define true                              1
#define false                             0

void set_aprerture(bool onoff);
void add_key_event(void);

/********驱动*********/
#include "user_driver/timer.h"//定时器
#include "user_driver/xgimi_driver_key.h"//实体按键驱动
#include "user_driver/I2c.h"//i2c
#include "utils.h"
#include "src/control_task.h"
#include "i2c_slave.h"
#include "mcu_bus.h"

#ifdef __cplusplus
}
#endif
#endif /* UTILS_H_INCLUDED */

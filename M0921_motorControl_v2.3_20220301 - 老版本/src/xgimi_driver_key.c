/**************************************************************************************************
** File Name   : xgimi_driver_key.h
** Description : This file is for timer
** Author      : fu.tang
** Verison     : V1.0
** History     : 2018-4-23
**
**
**
**************************************************************************************************/
#include "user_driver/xgimi_driver_key.h"
#include "IR.h"

static struct list_head key_head;
extern bool ir_use_flag;
void key_add(bool (*key_check)(void), void (*key_cb)(eKeyType), int type, bool (*key_press_cb)(void))
{
	static uint8_t key_value = 1;
	struct key_def *key = malloc(sizeof(struct key_def));
	memset(key, 0, sizeof(struct key_def));
	key->KeyType = eKeyType_NULL;
	key->KeyValue = key_value++;
	key->key_check = key_check;
	key->key_cb = key_cb;
	key->cb_type = type;      //判断 是红外还是按键
	key->key_press_cb = key_press_cb;    //判断按键是否一直按下
	list_add_tail(&key->qset, &key_head);
}

void key_init(void)
{
	init_list_head(&key_head);
	add_key_event();
	rtc_init();
}


/**************************************************************************************************
** Func  :   get_gpio_key_value
** Des    :  得到gpio键值
** Arg     : 返回具体是哪一个键，键的ID号
** Return:   void
***************************************************************************************************/
static struct key_def* get_gpio_key_value(void)
{
	struct key_def *key;
	list_for_each_entry(key, &key_head, qset) {
		if (key->key_check() == true) {
			return key;
		}
	}
	return NULL;
}



/**************************************************************************************
* FunctionName   : get_key_value
* Description    : 得到按键值和类型
* EntryParameter : value:键值地址  type:类型地址
* ReturnValue    : None
**************************************************************************************/
void key_process(void)
{
	static eKeyDetectSta s_u8KeyState = KEY_STATE_INIT ;
	static uint16_t prvKeyDownTime = 0 ;
    struct key_def *key;	
	static struct key_def *last_key = NULL;
	
	key = get_gpio_key_value();    //获取键值
	
	if (key!=NULL && key->cb_type==eNoWobble && ir_use_flag) {  
		SREG &= ~0x80;  //关中断
		key->key_cb(last_key->KeyType);
		SREG |= 0x80;
		return ;
	}
		
	if ((key!=NULL) && (last_key!=NULL)) {
		if (key->KeyValue != last_key->KeyValue) {
			last_key = key;
			s_u8KeyState = KEY_STATE_INIT;
		}
	}
	
	switch(s_u8KeyState)
	{
		case KEY_STATE_INIT:
		{
			if(key != NULL)
			{
				last_key = key;
				s_u8KeyState = KEY_STATE_WOBBLE;
				prvKeyDownTime = get_system_ticker();
			}
		}
		break;
			
		case KEY_STATE_WOBBLE:       //消抖
		{
			static bool key_press = false;
			if(key != NULL)
			{
				if(GetTimeDelta(prvKeyDownTime,get_system_ticker()) > TIME_KEY_DOWN_VALID)  //按下处理
				{
					key_press = true;
					
//					PI2_LED_L;
//					PI3_LED_L;
					
					//改成只掉一次
					if (last_key->key_press_cb) {  //一直按下
						last_key->key_press_cb();
					}
				}
			}
			else
			{
				if (key_press == true) {
					s_u8KeyState = KEY_STATE_PRESS;
					key_press = false;
				} else {
					s_u8KeyState = KEY_STATE_INIT;
				}
			}
		}
		break;
			
		case KEY_STATE_PRESS:   //弹起处理,单次按键
		{
			if (last_key) {
				SREG &= ~0x80;
				last_key->key_cb(last_key->KeyType);
				SREG |= 0x80;
			}
			s_u8KeyState = KEY_STATE_INIT;
		}
		break;
			
		default:
		break;
	}
}



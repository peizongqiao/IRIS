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
	key->cb_type = type;      //�ж� �Ǻ��⻹�ǰ���
	key->key_press_cb = key_press_cb;    //�жϰ����Ƿ�һֱ����
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
** Des    :  �õ�gpio��ֵ
** Arg     : ���ؾ�������һ����������ID��
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
* Description    : �õ�����ֵ������
* EntryParameter : value:��ֵ��ַ  type:���͵�ַ
* ReturnValue    : None
**************************************************************************************/
void key_process(void)
{
	static eKeyDetectSta s_u8KeyState = KEY_STATE_INIT ;
	static uint16_t prvKeyDownTime = 0 ;
    struct key_def *key;	
	static struct key_def *last_key = NULL;
	
	key = get_gpio_key_value();    //��ȡ��ֵ
	
	if (key!=NULL && key->cb_type==eNoWobble && ir_use_flag) {  
		SREG &= ~0x80;  //���ж�
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
			
		case KEY_STATE_WOBBLE:       //����
		{
			static bool key_press = false;
			if(key != NULL)
			{
				if(GetTimeDelta(prvKeyDownTime,get_system_ticker()) > TIME_KEY_DOWN_VALID)  //���´���
				{
					key_press = true;
					
//					PI2_LED_L;
//					PI3_LED_L;
					
					//�ĳ�ֻ��һ��
					if (last_key->key_press_cb) {  //һֱ����
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
			
		case KEY_STATE_PRESS:   //������,���ΰ���
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



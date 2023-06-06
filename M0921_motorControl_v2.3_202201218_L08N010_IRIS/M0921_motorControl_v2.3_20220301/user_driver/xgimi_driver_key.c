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
#include "xgimi_driver_key.h"

#define KEY_NUM 6
_KeyDefStruct  KeyStruct[] = 
{
	{eKeyType_NULL,eKEY_VolumeUP,0,0},//������
	{eKeyType_NULL,eKEY_VolumeDown,0,0},//������
	{eKeyType_NULL,eKEY_PlayPrev,0,0},//��һ��
	{eKeyType_NULL,eKEY_PlayNext,0,0 },//��һ��
	{eKeyType_NULL,eKEY_Update,0,1 },//��һ����һ��ͬʱ����
	{eKeyType_NULL,eKEY_Play,1,1 },//���ż�
//	{eKeyType_NULL,eKEY_NULL,0,0 },//û�а���
}	;
	
	
/**************************************************************************************************
** Func  :   get_key_hw_value
** Des    :  ��ȡӲ������ֵ
** Arg     : void
** Return:   void
***************************************************************************************************/
uint8_t get_key_hw_value(void)
{
	uint8_t key_hw_value = eKEY_NULL;
		
	if(Check_Left_TouchButton())
	{
		key_hw_value |= KEY1_FLAG;
	}

	if(Check_Right_TouchButton())
	{
		key_hw_value |= KEY2_FLAG;
	}
		
	if(Check_Right_TouchButton() && Check_Left_TouchButton() )
	{
		key_hw_value &= ~(KEY1_FLAG + KEY2_FLAG) ;
	}
	if (KeyLeftIsPressed())
	{
		key_hw_value |= KEY3_FLAG;
	}
	if (KeyRightIsPressed())
	{
		key_hw_value |= KEY4_FLAG;
	}
	if (KeyMiddleIsPressed())
	{
		key_hw_value |= KEY5_FLAG;
	}	
	
		

	return key_hw_value;
}

/**************************************************************************************************
** Func  :   get_gpio_key_value
** Des    :  �õ�gpio��ֵ
** Arg     : ���ؾ�������һ����������ID��
** Return:   void
***************************************************************************************************/
void get_gpio_key_value(_KeyDefStruct *str)
{
	int i = 0;
	eKeyValue  CurKey = eKEY_NULL;
		
	CurKey = (eKeyValue)get_key_hw_value();
    if(CurKey == eKEY_NULL)
    {
        return ;
    }
		
	for(i = 0;i < KEY_NUM; i++)
	{
		if(KeyStruct[i].KeyValue == CurKey )
		{
			str->KeyValue =  KeyStruct[i].KeyValue;
			str->need_doubleclick =  KeyStruct[i].need_doubleclick;
			str->need_longclick =  KeyStruct[i].need_longclick;
			break;
		}
	}
}



/**************************************************************************************
* FunctionName   : get_key_value
* Description    : �õ�����ֵ������
* EntryParameter : value:��ֵ��ַ  type:���͵�ַ
* ReturnValue    : None
**************************************************************************************/
_KeyDefStruct  get_key_value(void)
{
	static eKeyDetectSta s_u8KeyState = KEY_STATE_INIT ;
	static uint16_t prvKeyDownTime = 0 ;
	uint16_t NowTicker = 0;
	uint16_t TickDelta = 0;
	static eKeyValue s_u8LastKey = eKEY_NULL ;   //���水���ͷ�ʱ��ļ�ֵ
   _KeyDefStruct key = {eKeyType_NULL,eKEY_NULL,0,0,0};
	static uint8_t	doubleclick_flag = 0;
	
	
	get_gpio_key_value(&key);    //��ȡ��ֵ
	
	switch(s_u8KeyState)
	{
		case KEY_STATE_INIT:
		{
			if(eKEY_NULL != key.KeyValue)
			{
				s_u8KeyState = KEY_STATE_WOBBLE ;
				prvKeyDownTime = get_system_ticker();
			}
		}
		break;
			
		case KEY_STATE_WOBBLE:       //����
		{
			if(eKEY_NULL != key.KeyValue)
			{
				if(GetTimeDelta(prvKeyDownTime,get_system_ticker()) > TIME_KEY_DOWN_VALID)
				{
					prvKeyDownTime = get_system_ticker();
					s_u8KeyState = KEY_STATE_PRESS ;
					s_u8LastKey = key.KeyValue;			
				}
			}
			else
			{
				s_u8KeyState = KEY_STATE_INIT;
			}
		}
		break;
			
		case KEY_STATE_PRESS:
		{
			if(key.need_longclick == true)//�Ƿ�֧�ֳ���
			{
				s_u8KeyState = KEY_STATE_LONG;		
			}
			else//��֧��
			{
				 if(key.need_doubleclick == true)//�Ƿ���Ҫ˫��
				 {
					s_u8KeyState = KEY_STATE_DOUBLECLICK;
				 }
				 else
				 {
					key.KeyType  = eKeyType_Down ;
					s_u8KeyState = KEY_STATE_CONTINUE;  
				 }
				 prvKeyDownTime = get_system_ticker(); //�õ�ϵͳʱ��
			}
		}
		break;
		
		case KEY_STATE_LONG:
		{
			 NowTicker =  get_system_ticker();
             TickDelta = GetTimeDelta(prvKeyDownTime, NowTicker);
			 
			if(eKEY_NULL != key.KeyValue)
			{
				if((TickDelta > TIME_KEY_LONG_PERIOD) &&(s_u8LastKey == key.KeyValue))
				{
						key.KeyType  = eKeyType_Long;   //�������¼�����
						prvKeyDownTime = get_system_ticker();
						s_u8KeyState = KEY_STATE_CONTINUE;
				}
				 if(key.need_doubleclick == true)//�Ƿ���Ҫ˫��
				 {
					 doubleclick_flag = true;
				 }
				 else
				 {
					 doubleclick_flag = false;
				 }
			}
			else
			{
				 if(doubleclick_flag)//�Ƿ���Ҫ˫��
				 {
					s_u8KeyState = KEY_STATE_DOUBLECLICK;
				 }
				 else
				 {
					key.KeyType  = eKeyType_Down ;
					s_u8KeyState = KEY_STATE_CONTINUE;  
				 }
				prvKeyDownTime = get_system_ticker(); //�õ�ϵͳʱ��
			}
		}
		break;
		
		
		case KEY_STATE_DOUBLECLICK:
		{
			   NowTicker =  get_system_ticker();
               TickDelta = GetTimeDelta(prvKeyDownTime, NowTicker);	   
				if(key.need_doubleclick == true)
				{
					if((eKEY_NULL != key.KeyValue)&&(s_u8LastKey == key.KeyValue)&&(TickDelta < TIME_KEY_DOUBLE_KICLK)&&(TickDelta > TIME_KEY_DOWN_VALID))
					{
						key.KeyType  = eKeyType_DoubleClick;   //˫���¼�
						s_u8KeyState = KEY_STATE_CONTINUE;
						break;
					}
				}
			
				if(TickDelta > TIME_KEY_DOUBLE_KICLK)
				{
					key.KeyType  = eKeyType_Down;   //�̰����¼�����
				//	key.KeyValue = s_u8LastKey;
					s_u8KeyState = KEY_STATE_RELEASE;
				}					 	
							    
		}
		break;
			
		case KEY_STATE_CONTINUE:
		{
			if(eKEY_NULL != key.KeyValue)
			{
				if(s_u8LastKey != key.KeyValue)
				{
					s_u8KeyState = KEY_STATE_INIT;
				}
				if(GetTimeDelta(prvKeyDownTime,get_system_ticker()) > TIME_KEY_CONTINUE_PERIOD)
				{
					prvKeyDownTime = get_system_ticker();
					key.KeyType = eKeyType_Continue;   //����������
				}
			}
			else
			{
				s_u8KeyState = KEY_STATE_LONG_RELEASE;
			}
		}
		break;
			
		case KEY_STATE_RELEASE:
		{
			key.KeyType = eKeyType_Up;   //����̧��
			doubleclick_flag = false;
			key.KeyValue = s_u8LastKey;
			s_u8KeyState = KEY_STATE_INIT;
		}
		break;

		case KEY_STATE_LONG_RELEASE:
		{
			key.KeyType = eKeyType_LongUp;
			key.KeyValue = s_u8LastKey;
			s_u8KeyState = KEY_STATE_INIT;
		}
		
		break;
			
		default:
		break;
	}
	
	return key;
}



#ifndef _XGIMI_DRIVER_KEY_H_
#define _XGIMI_DRIVER_KEY_H_

#include "utils.h"

#define     Check_Left_TouchButton()           (get_sensor_state(0) & 0x80)
#define     Check_Right_TouchButton()          (get_sensor_state(1) & 0x80)
//发IIC的标志
#define KEY1_FLAG BIT0
#define KEY2_FLAG BIT1
#define KEY3_FLAG BIT2
#define KEY4_FLAG BIT3
#define KEY5_FLAG BIT4





typedef enum{
	eKeyType_NULL = 0,  //无状态
//	eKeyType_FirstDown = 1,  //按键按下
	eKeyType_Down =1,  //按键按下
	eKeyType_Long =2 , //长按
	eKeyType_Continue=3 ,  //按键连发
	eKeyType_Up =4,      //按键抬起
	eKeyType_LongUp = 5,  //长按
	eKeyType_DoubleClick = 6,//双击
}eKeyType;  //按键类型

typedef enum {
	eWobble = 0,	//需要消抖
	eNoWobble = 1,
}cbType;


typedef enum{
	eKEY_NULL = 0x00,	//没有按键按下
	eKEY_VolumeUP = 0x01,//声音加
	eKEY_VolumeDown = 0x02,//声音减
	eKEY_PlayPrev = 0x04,//前一首
	eKEY_PlayNext = 0x08,//下一首
	eKEY_Update = 0x0C,//双键同时按下升级
	eKEY_Play= 0x10,//双键同时按下升级
	
}eKeyValue;     //实体按键键值


typedef enum
{
	KEY_STATE_INIT = 0,
	KEY_STATE_WOBBLE = 1,
	KEY_STATE_PRESS = 2,
	KEY_STATE_LONG = 3,
	KEY_STATE_CONTINUE = 4,
	KEY_STATE_RELEASE = 5,
	KEY_STATE_LONG_RELEASE = 6,
	KEY_STATE_DOUBLECLICK = 7,
	KEY_STATE_DOUBLECLICK_RELEASE = 8,
}eKeyDetectSta;         //按键侦测状态

typedef struct key_def
{
	eKeyType KeyType;
	eKeyValue KeyValue;
	bool need_doubleclick;//是否需要双击
	bool need_longclick;//是否需要长按
	bool need_continueclick;//是否需要连发
	int cb_type;     //区分红外还是按键
	bool (*key_check)(void);
	void (*key_press_cb)();
	void (*key_cb)(eKeyType type);
	struct list_head qset;
	//eKeyValue LastKeyValue,//按键时间
}_KeyDefStruct;

//定义长按键的TICK数,以及连_发间隔的TICK数
#define TIME_KEY_DOWN_VALID         4       //按键按下生效时间
#define TIME_KEY_LONG_PERIOD        300     //长按时间
#define TIME_KEY_CONTINUE_PERIOD    30     //按键连发间隔时间
#define TIME_KEY_DOUBLE_KICLK       20 //uintms * 20

#define KeyRightIsPressed()    (get_key_next_level())   //Right Key
#define KeyMiddleIsPressed()   (get_key_play_level())   //Middle  Key
#define KeyLeftIsPressed()     ( get_key_prv_level())   //Left  Key

void key_init(void);
void key_add(bool (*key_check)(void), void (*key_cb)(eKeyType), int type, bool (*key_press_cb)(void));
void key_process(void);

#endif


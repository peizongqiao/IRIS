#ifndef _XGIMI_DRIVER_KEY_H_
#define _XGIMI_DRIVER_KEY_H_

#include "utils.h"

#define     Check_Left_TouchButton()           (get_sensor_state(0) & 0x80)
#define     Check_Right_TouchButton()          (get_sensor_state(1) & 0x80)
//��IIC�ı�־
#define KEY1_FLAG BIT0
#define KEY2_FLAG BIT1
#define KEY3_FLAG BIT2
#define KEY4_FLAG BIT3
#define KEY5_FLAG BIT4





typedef enum{
	eKeyType_NULL = 0,  //��״̬
//	eKeyType_FirstDown = 1,  //��������
	eKeyType_Down =1,  //��������
	eKeyType_Long =2 , //����
	eKeyType_Continue=3 ,  //��������
	eKeyType_Up =4,      //����̧��
	eKeyType_LongUp = 5,  //����
	eKeyType_DoubleClick = 6,//˫��
}eKeyType;  //��������

typedef enum {
	eWobble = 0,	//��Ҫ����
	eNoWobble = 1,
}cbType;


typedef enum{
	eKEY_NULL = 0x00,	//û�а�������
	eKEY_VolumeUP = 0x01,//������
	eKEY_VolumeDown = 0x02,//������
	eKEY_PlayPrev = 0x04,//ǰһ��
	eKEY_PlayNext = 0x08,//��һ��
	eKEY_Update = 0x0C,//˫��ͬʱ��������
	eKEY_Play= 0x10,//˫��ͬʱ��������
	
}eKeyValue;     //ʵ�尴����ֵ


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
}eKeyDetectSta;         //�������״̬

typedef struct key_def
{
	eKeyType KeyType;
	eKeyValue KeyValue;
	bool need_doubleclick;//�Ƿ���Ҫ˫��
	bool need_longclick;//�Ƿ���Ҫ����
	bool need_continueclick;//�Ƿ���Ҫ����
	int cb_type;     //���ֺ��⻹�ǰ���
	bool (*key_check)(void);
	void (*key_press_cb)();
	void (*key_cb)(eKeyType type);
	struct list_head qset;
	//eKeyValue LastKeyValue,//����ʱ��
}_KeyDefStruct;

//���峤������TICK��,�Լ���_�������TICK��
#define TIME_KEY_DOWN_VALID         4       //����������Чʱ��
#define TIME_KEY_LONG_PERIOD        300     //����ʱ��
#define TIME_KEY_CONTINUE_PERIOD    30     //�����������ʱ��
#define TIME_KEY_DOUBLE_KICLK       20 //uintms * 20

#define KeyRightIsPressed()    (get_key_next_level())   //Right Key
#define KeyMiddleIsPressed()   (get_key_play_level())   //Middle  Key
#define KeyLeftIsPressed()     ( get_key_prv_level())   //Left  Key

void key_init(void);
void key_add(bool (*key_check)(void), void (*key_cb)(eKeyType), int type, bool (*key_press_cb)(void));
void key_process(void);

#endif


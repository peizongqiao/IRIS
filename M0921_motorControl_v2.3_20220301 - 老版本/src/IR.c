/*
 * IR.c
 *
 * Created: 2019/7/31 22:08:45
 *  Author: Ade.tang
 */ 

#include <IR.h>
#include <port.h>
#include <atmel_start_pins.h>
#include "utils.h"

volatile static uint8_t  ir_level = 0;
volatile static uint8_t Num_time=0;
volatile static uint8_t keyNum = 0;
static volatile uint8_t reptTimes = 0;
volatile uint8_t reptIR = 0;
volatile uint8_t reptIRchecked = 0;		//��Timer1��ʱ��Χ�ڣ�0-δ��⵽rept�źţ�1-��⵽rept�ź�

/*static struct ir_data{
	uint8_t pulse;
	uint8_t level;
};*/
//volatile struct ir_data get_ir_data[65];
volatile uint8_t get_ir_data[65];
bool ir_use_flag = true;
volatile static uint8_t data_flag=0;
volatile static uint8_t start_flag = 0;
volatile static uint8_t ir_decode_flag =0;
volatile static uint8_t pulse_bnum=0;
static uint8_t ir_sign_error=0;
volatile static uint8_t addrH,addrL,IRData,IRRevData;  //��ַ������
//static uint8_t ir_left,ir_right,ir_up,ir_down;  // left == 80  right ==16  up == 144  down == 128  lleft ==224 ok == 208 rright ==160  return == 176
volatile uint8_t ir_key = 0;
volatile uint8_t ir_keyBuf = 0;


void rtc_init()
{
	while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */				
	}
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc /* 1 */
	| 0 << RTC_RUNSTDBY_bp; /* Run In Standby: disabled */
	
	RTC.CLKSEL = 0x0; //1kƵ��   ??
	
	//����PA1�ⲿ�ж�
//	PORTA.PIN1CTRL = (0x1<<3) //����
//	| (0x1<<0);  //˫���ش���

//	PORTA_set_pin_dir(6,0);
	PORTA.PIN6CTRL = (0x1<<3) //����
	| (0x1<<0);  //˫���ش���
	
	TCB_init();
}

//��ʱ��ʼ
 void rtc_start()
{
	while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */	
	}//��������Ҫ����ݱ�־���������¼���
	RTC.CNT = 0x0;
	RTC.CTRLA |= 0x01;
}

//���ؼ�ʱʱ��,����0.1ms
 uint16_t rtc_stop()
{
	RTC.CTRLA &= ~(0x1);
	volatile float cnt = RTC.CNT;
	return (uint16_t)(cnt/3.2768);
}



 uint8_t ir_decode()
{
	
	for(keyNum=1;keyNum < 17;keyNum++){
		addrH <<=1;
		Num_time=get_ir_data[keyNum]+get_ir_data[++keyNum]; // �Ѹߵ͵�ƽ������
		if((Num_time>10)&&(Num_time<20)) // �Ƿ���2.25ms,���Ƿ����߼�1
		{
			addrH+=1;
		}
	}
	for(keyNum=17;keyNum < 33;keyNum++){
		addrL <<=1;
		Num_time=get_ir_data[keyNum]+get_ir_data[++keyNum]; // �Ѹߵ͵�ƽ������
		if((Num_time>10)&&(Num_time<20)) // �Ƿ���2.25ms,���Ƿ����߼�1
		{
			addrL+=1;
		}
	}
	for(keyNum=33;keyNum < 49;keyNum++){
		IRData <<=1;
		Num_time=get_ir_data[keyNum]+get_ir_data[++keyNum]; // �Ѹߵ͵�ƽ������
		if((Num_time>10)&&(Num_time<20)) // �Ƿ���2.25ms,���Ƿ����߼�1
		{
			IRData+=1;
		}
	}
	for(keyNum=49;keyNum < 65;keyNum++){
		IRRevData <<=1;
		Num_time=get_ir_data[keyNum]+get_ir_data[++keyNum]; // �Ѹߵ͵�ƽ������
		if((Num_time>10)&&(Num_time<20)) // �Ƿ���2.25ms,���Ƿ����߼�1
		{
			IRRevData+=1;
		}
	}
		
	if(addrH==(uint8_t)~addrL  && IRRevData == (uint8_t)~IRData) // �жϽ��������Ƿ���ȷ
	{
		memset(get_ir_data,0,sizeof(get_ir_data));
		return IRData;		
	}
	else {
		memset(get_ir_data,0,sizeof(get_ir_data));
		return 0;
	}
	
}

static uint8_t k;
 void get_ir_buf(volatile uint8_t Num_ms)
{
	if(Num_ms < 100 && Num_ms > 70){   //�ж�9ms
		start_flag = 1;
	}
	
	else if(Num_ms < 55 && Num_ms > 30 && start_flag){ //�ж�4.5ms
		data_flag =1;
		start_flag =0;
		pulse_bnum = 0;
	}
	
	else if(Num_ms < 30 && Num_ms > 15 && start_flag){ //�ж�2.3ms	ң���ظ�ָ�� ����
		
		data_flag = 0;
		start_flag = 0;
		reptIR = 1;
		ir_key = k;
	}
	
	if(data_flag){
		get_ir_data[pulse_bnum++] = Num_ms;
		if(pulse_bnum == 65 ){
			pulse_bnum = 0;
			data_flag = 0;
			k = ir_decode();
			if(k == 64){     //�ж��Ƿ�ʹ��ң����
				ir_use_flag = !ir_use_flag;
			}
			//if(ir_key==136 || ir_key==168 || ir_key==64  || ir_key==160){	//add��sub������ ���ӳ�������
				//ir_keyBuf = ir_key;
				//TCB0.CTRLA |= 0x01;		//�򿪶�ʱ�� 
			//}
		}
	}
}

//static uint8_t ir_left,ir_right,ir_up,ir_down;  // left == 80  right ==16  up == 144  down == 128  lleft ==224  ok == 208 rright ==160  return == 176  onoff== 184    jingyin == 64 
 //stop == 152  sub == 168 add==136 menu == 120  menur == 248
 bool is_key_mute()
 {
	 if(ir_key == 64){
		ir_key = 0;
		return true;
		}else{
		return false;
	}
 }
 
 bool ir_key_up()
{
	if(ir_key == 144){
		ir_key = 0;
		return true;
		}else{
		return false;
	}
}

 bool ir_key_down()
{
	if(ir_key == 128){
		ir_key = 0;
		return true;
		}else{
		return false;
	}
}

 bool ir_key_left()
{
	if(ir_key == 80){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}

 bool ir_key_right()
{
	if(ir_key == 16){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}

 bool ir_key_rright()
{	
	if(ir_key == 160){
		ir_key = 0;
		return true;
	}else{
		return false;
	}
}
 bool ir_key_lleft()
{
	if(ir_key == 224){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}
 bool ir_key_ok()
{
	if(ir_key == 208){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}
 bool ir_key_return()
{
	if(ir_key == 176){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}

bool ir_key_onoff()
{
	if(ir_key == 184){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}

bool ir_key_add()
{
	if(ir_key == 136){
//		ir_key = 0;		//���������ܣ��豣�ְ���ֵ
		return true;
		}else{
		return false;
	}
}

bool ir_key_sub()
{
	if(ir_key == 168){
//		ir_key = 0;		//���������ܣ��豣�ְ���ֵ
		return true;
		}else{
		return false;
	}	
}

bool ir_key_menu()
{
	if(ir_key == 120){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}

bool ir_key_menur()
{
	if(ir_key == 248){
		ir_key = 0;
		return true;
		}else{
		return false;
	}	
}

bool ir_key_stop() //�л�iicͨ��
{
	if(ir_key == 152){
		ir_key = 0;
		return true;
		}else{
		return false;
	}
}

ISR(PORTA_PORT_vect)
{
	//PA6�ж�
	PORTA.INTFLAGS |= 0x40;  //0xff
	
	volatile  uint8_t Num_ms;
	Num_ms = rtc_stop();
	rtc_start();//���¿�ʼ����
	
	//�ùܽ�״̬
	//ir_level = PC3_get_level();
	
	//----->�ռ�����  �����ƽ������
	get_ir_buf(Num_ms);
	
}

TCB_init()		//��ʱ��B ����ң���ظ���־�ͳ����ļ��
{
	TCB0.CTRLA = 0x02;	//������ ��ͬ�� 2��Ƶ ��ʹ��
	TCB0.CTRLB = 0x00;	//�������ж�ģʽ
	TCB0.INTFLAGS |= 0x01;	//����жϱ�־
	TCB0.INTCTRL |= 0x01;	//ʹ���ж�
	TCB0.CCMP = 50000;		//20MHz/2_50000 ->5ms
}

ISR(TCB0_INT_vect)
{
	static uint8_t TCB0_Cnt = 0;
	
	TCB0.INTFLAGS |= 0x01;
	TCB0_Cnt++;
	{
		if(TCB0_Cnt>=24)	//5ms * 24 = 120ms
		{
			TCB0_Cnt = 0;
			if(reptIR)
			{
				reptTimes++;
				if(reptTimes>=2)
				{
					reptTimes = 0;
					ir_key = ir_keyBuf;
					reptIRchecked = 1;
				}
			}
			else
			{
				reptTimes = 0;
				reptIRchecked = 0;
				TCB0.CTRLA &= ~0x01;		//ֹͣ����
				TCB0.CNT = 0;
			}
			reptIR = 0;
		}
	}
}

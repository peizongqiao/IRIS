/*
motor contrl 1.0.3
by ade.tang
*/
#include <atmel_start.h>
#include <pwm_basic.h>
#include <atomic.h>
#include <adc_basic.h>
#include <math.h>
#include "user_driver/i2c.h"
#include "all_headers.h"
#include "utils.h"
#include "motorContrl.h"
#include "delay.h"
#include "com.h"

//drv8846 M0、M1控制，细分参数选择
#define M0_0	PORTB_set_pin_level(3, 0)
#define M0_1	PORTB_set_pin_level(3, 1)
#define M1_0	PORTC_set_pin_level(3, 0)
#define M1_1	PORTC_set_pin_level(3, 1)

//drv8846 I0、I1控制，驱动电流百分比选择
#define I0_0	PORTB_set_pin_level(5, 0)
#define I0_1	PORTB_set_pin_level(5, 1)
#define I1_0	PORTB_set_pin_level(4, 0)
#define I1_1	PORTB_set_pin_level(4, 1)

//drv8846 nSLEEP使能控制
#define	icpower_on		PORTB_set_pin_level(0, 1)
#define	icpower_off		PORTB_set_pin_level(0, 0)

//drv8846 nENBL使能控制
#define	motor_on		PORTC_set_pin_level(3, 0)
#define	motor_off		PORTC_set_pin_level(3, 1)

//drv8846 DIR控制，正反转选择
#define	motor_cw		PORTC_set_pin_level(0, 1)
#define	motor_ccw		PORTC_set_pin_level(0, 0)

//LED ON/OFF
#define	led1_on			PORTB_set_pin_level(2, 1)
#define	led1_off		PORTB_set_pin_level(2, 0)
#define	led2_on			PORTB_set_pin_level(3, 1)
#define	led2_off		PORTB_set_pin_level(3, 0)


volatile uint8_t motor_pps = 800;		//PPS设置
volatile uint8_t step_mode = 32;		//细分参数
volatile uint8_t step_edge = 1;			//边沿触发；0：rising-edge only；1：rising and falling edge
volatile float torque = 1.000;			//驱动电流百分比


//PPS设置
void motor_pwm()
{
	int i;
	int motor_pps = 800;
	for(i=0;i<step_mode/2;i++){
		PORTB_set_pin_level(1, 1);
		_delay_us(1000000/motor_pps);
		PORTB_set_pin_level(1, 0);
		_delay_us(1000000/motor_pps);
	}
}

//drv8846 M0、M1控制，细分参数选择
void set_step_mode()
{
	if(step_mode == 1){									//full step,rising-edge only
		M1_0;
		M0_0;
	}else if(step_mode == 4){							//1/4 step,rising-edge only
		M1_0;
		M0_1;
	}else if(step_mode == 16 && step_edge == 1){		//16 micro steps/step,rising and falling edge
		M1_1;
		M0_0;
	}else if(step_mode == 32 && step_edge == 1){		//32 micro steps/step,rising and falling edge
		M1_1;
		M0_1;
	}
}

//drv8846 I0、I1控制，驱动电流百分比选择
void set_torque()
{
	if(torque == 1.000){			//100%
		I1_0;
		I0_0;
	}else if(torque == 0.750){		//75%
		I1_0;
		I0_1;
	}else if(torque == 0.250){		//25%
		I1_1;
		I0_0;
	}else if(torque == 0.000){		//0%		//outputs disabled
		I1_1;
		I0_1;
	}
}

//MOTOR初始化
void motor_init()
{
	//设置Pin为输出
	PORTB_set_pin_dir(0, 1);		//Motor_ON/OFF
	PORTC_set_pin_dir(3, 1);		//Motor_nENBL
	PORTB_set_pin_dir(1, 1);		//Motor_PWM
	PORTC_set_pin_dir(0, 1);		//Motor_CW/CCW
	PORTB_set_pin_dir(2, 1);		//LED1
	PORTB_set_pin_dir(3, 1);		//LED2
	
	//MOTOR初始化
	icpower_on;
	motor_on;
	led1_off;
	led2_off;
	
// 	if(motor2WaitTop()){
// 		run_ccw_end();
// 	}
}

//
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
		
	motor_init();
	
	/**初始化显示器oled**/
//	OLED_Init();			//初始化OLED
//	OLED_Clear();
	
	task_init();
	key_init();	
	task_add(motorKeepRun,3);    //按下一直向前 或向后  10
			
	/* Enable interrupts */
	SREG |= 0x80;

	while (1) {
//		SendString("USART PORT Right!");CT();
		task_process();
		key_process();
	}
}

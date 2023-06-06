/*
 * motorContrl.h
 *
 * Created: 2019/11/22 11:05:20
 *  Author: Ade.tang
 */ 


#ifndef MOTORCONTRL_H_
#define MOTORCONTRL_H_
#include <compiler.h>
#include "utils.h"
#define MOTOR1_A_H	PORTC_set_pin_level(0,1)
#define MOTOR1_A_L	PORTC_set_pin_level(0,0)
#define MOTOR1_B_H	PORTA_set_pin_level(3,1)
#define MOTOR1_B_L	PORTA_set_pin_level(3,0)
#define MOTOR1_C_H	PORTA_set_pin_level(5,1)
#define MOTOR1_C_L	PORTA_set_pin_level(5,0)
#define MOTOR1_D_H	PORTA_set_pin_level(4,1)
#define MOTOR1_D_L	PORTA_set_pin_level(4,0)
//B he C 管脚定义交换一下   JMD1 接口  调焦  （板子上标注的motor2）

#define MOTOR2_A_H	PORTB_set_pin_level(0,1)
#define MOTOR2_A_L	PORTB_set_pin_level(0,0)
#define MOTOR2_B_H	PORTB_set_pin_level(2,1)
#define MOTOR2_B_L	PORTB_set_pin_level(2,0)
#define MOTOR2_C_H	PORTB_set_pin_level(1,1)
#define MOTOR2_C_L	PORTB_set_pin_level(1,0)
#define MOTOR2_D_H	PORTB_set_pin_level(3,1)
#define MOTOR2_D_L	PORTB_set_pin_level(3,0)
//b 和 c 交换   JMD3 接口  位移  （板子上标准的motor1）

#define PI2_LED_H PORTB_set_pin_level(4,1)
#define PI2_LED_L PORTB_set_pin_level(4,0)
#define PI3_LED_H PORTB_set_pin_level(5,1)
#define PI3_LED_L PORTB_set_pin_level(5,0)

//电机1 调焦
uint8_t motor1_run(uint16_t step);
uint8_t motor1_run_back(uint16_t step);
uint8_t motor1_run_front(uint16_t step);
//电机2 位移
uint8_t motor2_run(uint16_t step);
uint8_t motor2_run_back(uint16_t step);   //向上
uint8_t motor2_run_front(uint16_t step);  //向下

uint16_t motor2Calibration();

void motor2Run1p3();
void motor2Run2p3();
bool motor2_up_end(eKeyType type);
bool motor2_down_end(eKeyType type);

void motor1RunFont();
void motor1RunBack();
void motorRunStop();
void motor2RunFont();
void motor2RunBack();
void motor1RunFontIR();
void motor1RunBackIR();
void motor2RunFontIR();
void motor2RunBackIR();

bool isPIbottom();
bool isPItop();
uint8_t readPIstatus();
void motorRunFont(uint8_t step);
void motorRunBack(uint8_t step);
void motorRunFont1();
void motorRunFont19();
void motorRunFont16();
void motorRunFont23();
void motorRunFont30();
void motorRunBack1();
void motorRunBack7();
void motorRunBack19();
void motorRunBack28();
void motorRunBack30();
void run_cw_step2();
void run_ccw_step2();

void motorAging();
void run_cw_end();
void run_ccw_end();

void motorKeepRun(void);

//记录位移校准数据
bool config_read_eeprom(uint8_t *data, uint8_t size);
bool config_write_eeprom(uint8_t *data, uint8_t size);

#endif /* MOTORCONTRL_H_ */
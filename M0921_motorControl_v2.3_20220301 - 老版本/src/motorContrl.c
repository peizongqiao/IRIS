/*
 * motorContrl.c
 *
 * Created: 2019/11/22 11:05:37
 *  Author: Ade.tang
 */ 
#include "motorContrl.h"
#include "ccp.h"
#include "utils.h"
#include "delay.h"

//LED ON/OFF
#define	led1_on			PORTB_set_pin_level(2, 1)
#define	led1_off		PORTB_set_pin_level(2, 0)
#define	led2_on			PORTB_set_pin_level(3, 1)
#define	led2_off		PORTB_set_pin_level(3, 0)

#define  MOTOR_MAX_STEP 20000
#define  MOTOR_MIN_STEP 0
#define  AGING_TIMES_MAX 20000	//�ϻ�ѭ������
//#define  motorPIMaxStep 1500

//ADC0	PA5	ͨ��ΪAIN5
#define IRIS_PI		5
//PI�͵�ƽ�����ֵ��ѹ
#define PI_V		0.2
//PI�ߵ�ƽ�����ֵ��ѹ3.0
#define PI_LIMIT	3.0
//������ѹ���Ϊ0.1
#define PI_DELTA	0.1

static bool modeChoose = true; //ѡ������ģʽ ���� ��װģʽ�������ת�ٲ�ͬ

static volatile uint16_t current_step1 = 20000;  //��ǰ����
static volatile uint16_t current_step2 = 20000;
static volatile uint8_t motor1Direction=0;		//G08X300�ĵ������״̬
static volatile uint8_t motor2Direction=0;	
static volatile uint8_t motorSteps2Run=0;		//G10N000�ĵ�������벽��
static volatile uint16_t agingTimesCnt;
uint8_t PI_err = 0;			//���ᵽ���͵��׼��ʧ��ʱ��λ
uint8_t agingMode = false;		//0-����ģʽ, 1-�ϻ�ģʽ

volatile uint16_t calbrationPPS[4];
volatile uint16_t calbrationSteps;
volatile  uint16_t allSteps;


extern volatile uint8_t ir_key;
extern volatile uint8_t reptIRchecked;

bool motor1WaitTop();
bool motor2WaitTop();
bool motor2WaitBottom();

/*******eeprom ��д*****/
typedef uint16_t eeprom_adr_t;
typedef enum {
	NVM_OK    = 0, ///< NVMCTRL free, no write ongoing.
	NVM_ERROR = 1, ///< NVMCTRL operation retsulted in error
	NVM_BUSY  = 2, ///< NVMCTRL busy, write ongoing.
} nvmctrl_status_t;

void FLASH_0_read_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size)
{

	// Read operation will be stalled by hardware if any write is in progress
	memcpy(data, (uint8_t *)(EEPROM_START + eeprom_adr), size);
}

nvmctrl_status_t FLASH_0_write_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size)
{
	uint8_t *write = (uint8_t *)(EEPROM_START + eeprom_adr);

	/* Wait for completion of previous write */
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)
	;

	/* Clear page buffer */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);

	do {
		/* Write byte to page buffer */
		*write++ = *data++;
		size--;
		// If we have filled an entire page or written last byte to a partially filled page
		if ((((uintptr_t)write % EEPROM_PAGE_SIZE) == 0) || (size == 0)) {
			/* Erase written part of page and program with desired value(s) */
			ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
		}
	} while (size != 0);

	return NVM_OK;
}

bool config_read_eeprom(uint8_t *data, uint8_t size)
{
	bool ret = false;
	uint8_t *buf = malloc(size);
	FLASH_0_read_eeprom_block(0, buf, size);
	memcpy(data, buf, size);
	free(buf);
	return true;
}

bool config_write_eeprom(uint8_t *data, uint8_t size)
{
	if (FLASH_0_write_eeprom_block(0, data, size) != NVM_OK) {
		return false;
	}
	return true;
}
/*******eeprom ��д*****/


void delay_ms(volatile uint16_t cnt)
{
	volatile int i,j;
	
	for(i=cnt;i>0;i--)
	for(j=1100;j>0;j--);
}


uint8_t motor1_run_stop()
{
	MOTOR1_A_H;
	MOTOR1_B_H;
	MOTOR1_C_H;
	MOTOR1_D_H;
	return 0;
}

uint8_t motor2_run_stop()
{
	MOTOR2_A_H;
	MOTOR2_B_H;
	MOTOR2_C_H;
	MOTOR2_D_H;
	return 0;
}

void motorRunStop()
{
	motor1Direction = 0;
	motorSteps2Run = 0;
}

#ifdef G08X300
void motor1RunFont()   
{
	motor1Direction=1;
}
void motor1RunBack()
{
	motor1Direction=2;
}

void motor2RunFont()
{
	motor1Direction=3;
}

void motor2RunBack()
{
	motor1Direction=4;
}

#endif

void motor1RunFontIR()
{
	if(!reptIRchecked)
	{
		motor1_run_front(1);
		motor1Direction = 0;
		ir_key = 0;
	}
	else
	{
		motor1Direction = 1;		//����״̬�£����ֵ���ƶ�
	}
}
void motor1RunBackIR()
{
	if(!reptIRchecked)
	{
		motor1_run_back(1);
		motor1Direction = 0;
		ir_key = 0;
	}
	else
	{
		motor1Direction = 2;
	}
}

void motor2RunFontIR()
{
	motor2_run_front(1);
	motor2Direction = 0;
}

void motor2RunBackIR()
{
	motor2_run_back(1);
	motor2Direction = 0;
}

#ifdef G10N000

void motorRunFont(uint8_t step)
{
	motorSteps2Run = step;
}

void motorRunBack(uint8_t step)
{
	motorSteps2Run = step+128;
}

//4��8�� A-AB-B-BC-C-CD-D-DA-A  12v 500hz 1-2�༤����4��8�ģ�
uint8_t motor1_run(uint16_t step)
{
	uint8_t run_step = step % 8;
	
	switch(run_step){
		case 7:
		MOTOR1_A_H;
		MOTOR1_B_H;
		MOTOR1_C_L;
		MOTOR1_D_H;
		break;
		case 6:
		MOTOR1_A_H;
		MOTOR1_B_H;
		MOTOR1_C_L;
		MOTOR1_D_L;
		break;
		case 5:
		MOTOR1_A_H;
		MOTOR1_B_H;
		MOTOR1_C_H;
		MOTOR1_D_L;
		break;
		case 4:
		MOTOR1_A_L;
		MOTOR1_B_H;
		MOTOR1_C_H;
		MOTOR1_D_L;
		break;
		case 3:
		MOTOR1_A_L;
		MOTOR1_B_H;
		MOTOR1_C_H;
		MOTOR1_D_H;
		break;		
		case 2:
		MOTOR1_A_L;
		MOTOR1_B_L;
		MOTOR1_C_H;
		MOTOR1_D_H;
		break;
		case 1:
		MOTOR1_A_H;
		MOTOR1_B_L;
		MOTOR1_C_H;
		MOTOR1_D_H;
		break;
		case 0:
		MOTOR1_A_H;
		MOTOR1_B_L;
		MOTOR1_C_L;
		MOTOR1_D_H;
		break;		
		default:
		MOTOR1_A_H;
		MOTOR1_B_H;
		MOTOR1_C_H;
		MOTOR1_D_H;
		break;		
	}

	uint16_t delay;
	delay = calbrationPPS[0];	
	while(delay--);
	return 1;
}


//4��8�� A-AB-B-BC-C-CD-D-DA-A  12v 500hz 1-2�༤����4��8�ģ�  �� ���� ������ͬ
uint8_t motor2_run(uint16_t step)
{

	uint8_t run_step = step % 8;
	
	switch(run_step){
		case 7:
		MOTOR2_A_H;
		MOTOR2_B_H;
		MOTOR2_C_L;
		MOTOR2_D_H;
		break;
		case 6:
		MOTOR2_A_H;
		MOTOR2_B_H;
		MOTOR2_C_L;
		MOTOR2_D_L;
		break;
		case 5:
		MOTOR2_A_H;
		MOTOR2_B_H;
		MOTOR2_C_H;
		MOTOR2_D_L;
		break;
		case 4:
		MOTOR2_A_L;
		MOTOR2_B_H;
		MOTOR2_C_H;
		MOTOR2_D_L;
		break;
		case 3:
		MOTOR2_A_L;
		MOTOR2_B_H;
		MOTOR2_C_H;
		MOTOR2_D_H;
		break;
		case 2:
		MOTOR2_A_L;
		MOTOR2_B_L;
		MOTOR2_C_H;
		MOTOR2_D_H;
		break;
		case 1:
		MOTOR2_A_H;
		MOTOR2_B_L;
		MOTOR2_C_H;
		MOTOR2_D_H;
		break;
		case 0:
		MOTOR2_A_H;
		MOTOR2_B_L;
		MOTOR2_C_L;
		MOTOR2_D_H;
		break;
		default:
		MOTOR2_A_H;
		MOTOR2_B_H;
		MOTOR2_C_H;
		MOTOR2_D_H;
		break;
	}
	uint16_t delay;
	delay = calbrationPPS[1];
	while(delay--);	
	return 1;
}


uint8_t motor1_run_front(uint16_t step)
{
	for(uint16_t i=0;i<step;i++)
	{
		motor1_run(current_step1++);
	}
	motor1_run_stop();
	return 0;
}

uint8_t motor1_run_back(uint16_t step)
{
	for(uint16_t i=0;i<step;i++)
	{
		if (motor1WaitTop())
		{
			motor1_run_stop();
			return 0;
		}
		motor1_run(current_step1--);
	}
	motor1_run_stop();
	return 0;
}

uint8_t motor2_run_front(uint16_t step)
{
	for(uint16_t i=0;i<step;i++)
	{
		if (motor2WaitBottom())    //�����ƶ�
		{
			motor2_run_stop();
			return 1;
		}
		motor2_run(current_step2++);
		if(!PORTC_get_pin_level(2))
		{
			calbrationSteps++;
		}
	}
	motor2_run_stop();
	return 0;
}

uint8_t motor2_run_back(uint16_t step)
{
	for(uint16_t i=0;i<step;i++)
	{
		if (motor2WaitTop())   //���� �ƶ�
		{
			motor2_run_stop();
			return 1;
		}
		motor2_run(current_step2--);
			
		if(!PORTC_get_pin_level(3))
		{
			calbrationSteps++;
		}
	}
	motor2_run_stop();
	return 0;
}


 bool motor1WaitTop()
{
	PORTC_set_pin_dir(1, 0);
	return !PORTC_get_pin_level(1);
}

bool isPIbottom()
{
	return !PORTC_get_pin_level(2);
}

bool isPItop()
{
	return !PORTC_get_pin_level(3);
}

uint8_t readPIstatus()
{
	uint8_t status = 0;
	if (isPItop()) {
		status |= 0x01;
	}
	if (isPIbottom()) {
		status |= 0x02;
	}
	return status;
}

static bool PI_check(uint8_t channal, float value)
{
	float v;
	v = (float)ADC_0_get_conversion(channal)*3.3/255;  // 8λ��255 10λ��1023
	
	// 	int num;							//����������ѹ���ڴ�ӡ���
	// 	num = 100*v;
	// 	SendString("PI_V:");
	// 	SendNumb(num, 3);CT();
	
	if(v > value){
		return false;
	}
	if(v - value < PI_DELTA){
		return true;
	}else{
		return false;
	}
}

static bool PI_limit(uint8_t channal, float value)
{
	float v;
	v = (float)ADC_0_get_conversion(channal)*3.3/255;  // 8λ��255 10λ��1023
	if(v < value){
		return false;
	}
	if(v - value > PI_DELTA){
		return true;
	}else{
		return false;
	}
}

bool motor2WaitTop()
{
	led1_off;

	if(PI_check(IRIS_PI, PI_V)){
		led1_on;
 		return true;		
	}else{
		led1_off;
		return false;		
	}
}

void motorRunFont1()
{
	run_cw_step(1);
}

void motorRunFont21()
{
	motorSteps2Run = 21;
}

void motorRunFont16()
{
	run_ccw_end();
	run_cw_step(16);
}



void motorRunFont23()
{
	run_ccw_end();
	run_cw_step(23);
}

void motorRunFont30()
{

	run_cw_step(30);
}

void motorRunBack1()
{
	motorSteps2Run = 1+128;
}

void motorRunBack7()
{
	motorSteps2Run = 7+128;
}

void motorRunBack17()
{
	motorSteps2Run = 17+128;
}

void motorRunBack28()
{
	motorSteps2Run = 28+128;
}

void motorRunBack30()
{
	motorSteps2Run = 30+128;
}

void motorAging()
{
	if(agingMode == false)
	{
		agingMode = true;
		agingTimesCnt = 0;
	}
	else {
		agingMode = false;
	}
}
#endif

void run_cw_step(uint8_t step)
{
	int i;
	
	if(step>30){
//		PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
		PORTC_set_pin_level(3, 1);
		_delay_ms(50);
		return 1;
	}
	
	led1_off;	
	
//	PORTB_set_pin_level(0, 1);				//�ϵ�����
	PORTC_set_pin_level(3, 0);
	
	PORTC_set_pin_level(0, 1);
	_delay_ms(50);
	for(i=0;i<step;i++){					//��תstep��
		motor_pwm();
	}
//	PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
//	PORTC_set_pin_level(3, 1);
}

void run_ccw_step(uint8_t step)
{
	int i;
	
	if(motor2WaitTop()){
//		PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
		PORTC_set_pin_level(3, 1);
		return 1;
	}

//	PORTB_set_pin_level(0, 1);				//�ϵ�����
	PORTC_set_pin_level(3, 0);
	
	PORTC_set_pin_level(0, 0);
	_delay_ms(50);
	for(i=0;i<step;i++){					//��תstep��
		motor_pwm();
	}
//	PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
//	PORTC_set_pin_level(3, 1);
}

void run_ccw_step2()
{
	led1_off;

	PORTC_set_pin_level(3, 0);
	
	PORTC_set_pin_level(0, 1);
	_delay_ms(50);
	PORTB_set_pin_level(1, 1);
	_delay_us(1000000/1000);
	PORTB_set_pin_level(1, 0);
	_delay_us(1000000/1000);
	
//	PORTC_set_pin_level(3, 1);
}

void run_cw_step2()
{
	if(motor2WaitTop()){
//		PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
		PORTC_set_pin_level(3, 1);
		return 1;
	}	

	PORTC_set_pin_level(3, 0);
	
	PORTC_set_pin_level(0, 0);
	_delay_ms(50);
	PORTB_set_pin_level(1, 1);
	_delay_us(1000000/800);
	PORTB_set_pin_level(1, 0);
	_delay_us(1000000/800);
	
//	PORTC_set_pin_level(3, 1);
}

void run_ccw_end()
{
	if(motor2WaitTop()){
//		PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
		PORTC_set_pin_level(3, 1);
		_delay_ms(50);
		return 1;
	}
	
//	PORTB_set_pin_level(0, 1);				//�ϵ�����
	PORTC_set_pin_level(3, 0);
	
	PORTC_set_pin_level(0, 0);
	_delay_ms(50);
	while(!motor2WaitTop()){
		PORTB_set_pin_level(1, 1);
		_delay_us(1000000/500);
		PORTB_set_pin_level(1, 0);
		_delay_us(1000000/500);
	}
//	PORTB_set_pin_level(0, 0);				//�ϵ�ֹͣ
//	PORTC_set_pin_level(3, 1);
}


void motorKeepRun(void)    // ���� ���� ������ת��ת
{
	if(motorSteps2Run > 128){
		run_ccw_step(motorSteps2Run-128);
		
	}else if(motorSteps2Run != 0){
		run_cw_step(motorSteps2Run);
	}
	motorSteps2Run = 0;
	
	if(agingMode){
		if(agingTimesCnt < AGING_TIMES_MAX){
			run_cw_step(28);
			_delay_ms(1000);
			run_ccw_end();
			_delay_ms(1000);
			agingTimesCnt++;
			
			int num;
			num = 10000*agingTimesCnt;
			SendString("ѭ���ϻ�������");
			SendNumb(agingTimesCnt, 6);CT();
			
		}else{
			agingTimesCnt = 0;
			agingMode = false;
		}
	}
}

void motor_reset()
{
	SendString("IRIS Reset!");CT();
	
	
}
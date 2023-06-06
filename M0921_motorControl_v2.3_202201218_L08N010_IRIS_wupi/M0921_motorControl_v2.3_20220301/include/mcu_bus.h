/*
 * app_mcu_bus.h
 *
 * Created: 2019/2/13 14:39:58
 *  Author: blion.yuan
 */ 


#ifndef APP_MCU_BUS_H_
#define APP_MCU_BUS_H_

#define MCU_BUS_DEV_WR_ADDR	 (I2C_SLAVE_ADDR)						//write address
#define MCU_BUS_DEV_RD_ADDR	 (MCU_BUS_DEV_WR_ADDR + 1)	//read daaress

//�Ĵ������ã�ʹ�ܼĴ���(1)��  �����üĴ���(0)

#define MCUBUS_COMM_CTL_MODE_AUTO	(0)
#define MCUBUS_COMM_CTL_MODE_HANDLE	(1)

//register address(command)
enum REG {
	REG_PATTERN = 0x0,	//����pattern
	REG_SPLASH,			//����splash
	REG_EXTERNL,		//�ⲿͨ��
	REG_POWER,			//dlp��Դ
	REG_MCUVER=0x07,       //���汾
	REG_MOTORFONT=0x2d,	//���ǰ��
	REG_MOTORSTATUS=0x21,//���״̬�������򵽵�
	REG_MOTORBACK=0xdb,	//�����ת
	REG_MAX,			//�߽�
	
};

#define MCUBUS_REG_NULL				(0xff) //register command: empty command

//status register enum
typedef enum _REG_STATUS
{
	eREG_STATUS_FREE=0, //empty status
	eREG_STATUS_M_WR_BUSY=0x01,	//master is writing
	eREG_STATUS_M_RD_BUSY=0x02, //master is reading
	eREG_STATUS_S_WR_BUSY=0x04, //slave is writing
	eREG_STATUS_S_RD_BUSY=0x08, //slave is reading
	eREG_STATUS_ERROR=0x10, //fault
}REG_STATUS_e;

//control mode register enum
typedef enum _REG_CTRL_MODE
{
	eREG_CTRL_AUTO=0, // auto check table control
}REG_CTRL_MODE_e;

//long data buffer memory structure
typedef struct _MB_REG_COMM_BUF
{
	uint8_t len;
	uint8_t data[16]; 
}MB_REG_COMM_BUF_t;

//register topology structure
typedef struct _MCUBUS_REG_AREA
{
	uint8_t motor1_forward_run;			//������ǰת
	uint8_t motor1_backward_run;		//�������ת
	uint8_t motor1_stop_run;		//ͣת
	uint8_t motor2_run_half;        //תһ��
	uint8_t motor2_run_end;        //ת����
	uint8_t dev_status;
	volatile int keep_alive;
	uint8_t mcu_ver[1];   //�汾��
	uint8_t motor_status[1];
	uint8_t calbrationPPS[6];
}MCUBUS_REG_AREA_t;

typedef struct _MCUBUS_REG_CTRL
{
	uint8_t cur_cmd; // current command	
	uint8_t *p_start_addr; //the start address of point
	uint8_t *p_end_addr; //the end address of point
}MCUBUS_REG_CTRL_t;

extern MCUBUS_REG_AREA_t g_t_reg;

typedef void (* MB_CALLBACK_PTR)(uint8_t cb_cmd, MB_REG_COMM_BUF_t *cmd_buf);  //�ϲ�ص���������

void mcubus_init();


#endif /* APP_MCU_BUS_H_ */
/*
 * app_mcu_bus.c
 *
 * Created: 2019/2/13 14:39:16
 *  Author: blion.yuan
 */ 
#include <driver_init.h>
#include <string.h>
#include <stdbool.h>

#include "mcu_bus.h"
#include "i2c_slave.h"
#include "motorContrl.h"

static MCUBUS_REG_CTRL_t m_reg_ctrl; //寄存器控制结构变量，包含当前寄存器命令，寄存器区起始，结束地址

static MCUBUS_REG_AREA_t *m_pt_regs = NULL; //定义i2c寄存器区变量指针，保存上层传来的地址

static MB_CALLBACK_PTR m_p_mb_call_app =NULL; //mcubus回调函数指针，向上层应用层传输数据
static MB_REG_COMM_BUF_t cmd_buf;
extern volatile uint16_t calbrationPPS[4],allSteps;

//Master write register callback function
static void mcubus_master_wr_reg_cb(uint8_t wr_val)
{
	if(m_pt_regs == NULL)
	{
		m_pt_regs->dev_status |= eREG_STATUS_ERROR;	// status fault
		return;
	}
	if((m_reg_ctrl.p_start_addr != NULL)&&(m_reg_ctrl.p_end_addr != NULL))
	{//start write register		
		m_pt_regs->dev_status |= eREG_STATUS_M_WR_BUSY; //flag is busy
		cmd_buf.data[cmd_buf.len++] = wr_val;
	}
	else
	{//it is not the current position of register point,judge register command first
	//	m_reg_ctrl.p_end_addr = (uint8_t *)m_pt_regs + sizeof(MCUBUS_REG_AREA_t);  //point to the end of register
		m_reg_ctrl.cur_cmd = wr_val; //save current register address(command) 
		cmd_buf.len = 0;
		if(wr_val == REG_MCUVER){
			m_reg_ctrl.p_start_addr = (uint8_t*)(m_pt_regs->mcu_ver); //point to the register to operate
			m_reg_ctrl.p_end_addr = m_reg_ctrl.p_start_addr + +sizeof(m_pt_regs->mcu_ver);//address end
		} else if (wr_val == 0x5c) {  //iic 设置pps
			m_reg_ctrl.p_start_addr = (uint8_t*)((m_pt_regs+wr_val)); //point to the register to operate
			m_reg_ctrl.p_end_addr = m_reg_ctrl.p_start_addr + 1;//address end
		}else if (wr_val == 0x5d)  //iic 读取 pps
		{
			volatile uint16_t calbrationPPS0=0,calbrationPPS1=0;
			calbrationPPS0 = (840*950.0)/calbrationPPS[0];
			calbrationPPS1 = (840*950.0)/calbrationPPS[1];
			
			m_pt_regs->calbrationPPS[0] = calbrationPPS0;
			m_pt_regs->calbrationPPS[1] = calbrationPPS0 >> 8;
			m_pt_regs->calbrationPPS[2] = calbrationPPS1;
			m_pt_regs->calbrationPPS[3] = calbrationPPS1 >> 8;
			m_pt_regs->calbrationPPS[4] = allSteps;
			m_pt_regs->calbrationPPS[5] = allSteps >> 8;
			
			m_reg_ctrl.p_start_addr = (uint8_t*)((m_pt_regs->calbrationPPS)); //point to the register to operate
			m_reg_ctrl.p_end_addr = m_reg_ctrl.p_start_addr + sizeof(m_pt_regs->calbrationPPS);//address end			
		} else if (wr_val == REG_MOTORSTATUS) {
			m_pt_regs->motor_status[0] = readPIstatus();
			
			m_reg_ctrl.p_start_addr = (uint8_t*)(m_pt_regs->motor_status); //point to the register to operate
			m_reg_ctrl.p_end_addr = m_reg_ctrl.p_start_addr + +sizeof(m_pt_regs->motor_status);//address end
		} else if (wr_val < REG_MAX) {
			m_reg_ctrl.p_start_addr = (uint8_t*)((m_pt_regs+wr_val)); //point to the register to operate
			m_reg_ctrl.p_end_addr = m_reg_ctrl.p_start_addr + 1;//address end
		}else {
			m_reg_ctrl.p_start_addr = NULL;
			m_reg_ctrl.p_end_addr = NULL;
			m_reg_ctrl.cur_cmd = MCUBUS_REG_NULL;
			m_pt_regs->dev_status |= eREG_STATUS_ERROR; //register overflow the	range
		}
	}
}

//Master read register callback function
static uint8_t mcubus_master_rd_reg_cb(void)
{
	volatile uint8_t retval = 0;
	if(m_pt_regs == NULL)
	{
		return 0;
	}
	if((m_reg_ctrl.p_start_addr != NULL)&&(m_reg_ctrl.p_end_addr != NULL))
	{//start to write register value		
		m_pt_regs->dev_status |= eREG_STATUS_M_RD_BUSY; //set the flag busy
		if(m_reg_ctrl.p_start_addr < m_reg_ctrl.p_end_addr)
		{
			retval = *(m_reg_ctrl.p_start_addr); //save the data to the corresponding register area
			m_reg_ctrl.p_start_addr ++;      //move the point,prepare for next 		
		}		
	}	
	return retval;
}

//Master read slave register callback function
static void mcubus_read_handler(void)
{
	// Master read reg
	
	if(NULL != m_pt_regs)
	{
		
		//if (m_reg_ctrl.cur_cmd == REG_MCUVER)
		//{
			//I2C_0_write(m_pt_regs->mcu_ver[0]);
		//}else{		
			//for(int i=0;i < 6;i++){
				//__delay_ms(1);
				//I2C_0_write(m_pt_regs->calbrationPPS[i]);
			//}
		//}
		while(m_reg_ctrl.p_start_addr < m_reg_ctrl.p_end_addr){
			__delay_ms(1);
			I2C_0_write(mcubus_master_rd_reg_cb());//salve->master write operation		
		}
	}	
}

//Master write slave register callback function
static void mcubus_write_handler(void)
{
	// Master write reg
	if(NULL != m_pt_regs)
	{
		__delay_ms(1);
		mcubus_master_wr_reg_cb( I2C_0_read());
		I2C_0_send_ack(); // or send_nack() if we don't want to receive more data		
	}
}
//i2c slave address receive callback function
static void mcubus_address_handler(void)
{
	volatile uint8_t tmp_dev=0;
	if(m_pt_regs != NULL)
	{
		if( (m_pt_regs->dev_status & eREG_STATUS_ERROR) == eREG_STATUS_ERROR)
		{
			m_pt_regs->dev_status &= ~eREG_STATUS_ERROR; //给出错误
			//m_reg_ctrl.p_start_addr = NULL;
			//m_reg_ctrl.p_end_addr = NULL;
		}	
		tmp_dev = I2C_0_read();
		if( (tmp_dev == MCU_BUS_DEV_WR_ADDR)||(tmp_dev == MCU_BUS_DEV_RD_ADDR) )//write slave address
		{
			I2C_0_send_ack(); // master is calling me
			if(tmp_dev == MCU_BUS_DEV_WR_ADDR) //
			{
				m_reg_ctrl.cur_cmd = MCUBUS_REG_NULL; //clean current command
				m_reg_ctrl.p_start_addr = NULL;
				m_reg_ctrl.p_end_addr = NULL;
			}		
		}
		else
		{
			//message does not for me
			m_reg_ctrl.p_start_addr = NULL;
			m_reg_ctrl.p_end_addr = NULL;
			I2C_0_send_nack();
		}
	}
}


static void mcubus_stop_handler(void)
{
	if(m_pt_regs != NULL)
	{
		//m_reg_ctrl.p_start_addr = NULL; //set null to current operate register
		//m_reg_ctrl.p_end_addr = NULL; //set null to current operate register	

		//当前命令执行完毕，上抛命令
		if( (NULL != m_p_mb_call_app  )&&( MCUBUS_REG_NULL != m_reg_ctrl.cur_cmd) ) {
			m_p_mb_call_app(m_reg_ctrl.cur_cmd, &cmd_buf); //上抛给应用层前命令
		}
		
		m_reg_ctrl.cur_cmd = MCUBUS_REG_NULL; //clean register address command
		m_pt_regs->dev_status &= (~eREG_STATUS_M_WR_BUSY); //clean the busy flag
		m_pt_regs->dev_status &= (~eREG_STATUS_M_RD_BUSY); //clean the busy flag
	}
}


static void mcubus_error_handler(void)
{
	if(m_pt_regs != NULL)
	{
		m_pt_regs->dev_status |= eREG_STATUS_ERROR; //give the error
		m_reg_ctrl.p_start_addr = NULL; //set null to current operate register
		m_reg_ctrl.p_end_addr = NULL; //set null to current operate register
		
	}
	I2C_0_close();
//	MCU_BUS_UNLINK_SOC;
	I2C_0_open(); //abnormal close and restart the hardware i2c

}

static bool init(MCUBUS_REG_AREA_t * p_reg, MB_CALLBACK_PTR p_appfunc)
{
	I2C_0_init();
	m_pt_regs = p_reg;
	if(m_pt_regs == NULL)
	{
		return false;
	}
	else
	{
		m_pt_regs->dev_status = eREG_STATUS_FREE;//set status as empty
	}
	if (NULL != p_appfunc) {
		m_p_mb_call_app = p_appfunc;
	}
	
	I2C_0_set_read_callback(mcubus_read_handler);
	I2C_0_set_write_callback(mcubus_write_handler);
	I2C_0_set_address_callback(mcubus_address_handler);
	I2C_0_set_stop_callback(mcubus_stop_handler);
	I2C_0_set_collision_callback(mcubus_error_handler);
	I2C_0_set_bus_error_callback(mcubus_error_handler);
	
	I2C_0_open();

	return true;
}

//static void set_pattern(uint8_t *type)
//{
	//char t = *type;
	//switch (t) {
		//case 'R':
		//pattern_red();
		//break;
		//case 'G':
		//pattern_green();
		//break;
		//case 'B':
		//pattern_blue();	
		//break;
		//case 'W':
		//pattern_white();
		//break;
		//case 'H':
		//pattern_black();
		//break;
	//}
//}

//static void set_splash(uint8_t *index)
//{
	//uint8_t _index = *index;
	//switch (_index) {
		//case 0:
		//splash_0();
		//break;
		//case 1:
		//splash_1();
		//break;
		//case 2:
		//splash_2();
		//break;
		//case 3:
		//splash_3();
		//break;
	//}
//}
//
//static void set_external()
//{
	//source_external();
//}

//static void set_power(uint8_t *power)
//{
	//bool _power = *power;
	//if (_power) {
		//power_on();
	//} else {
		//power_off();
	//}
//}

MCUBUS_REG_AREA_t g_t_reg;
static void mcubus_cmd_cb(uint8_t cmdval, MB_REG_COMM_BUF_t *cmd_buf)
{
//解析composer 的数据

	uint8_t tempPPS[9] = {0};
	uint8_t tmp, i;
	
	uint8_t step;

	switch (cmdval) {
		case 0x5c:
			
			
			if(cmd_buf->len != 0){
				calbrationPPS[0] = (840*950.0)/(cmd_buf->data[0] + cmd_buf->data[1]*16*16);
				calbrationPPS[1] = (840*950.0)/(cmd_buf->data[2] + cmd_buf->data[3]*16*16);
				calbrationPPS[2] = cmd_buf->data[4] + cmd_buf->data[5]*16*16;
				
				tempPPS[0] = calbrationPPS[0] >> 8;
				tempPPS[1] = calbrationPPS[0];

				tempPPS[2] = calbrationPPS[1] >> 8;
				tempPPS[3] = calbrationPPS[1];

				tempPPS[4] = calbrationPPS[2] >> 8;
				tempPPS[5] = calbrationPPS[2];

				tempPPS[6] = calbrationPPS[3] >> 8;
				tempPPS[7] = calbrationPPS[3];
				
				tempPPS[6] = allSteps >> 8;
				tempPPS[7] = allSteps;
				
				tmp = 0;
				for(i=0;i<6;i++)
				{
					tmp ^= tempPPS[i];
				}
				tempPPS[8] = tmp;			//最后增加一字节的校验
				config_write_eeprom(tempPPS,sizeof(tempPPS));
			}
		break;
		case REG_MOTORFONT:
			step = cmd_buf->data[0];
			if (step<129) {
				motorRunFont(step);
			}
		break;
		case REG_MOTORBACK:
			step = cmd_buf->data[0];
			if (step<129) {
				motorRunBack(step);
			}
		break;
	}
	
	

}

void mcubus_init()
{
	////初始化版本
	uint8_t ver[16] = {"V1.3_20210526"};			//程序名：G10N000_p33_1080p_motorControl_V1.3_20210526
	memcpy(g_t_reg.mcu_ver,ver,1);
	
	init(&g_t_reg, mcubus_cmd_cb);
	
}

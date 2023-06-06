#include "utils.h"
#include "IR.h"
#include "motorContrl.h"
#include "math.h"
#include "delay.h"

#define K2_V		3.00
#define K2_CHANNAL	7
#define K3_V		0.62
#define K3_CHANNAL	7
#define K4_V		0.91
#define K4_CHANNAL	7
#define K5_V		1.24
#define K5_CHANNAL	7
#define K6_V		1.48
#define K6_CHANNAL	7
#define K7_V		1.85
#define K7_CHANNAL	7
#define K8_V		2.14
#define K8_CHANNAL	7
#define K9_V		2.48
#define K9_CHANNAL	7
#define K10_V		2.73
#define K10_CHANNAL	7
//ADC0  PA7  通道为7
#define KEY_DELTA	0.05

static bool get_key_press(uint8_t channal, float value)
{
	float v, val_abs;
	v = (float)ADC_0_get_conversion(channal)*3.3/255;  // 8位除255 10位除1023
	
// 	int num;							//按键采样电压串口打印输出
// 	num = 100*v;
// 	SendNumb(num, 3);CT();
	
	val_abs = fabs(v-value);

	if (val_abs < KEY_DELTA) {
		return true;
	} else {
		return false;
	}
}

static bool get_k2_press(void)
{
	return get_key_press(K2_CHANNAL, K2_V);
}

static bool get_k3_press(void)
{
	return get_key_press(K3_CHANNAL, K3_V);
}

static bool get_k4_press(void)
{
	return get_key_press(K4_CHANNAL, K4_V);
}

static bool get_k5_press(void)
{
	return get_key_press(K5_CHANNAL, K5_V);
}

static bool get_k6_press(void)
{
	return get_key_press(K6_CHANNAL, K6_V);
}
static bool get_k7_press(void)
{
	return get_key_press(K7_CHANNAL, K7_V);
}

static bool get_k8_press(void)
{
	return get_key_press(K8_CHANNAL, K8_V);
}

static bool get_k9_press(void)
{
	return get_key_press(K9_CHANNAL, K9_V);
}

static bool get_k10_press(void)
{
	return get_key_press(K10_CHANNAL, K10_V);	
}

//add IR keys
static bool get_ir_up_press(void)  //up
{
	return ir_key_up();
}

static bool get_ir_down_press(void) //down
{
	return ir_key_down();
}

static bool get_ir_add_press(void)  //add
{
	return ir_key_add();
}

static bool get_ir_sub_press(void)  //sub
{
	return ir_key_sub();
}

static bool get_ir_rright_press(void)  //rright
{
	return ir_key_rright();
}

static bool get_ir_mute_press(void)  //mute
{
	return is_key_mute();
}

static bool get_ir_onoff_press(void)  //onoff
{
	return ir_key_onoff();
}

void set_aprerture(bool onoff)
{
	if (onoff) {
		PORTB_set_pin_level(3, 1);
		PORTB_set_pin_level(4, 0);
		} else {
		PORTB_set_pin_level(3, 0);
		PORTB_set_pin_level(4, 1);
	}
}

static void aperture_onoff(eKeyType type)
{
	static bool flag = true;
	flag = !flag;
	set_aprerture(flag);
}

void add_key_event(void)
{
    key_add(get_k2_press, run_ccw_end, eWobble, NULL);			//反转回PI
	key_add(get_k5_press, motorRunFont1, eWobble, NULL);		//正转1步
	key_add(get_k6_press, motorRunBack1, eWobble, NULL);		//反转1步
	key_add(get_k8_press, motorRunFont19, eWobble, NULL);		//正转19步
	key_add(get_k9_press, motorRunBack19, eWobble, NULL);		//反转19步
	key_add(get_k10_press, motorKeepRun, eWobble, NULL);	    //循环老化
	key_add(get_k7_press, aperture_onoff, eWobble, NULL);
	/*
	key_add(get_k3_press, motorRunBack13, eWobble, NULL);		//反转13步
	key_add(get_k7_press, motorRunFont30, eWobble, NULL);		//正转30步	
	key_add(get_k10_press, motorRunFont16, eWobble, NULL);		//正转17步
	
	
	key_add(get_k5_press, motorRunBack1, eWobble, NULL);		//反转1步
	key_add(get_k6_press, motorRunFont1, eWobble, NULL);		//正转1步
	
	key_add(get_k8_press, run_cw_step2, eWobble, NULL);			//反转2拍
	key_add(get_k9_press, run_ccw_step2, eWobble, NULL);		//正转2拍	
	*/
	
}



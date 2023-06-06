/*
 * IR.h
 *
 * Created: 2019/7/31 22:12:08
 *  Author: Ade.tang
 */ 


#ifndef IR_H_
#define IR_H_
#include <compiler.h>

bool is_key_mute();	//æ≤“Ù
bool ir_key_up();
bool ir_key_down();
bool ir_key_left();
bool ir_key_right();
bool ir_key_rright();
bool ir_key_lleft();
bool ir_key_ok();
bool ir_key_return();
bool ir_key_menu();
bool ir_key_add();
bool ir_key_sub();
bool ir_key_stop();
bool ir_key_menur();

TCB_init();

#endif /* IR_H_ */
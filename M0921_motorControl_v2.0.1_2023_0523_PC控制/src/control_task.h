/*
 * control_task.h
 *
 * Created: 2018/5/2 9:39:34
 *  Author: TF
 */ 


#ifndef CONTROL_TASK_H_
#define CONTROL_TASK_H_
#include "utils.h"
typedef struct  task_components
{
	uint8_t run_flag;//任务运行标志
	uint16_t timer;//计时器
	uint16_t itv_time;//任务间隔时间
	void (* task_hook)(void);//任务函数指针
	struct list_head qset;
}task_componets_t;

void task_init(void);
void task_add(void (*task_hook)(void), uint16_t itv_time);
void task_process(void);
void task_remarks(void);

#endif /* CONTROL_TASK_H_ */
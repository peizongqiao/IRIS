 
#include "control_task.h"

static struct list_head task_head;	//任务链表头

void task_init(void)
{
	init_list_head(&task_head);
}

void task_add(void (*task_hook)(void), uint16_t itv_time)
{
	struct task_components *task;
	task = malloc(sizeof(struct task_components));
	task->run_flag = 0;
	task->itv_time = itv_time/3;
	task->timer = itv_time;
	task->task_hook = task_hook;
	list_add_tail(&task->qset, &task_head);
}

void task_remarks(void)
{
	struct task_components *task;
	
	list_for_each_entry(task, &task_head, qset) {
		if (task->timer) {
			task->timer--;
			if (task->timer == 0) {
				task->timer = task->itv_time;
				task->run_flag = true;
			}
		}
	}
}

void task_process(void)
{
	struct task_components *task;
	uint8_t tmp = 0;

	if(SREG & 0x80 ) {
		tmp = 1;
		SREG &= ~0x80;
	}
	list_for_each_entry(task, &task_head, qset) {
		if (task->run_flag == true) {
			task->run_flag = false;
			task->task_hook();
		}
	}
	if (tmp) {
		SREG |= 0X80;
	}
}
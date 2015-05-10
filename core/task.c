/********************************************************
 * Filename: core/task.c
 * 
 * Author: parkjy, RTOSLab. SNU.
 * 
 * Description: task management.
 ********************************************************/
#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3

/*
 * Queue (list) of tasks that are ready to run.
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/*
 * Pointer to TCB of running task
 */
static eos_tcb_t *_os_current_task;
eos_tcb_t readyqueue[3] ;

static int32u_t create_task_initial = 1;
static int32u_t task_count = 0;

int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t _priority) {
	PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, _priority);

	addr_t context_top_ptr = 
	_os_create_context(sblock_start, sblock_size, entry, arg);
		
	task->stack_ptr = context_top_ptr;
	task->priority = _priority;
	task->state = WAITING;

	*(readyqueue + task_count) = *task;
	readyqueue[task_count].stack_ptr = task->stack_ptr;
	readyqueue[task_count].priority = task->priority;
	
	readyqueue[task_count].state = task->state;
	task_count++;
	return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task) {
}

static int32u_t eos_schedule_initial = 1;

void eos_schedule() {

	if(!eos_schedule_initial) {
		addr_t saved_context_stack_top_ptr = _os_save_context();
		if(saved_context_stack_top_ptr != NULL) {
			if(readyqueue[1].state == RUNNING) {	
				readyqueue[1].stack_ptr = saved_context_stack_top_ptr;
				readyqueue[1].state = WAITING;
				readyqueue[2].state = RUNNING;
				_os_restore_context( readyqueue[2].stack_ptr );   
			} else if(readyqueue[2].state == RUNNING) {
				readyqueue[2].stack_ptr = saved_context_stack_top_ptr;
				readyqueue[2].state = WAITING;
				readyqueue[1].state = RUNNING;
				_os_restore_context( readyqueue[1].stack_ptr );   
			}
		} else {
			return;
		}
	}

	eos_schedule_initial = 0;
	readyqueue[1].state = RUNNING;
	// No error before, eos_schedule+233
	_os_restore_context(readyqueue[1].stack_ptr);
}

eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}

void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
}

int32u_t eos_get_priority(eos_tcb_t *task) {
}

void eos_set_period(eos_tcb_t *task, int32u_t period){
	task->period = period;
}

int32u_t eos_get_period(eos_tcb_t *task) {
}

int32u_t eos_suspend_task(eos_tcb_t *task) {
}

int32u_t eos_resume_task(eos_tcb_t *task) {
}

void eos_sleep(int32u_t tick) {
}

void _os_init_task() {
	PRINT("initializing task module.\n");

	/* init current_task */
	_os_current_task = NULL;

	/* init multi-level ready_queue */
	int32u_t i;
	for (i = 0; i < LOWEST_PRIORITY; i++) {
		_os_ready_queue[i] = NULL;
	}
}

void _os_wait(_os_node_t **wait_queue) {
}

void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
}

void _os_wakeup_sleeping_task(void *arg) {
}

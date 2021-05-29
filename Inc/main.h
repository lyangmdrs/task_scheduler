/*
 * main.h
 *
 *  Created on: 1/May/2021
 *      Author: Lyang Medeiros
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

/* Definitions to stack division */

#define SIZE_TASK_STACK 	1024U
#define SIZE_SCHED_STACK 	1024U

#define SRAM_START			0x20000000U
#define SRAM_SIZE           (128 * 1024)
#define SRAM_END			(SRAM_START + SRAM_SIZE)

#define T1_STACK_START 		SRAM_END
#define T2_STACK_START		(SRAM_END - SIZE_TASK_STACK)
#define T3_STACK_START		(SRAM_END - 2 * SIZE_TASK_STACK)
#define T4_STACK_START		(SRAM_END - 3 * SIZE_TASK_STACK)
#define IDLE_STACK_START    (SRAM_END - 4 * SIZE_TASK_STACK)
#define SCHED_STACK_START	(SRAM_END - 5 * SIZE_TASK_STACK)

/* End of definitions to stack division */

//-----------------------------------------------------------------------------//

/* Definitions to SysTick settings */

#define HSI_CLOCK	0XF42400U // = 16*10^6
#define TICK_HZ 	1000U

/* End of definitions to SysTick settings */

//-----------------------------------------------------------------------------//

/* Constants Definitions */

#define MAX_TASKS 	5
#define DUMMY_XPSR 	0x01000000U
#define DUMMY_LR	0xFFFFFFFDU
#define TASK_READY_STATE 0x00
#define TASK_BLOCKED_STATE 0xFF
/* Constants Definitions */

//-----------------------------------------------------------------------------//

void enable_processors_fauts(void);
void init_systick_timer(uint32_t tick_hz);

__attribute__((naked))
void init_scheduler_stack(uint32_t stack_start_address);
__attribute__((naked))
void switch_sp_to_psp(void);

uint32_t get_psp_value(void);
void save_psp_value (uint32_t stack_addr);
void update_next_task(void);

void init_task_stack (void);

void taskHandler1(void);
void taskHandler2(void);
void taskHandler3(void);
void taskHandler4(void);

void idle_task(void);
void schedule(void);
void task_delay(uint32_t tick_count);
void update_global_tick_count(void);
void unblock_tasks(void);
typedef struct
{
	uint32_t psp_value;
	uint32_t block_count;
	uint8_t current_state;
	void (*taskHandler)(void);
}TCB_t; // Task Control Block

TCB_t user_tasks[MAX_TASKS];

uint8_t current_task = 1;
uint32_t global_tick_count = 0;

#endif /* MAIN_H_ */

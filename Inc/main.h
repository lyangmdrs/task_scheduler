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
#define SCHED_STACK_START	(SRAM_END - 4 * SIZE_TASK_STACK)

/* End of definitions to stack division */

//-----------------------------------------------------------------------------//

/* Definitions to SysTick settings */

#define HSI_CLOCK	0XF42400U // = 16*10^6
#define TICK_HZ 	1000U

/* End of definitions to SysTick settings */

//-----------------------------------------------------------------------------//

/* Constants Definitions */

#define MAX_TASKS 4

/* Constants Definitions */

//-----------------------------------------------------------------------------//
void init_systick_timer(uint32_t tick_hz);


uint32_t psp_of_tasks[MAX_TASKS] =
{
	T1_STACK_START, T2_STACK_START, T3_STACK_START, T4_STACK_START
};


__attribute__((naked))
void init_scheduler_stack(uint32_t stack_start_address);

void init_task_stack (void)

void taskHandler1(void);
void taskHandler2(void);
void taskHandler3(void);
void taskHandler4(void);


#endif /* MAIN_H_ */

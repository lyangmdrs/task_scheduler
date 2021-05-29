/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "main.h"

int main(void)
{
    printf("Hello Embedded World!\n");

    enable_processors_fauts();

    init_scheduler_stack(SCHED_STACK_START);
    init_task_stack();
    init_systick_timer(TICK_HZ);

    switch_sp_to_psp();

    taskHandler1();

    for(;;);
}

void enable_processors_fauts(void)
{
	uint32_t *pSHCSR = (uint32_t *)0xE000ED24; // System Handler Control and State Register
	*pSHCSR |= (1 << 16); // Memory Management fault;
	*pSHCSR |= (1 << 17); // Bus Fault;
	*pSHCSR |= (1 << 18); // Usage Fault;
}

/*
 * This microcontroller family has a system timer, called SysTick. This timer is
 * configured thru 4 dedicated registers. Which are:
 * 			> SYST_CSR		- SysTick Control and Status Register	- 0xE000E010
 * 			> SYST_RVR		- SysTick Reload Value Register			- 0xE000E014
 * 			> SYST_CVR		- SysTick Current Value Register		- 0xE000E018
 * 			> SYST_CALIB	- SysTick Calibration Value Register	- 0xE000E01C
 * The SysTick counts down from the reload value to zero, reloads with the value
 * in SYST_RVR, then counts down again, repeating the whole process.
 */
void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSCSR = (uint32_t *) 0xE000E010;
	uint32_t *pSRVR = (uint32_t *) 0xE000E014;

	uint32_t count_value = (HSI_CLOCK / tick_hz) - 1;

	/* Reseting the value of SYST_RVR
	*		The bits from 24 to 31 are reserved in this register. So, we must clear
	*		only the bits from 0 to 23.
	*/
	*pSRVR &= ~(0x00FFFFFFFF);

	// Loading count_value to SYST_RVR
	*pSRVR |= count_value;

	/* The first three bit of SYST_CSR are used to configure the SysTick.
	 * 		> Bit 0: Enables the counter when it's value is 1;
	 * 		> Bit 1: Sets an exception to indicate when the counting reaches 0,
	 * 		         when it's value is 1;
	 * 		> Bit 2: Sets the clock source. If this bit is 0, the clock source
	 * 		         will be a external clock. If this bit is 1, the clock source
	 * 		         will be the processor's clock.
	 */

	// Setting the SysTick clock source to use the processor clock
	*pSCSR |= (1 << 2);

	// Enabling the SysTick exception request
	*pSCSR |= (1 << 1);

	// Enabling the counter
	*pSCSR |= (1 << 0);
}

__attribute__((naked))
void init_scheduler_stack(uint32_t stack_start_address)
{
	/* Changing the value of Main Stack Pointer (MSP)
	 *  - By default, SP points to MSP;
	 *	- Every time that the processor enters in thread mode, SP will
	 *	point to MSP;
	 *	- So, if MSP is holding the initial address of scheduler's stack,
	 *	SP will point by default to this address and if, for some reason,
	 *	SP starts to point to PSP, every time the processor enters in
	 *	thread mode, the SP will points to scheduler stack.
	 */
	__asm volatile("MSR MSP, R0"); // Moves the value from 1st argument on MSP
	__asm volatile("BX LR"); // Returns to caller
}

void init_task_stack (void)
{
	/*
	 * Initiating all tasks stack with dummy data
	 */

	uint32_t *pPSP;

	user_tasks[0].current_state = TASK_READY_STATE;
	user_tasks[0].psp_value = IDLE_STACK_START;
	user_tasks[0].taskHandler = idle_task;

	user_tasks[1].current_state = TASK_READY_STATE;
	user_tasks[1].psp_value = T1_STACK_START;
	user_tasks[1].taskHandler = taskHandler1;

	user_tasks[2].current_state = TASK_READY_STATE;
	user_tasks[2].psp_value = T2_STACK_START;
	user_tasks[2].taskHandler = taskHandler2;

	user_tasks[3].current_state = TASK_READY_STATE;
	user_tasks[3].psp_value = T3_STACK_START;
	user_tasks[3].taskHandler = taskHandler3;

	user_tasks[4].current_state = TASK_READY_STATE;
	user_tasks[4].psp_value = T4_STACK_START;
	user_tasks[4].taskHandler = taskHandler4;


	// The stack operation model for Cortex-M4 processors is Full
	// Descending. Thus, we need decrement the memory address before
	// pushing data.
	for (int i = 0; i < MAX_TASKS; i++)
	{
		pPSP = (uint32_t*)user_tasks[i].psp_value;

		pPSP = pPSP - 1;
		// Program Status Register (PSR)
		/*
		 * Given that the Cortex-M4 processor only supports execution of
		 * instructions in Thumb state, we must pay attention to always set
		 * this register with it's T bit as 1.
		 *
		 * View pages 17 and 20 of Cortex-M4 Devices - Generic User Guide
		 *
		 */
		*pPSP = DUMMY_XPSR;

		pPSP = pPSP - 1;
		// Program Counter (PC)
		*pPSP = (uint32_t)user_tasks[i].taskHandler;

		pPSP = pPSP - 1;
		// Link Register (LR)
		/*
		 * The context switching happens inside of a exception. So, at this point
		 * LR must be loaded with one of the EXC_RETURN values.
		 *
		 * View pages 41 of Cortex-M4 Devices - Generic User Guide
		 *
		 */
		*pPSP = DUMMY_LR;

		// Initiate the remaining registers with zero
		for (int k = 0; k < 13; k++)
		{
			pPSP = pPSP - 1;
			*pPSP = 0;
		}

		user_tasks[i].psp_value = (uint32_t)pPSP;
	}
}

uint32_t get_psp_value(void)
{
	return user_tasks[current_task].psp_value;
}

void save_psp_value (uint32_t current_psp_value)
{
	user_tasks[current_task].psp_value = current_psp_value;
}

void update_next_task(void)
{
	int state = TASK_BLOCKED_STATE;

	for(int i = 0; i < (MAX_TASKS); i++)
	{
		current_task = (current_task + 1) % MAX_TASKS;
		state = user_tasks[current_task].current_state;
		if((state == TASK_READY_STATE) && (current_task != 0)) break;
	}

	if(state != TASK_READY_STATE) current_task = 0;
}

__attribute__((naked))
void switch_sp_to_psp(void)
{
	// Initialize PSP

	__asm volatile ("PUSH {LR}");
	__asm volatile ("BL get_psp_value");
	__asm volatile ("MSR PSP, R0");
	__asm volatile ("POP {LR}");

	// Point SP to PSP
	__asm volatile ("MOV R0, #0x02");
	__asm volatile ("MSR CONTROL, R0");
	__asm volatile ("BX LR");
}

void taskHandler1(void)
{
	for(;;)
	{
		printf("This is task 1!\n");
		task_delay(100);
	}
}

void taskHandler2(void)
{
	for(;;)
	{
		printf("This is task 2!\n");
		task_delay(200);
	}
}

void taskHandler3(void)
{
	for(;;)
	{
		printf("This is task 3!\n");
		task_delay(300);
	}
}

void taskHandler4(void)
{
	for(;;)
	{
		printf("This is task 4!\n");
		task_delay(400);
	}
}

void idle_task(void)
{
	for(;;);
}

void schedule(void)
{
	uint32_t *pICSR = (uint32_t *)0xE000ED04;
	// Pend a exception on PendSV
	*pICSR |= (1 << 28);
}

void task_delay(uint32_t tick_count)
{
	// Disable Interrupts
	/*
	 * Since user_tasks is as global variable and we will be modifying it in
	 * this function, its important don't get interrupted here.
	 */

	INTERRUPT_DISABLE();

	if(current_task)
	{
		user_tasks[current_task].block_count = global_tick_count + tick_count;
		user_tasks[current_task].current_state = TASK_BLOCKED_STATE;
		schedule();
	}

	/*
	 * Now, we can enable the interruptions again.
	 */

	INTERRUPT_ENABLE();
}

void update_global_tick_count(void)
{
	global_tick_count = global_tick_count + 1;
}

void unblock_tasks(void)
{
	for(int i = 1; i < MAX_TASKS; i++)
	{
		if(user_tasks[i].current_state != TASK_READY_STATE)
		{
			if(user_tasks[i].block_count == global_tick_count)
			{
				user_tasks[i].current_state = TASK_READY_STATE;
			}
		}
	}
}

void SysTick_Handler(void)
{
	uint32_t *pICSR = (uint32_t *)0xE000ED04;

	update_global_tick_count();
	unblock_tasks();

	// Pend a exception on PendSV
	*pICSR |= (1 << 28);
}

__attribute__((naked))
void PendSV_Handler(void)
{
	// Get PSP value of the current running task
		__asm volatile ("MRS R0, PSP");
		// Save the context of current task
		// Store Multiple Register, decrement before (STMBD)
		__asm volatile ("STMDB R0!, {R4-R11}");
		// Save LR before using BL instruction
		__asm volatile ("PUSH {LR}");
		// Save the current value of PSP
		__asm volatile ("BL save_psp_value");
		// Load the next task
		__asm volatile ("BL update_next_task");
		// Get the new task PSP
		__asm volatile ("BL get_psp_value");
		// Retrieve the Stack Frame 2 to the new task
		__asm volatile ("LDMIA R0!, {R4-R11}");
		// Update PSP
		__asm volatile ("MSR PSP, R0");
		// Retrieve the saved LR value
		__asm volatile ("POP {LR}");
		// In a naked function we must load PC with the
		// address to the next function in order to exit
		__asm volatile ("BX LR");
}

void HardFault_Handler(void)
{
	printf("System Fault: %s!\n", __FUNCTION__);
	for(;;);
}

void MemManage_Handler(void)
{
	printf("System Fault: %s!\n", __FUNCTION__);
	for(;;);
}

void BusFault_Handler(void)
{
	printf("System Fault: %s!\n", __FUNCTION__);
	for(;;);
}

void UsageFault_Handler(void)
{
	printf("System Fault: %s!\n", __FUNCTION__);
	for(;;);
}

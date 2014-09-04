/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2007 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdint.h>

#include "LPC11Uxx.h"

volatile uint32_t msTicks = 0; // counter for 1ms SysTicks

//====================================================================================
int main()
{
	uint32_t timer_mark;

	// Enable GPIO Clock ( powers the GPIO peripheral )
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

	// Select GPIO Mode and disable analog mode, refer to User Manual - UM10524
	LPC_IOCON->PIO0_7 = 0;

	// Set the pin direction, set high for an output
	LPC_GPIO->DIR[0] |= 1<<7;

	// Init SysTick
	SysTick_Config(SystemCoreClock / 1000);				// Generate interrupt every 1 ms

	for(;;)
	{
		timer_mark = msTicks;					// Take timer snapshot 
		while(msTicks < (timer_mark + 100));	// Wait until 100ms has passed
		LPC_GPIO->NOT[0] = 1<<7;
	}
}

//====================================================================================
void SysTick_Handler(void)
{
	msTicks++;
}


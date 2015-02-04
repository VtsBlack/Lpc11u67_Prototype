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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Includes.h"

#include "cmsis_os.h"

/*
 * neededd for LPC open libs
 */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

extern void SystemCoreClockUpdateX (void);




const char inst1[] = "LPC11u6x UART example using ring buffers\r\n";
const char inst2[] = "Press a key to echo it back or ESC to quit\r\n";

/*
	Task Prototype
 */
void InitTask(const void *arg);

osThreadDef(InitTask, osPriorityNormal, 1, 0);
/**
 * [Init_UART_PinMux Sets 18 and 19 pins to uart mode]
 */
static void Init_UART_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_PULLUP));
}

/**
 * [SER_Init  Initalize Uart Driver]
 */
void SER_Init (void) {
	
	Init_UART_PinMux();
	
  Chip_UART0_Init(LPC_USART0);
	Chip_UART0_SetBaud(LPC_USART0, 115200);
	Chip_UART0_ConfigData(LPC_USART0, (UART0_LCR_WLEN8 | UART0_LCR_SBS_1BIT));
	Chip_UART0_SetupFIFOS(LPC_USART0, (UART0_FCR_FIFO_EN | UART0_FCR_TRG_LEV2));
	Chip_UART0_TXEnable(LPC_USART0);
	
	
	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	debug_uart_rb_init();

	/* Enable receive data and line status interrupt */
	Chip_UART0_IntEnable(LPC_USART0, (UART0_IER_RBRINT | UART0_IER_RLSINT));

	/* Enable UART 0 interrupt */
	NVIC_EnableIRQ(USART0_IRQn);

	/* Send initial messages */
	Debug(inst1);
	Debug(inst2);
}


/**
 * [main Program entry]
 * @return [description]
 */
int main(void)
{
	
	SystemCoreClockUpdateX();
	/* Thin line enables clock to peripherals */
	LPC_SYSCTL->SYSAHBCLKCTRL |= (1<<16);
	
	/* Run Init Task */
    osThreadCreate (osThread(InitTask), NULL);
    
    while (1) {
        osDelay(1000);
    }
}

/**
 * [InitTask description]
 */
void InitTask(const void *arg)
{
	
	int clockRate = Chip_Clock_GetMainClockRate();

	/* Enabled Clock To GPPIO */
	Chip_GPIO_Init(LPC_GPIO);
	// Set the pin direction, set high for an output LED2
	Chip_GPIO_SetPinDIR(LPC_GPIO, 0, 2, 1);
    Chip_GPIO_SetPinDIR(LPC_GPIO, 1, 20, 1);
    Chip_GPIO_WriteDirBit(LPC_GPIO, 1, 20, 1);
	/* Serial Driver Enable */
	SER_Init();

	Debug("Main clock is: %d\r\n", clockRate);
	
	for(;;)
	{
		osDelay(500);

		Debug("Sveiki\r\n");
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 2, 0);

		osDelay(1000);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 2, 1);

	}
}

/* End Of File */

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

#include "chip.h"

#include "RTL.h"


const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

__task void test(void);

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
//void SystemInit2 (void) {
//  volatile uint32_t i;

//  LPC_SYSCON->PDRUNCFG     &= ~(1 << 5);          /* Power-up System Osc      */
//  LPC_SYSCON->SYSOSCCTRL    = 0;
//  for (i = 0; i < 200; i++) __NOP();





//  /* System clock to the IOCON needs to be enabled or
//  most of the I/O related peripherals won't work. */
//  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<16);

//}


static void Init_UART_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

}

void SER_Init (void) {
	
	Init_UART_PinMux();
	
  Chip_UART0_Init(LPC_USART0);
	Chip_UART0_SetBaud(LPC_USART0, 115200);
	Chip_UART0_ConfigData(LPC_USART0, (UART0_LCR_WLEN8 | UART0_LCR_SBS_1BIT));
	Chip_UART0_SetupFIFOS(LPC_USART0, (UART0_FCR_FIFO_EN | UART0_FCR_TRG_LEV2));
	Chip_UART0_TXEnable(LPC_USART0);
}

//====================================================================================
int main()
{
	#define LED 2
	
	SystemCoreClockUpdate();

	// Enable GPIO Clock ( powers the GPIO peripheral )
	//LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

	// Select GPIO Mode and disable analog mode, refer to User Manual - UM10524
	//LPC_IOCON->PIO0_2 = 0;
	//LPC_IOCON->PIO1_20 = 0;
	
	Chip_GPIO_Init(LPC_GPIO);


	// Set the pin direction, set high for an output
	Chip_GPIO_SetPinDIR(LPC_GPIO, 0, 2, 1);

	SER_Init();
	
	os_sys_init(test);
}


__task void test(void)
{
	for(;;)
	{
		os_dly_wait(50);
		Chip_UART0_Send(LPC_USART0, "Sveiki", strlen("Sveiki"));
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 2, 0);

		os_dly_wait(100);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 2, 1);

	}
}


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

#include "chip.h"

#include "RTL.h"

/*
 * neededd for LPC open libs
 */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


const char inst1[] = "LPC11u6x UART example using ring buffers\r\n";
const char inst2[] = "Press a key to echo it back or ESC to quit\r\n";

char message[256] = {0};


/*
	Task Prototype
 */
__task void InitTask(void);


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
	
	uint8_t key;
	int bytes;
	
	Init_UART_PinMux();
	
  Chip_UART0_Init(LPC_USART0);
	Chip_UART0_SetBaud(LPC_USART0, 115200);
	Chip_UART0_ConfigData(LPC_USART0, (UART0_LCR_WLEN8 | UART0_LCR_SBS_1BIT));
	Chip_UART0_SetupFIFOS(LPC_USART0, (UART0_FCR_FIFO_EN | UART0_FCR_TRG_LEV2));
	Chip_UART0_TXEnable(LPC_USART0);
	
	
	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	/* Enable receive data and line status interrupt */
	Chip_UART0_IntEnable(LPC_USART0, (UART0_IER_RBRINT | UART0_IER_RLSINT));

	/* Enable UART 0 interrupt */
	NVIC_EnableIRQ(USART0_IRQn);

	/* Send initial messages */
	Chip_UART0_SendRB(LPC_USART0, &txring, inst1, sizeof(inst1) - 1);
	Chip_UART0_SendRB(LPC_USART0, &txring, inst2, sizeof(inst2) - 1);

	/* Poll the receive ring buffer for the ESC (ASCII 27) key */
	key = 0;
//	while (key != ' ') {
//		bytes = Chip_UART0_ReadRB(LPC_USART0, &rxring, &key, 1);
//		if (bytes > 0) {
//			/* Wrap value back around */
//			if (Chip_UART0_SendRB(LPC_USART0, &txring, (const uint8_t *) &key, 1) != 1) {
//			}
//		}
//	}
}


/**
 * @brief	UART interrupt handler using ring buffers
 * @return	Nothing
 */
void USART0_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART0_IRQRBHandler(LPC_USART0, &rxring, &txring);
}

/**
 * [main Program entry]
 * @return [description]
 */
int main(void)
{
	
	SystemCoreClockUpdate();
	/* Thin line enables clock to peripherals */
	LPC_SYSCTL->SYSAHBCLKCTRL |= (1<<16);
	
	/* Run Init Task */
	os_sys_init(InitTask);
}

/**
 * [InitTask description]
 */
__task void InitTask(void)
{
	
	int msg_len = 0;
	int clockRate = Chip_Clock_GetMainClockRate();

	/* Enabled Clock To GPPIO */
	Chip_GPIO_Init(LPC_GPIO);
	// Set the pin direction, set high for an output LED2
	Chip_GPIO_SetPinDIR(LPC_GPIO, 0, 2, 1);
	/* Serial Driver Enable */
	SER_Init();

	msg_len = snprintf(message, sizeof(message), "Main clock is: %d\r\n", clockRate);
	Chip_UART0_SendRB(LPC_USART0, &txring, message, msg_len);
	
	for(;;)
	{
		os_dly_wait(50);

		Chip_UART0_SendRB(LPC_USART0, &txring, "Sveiki\r\n", strlen("Sveiki\r\n"));
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 2, 0);

		os_dly_wait(100);
		Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 2, 1);

	}
}

/* End Of File */

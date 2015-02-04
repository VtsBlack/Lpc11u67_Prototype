#include "Includes.h"

#define LOGMAX   200
char message[LOGMAX];


/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


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

void debug_uart_rb_init(void)
{
    RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);
    
}


void Debug(const char *Str, ...)
{
    char *pStr;


    if (strchr(Str, '%') != NULL) {
        va_list ap;

        va_start(ap, Str);
        vsnprintf(message, LOGMAX-5, Str, ap);
        va_end(ap);

        pStr = message;
        message[LOGMAX-1] = '\0';
    }
    else {
        pStr = (char*)Str;
    }
    Chip_UART0_SendRB(LPC_USART0, &txring, pStr, strlen(pStr));
}

/**
 * [DebugHex description]
 * @param dat [description]
 * @param sz  [description]
 */
void DebugHex(const uint8_t *dat, int sz)
{
    int i;
    if (!sz) {
        return;
    }
    Debug("[");
    for (i = 0; i < sz; i++) {
        Debug(":%02X", dat[i]);
    }
    Debug("]\r\n");
}


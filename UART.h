#ifndef UART_H
#define UART_H

// #############################################################################
// ------------ SPI DEFINITIONS
// #############################################################################

#define UART_COMMAND_BUFFER_SIZE		2
#define UART_MAX_COMMAND_TX_SIZE		4
#define UART_MAX_COMMAND_RX_SIZE		1
#define UART_RX_BUFFER_SIZE				2
#define UART_TX_LENGTH_BYTE				0
#define UART_RX_LENGTH_BYTE				1
#define UART_LENGTH_BYTES				2

#define	MAX_MODEM_RECEIVE				4

#define MAX_UART_DATA_LENGTH			10

#define TX_ISR_FLAG						(1<<1)
#define RX_ISR_FLAG						(1<<0)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void UART_Initialize(uint8_t *pCanTXPacket);
void UART_Transmit(void);
void UART_Start_Command (void);
void UART_End_Command (void);
void Write_UART(uint8_t TX_Length, uint8_t RX_Length, uint8_t * Data2Write, uint8_t * * Data2Receive, bool Init);

#endif // UART_H

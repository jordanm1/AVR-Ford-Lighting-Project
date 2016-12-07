#ifndef UART_H
#define UART_H

// #############################################################################
// ------------ SPI DEFINITIONS
// #############################################################################

#define COMMAND_BUFFER_SIZE		11
#define MAX_COMMAND_TX_SIZE		6
#define MAX_COMMAND_RX_SIZE		4
#define TX_LENGTH_BYTE			0
#define RX_LENGTH_BYTE			1
#define LENGTH_BYTES			2

#define MAX_UART_DATA_LENGTH	10

#define SS_BIT_HI       (0x40)

// Redefine Pin Mappings
#define DDR_SPI         (DDRA)
#define MOSI            (PINA4)
#define MISO            (PINA2)
#define SCK             (PINA5)
#define SS              (PINA6)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void UART_Initialize(void);
void UART_Transmit(void);
void UART_Start_Command (void);
void UART_End_Command (void);
void Write_UART(uint8_t TX_Length, uint8_t RX_Length, uint8_t * Data2Write, uint8_t * * Data2Receive);

#endif // UART_H

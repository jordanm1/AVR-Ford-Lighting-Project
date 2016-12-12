#ifndef SPI_H
#define SPI_H

// #############################################################################
// ------------ SPI DEFINITIONS
// #############################################################################

#define SPI_MASTER      0
#define SPI_SLAVE       1

#define COMMAND_BUFFER_SIZE 14
#define MAX_COMMAND_TX_SIZE 6
#define MAX_COMMAND_RX_SIZE 1
#define TX_LENGTH_BYTE      0
#define RX_LENGTH_BYTE      1
#define LENGTH_BYTES        2

#define MAX_SPI_DATA_LENGTH 10

#define SS_BIT_HI       (0x40)

#define DDR_SPI         (DDRA)
#define MOSI            (PINA4)
#define MISO            (PINA2)
#define SCK             (PINA5)
#define SS              (PINA6)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void SPI_Initialize(void);
void SPI_Transmit(void);
void SPI_Start_Command (void);
void SPI_End_Command (void);
void Write_SPI(uint8_t TX_Length, uint8_t RX_Length, uint8_t * Data2Write, uint8_t * * Data2Receive);

#endif // SPI_H

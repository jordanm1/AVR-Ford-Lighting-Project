#ifndef SPI_H
#define SPI_H

// #############################################################################
// ------------ SPI DEFINITIONS
// #############################################################################

#define SPI_MASTER      0
#define SPI_SLAVE       1

#define SPI_DATA_LENGTH (5)
#define SS_BIT_HI       (0x40)

#define DDR_SPI         (DDRA)
#define MOSI            (PA4)
#define MISO            (PA2)
#define SCK             (PA5)
#define SS              (PA6)
#define SS_MASTER       (PA7)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_SPI_Module(uint8_t Master_Slave_Identifier);
void SPI_MasterTransmit(char *TX_Data);
char* Get_RX_Data(void);

#endif // SPI_H

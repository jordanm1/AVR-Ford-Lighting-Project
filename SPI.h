#ifndef SPI_H
#define SPI_H

// #############################################################################
// ------------ SPI DEFINITIONS
// #############################################################################

#define SPI_MASTER      0
#define SPI_SLAVE       1

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

void MS_SPI_Initialize(uint8_t * p_this_node_id, uint8_t * p_spi_data);
void SPI_Transmit(uint8_t Max_Data_Length_Expected);

#endif // SPI_H

#ifndef CAN_H
#define CAN_H

// #############################################################################
// ------------ CAN DEFINITIONS
// #############################################################################

// SPI Instructions
#define CAN_RESET               (0xc0) // 11000000
#define CAN_READ                (0x03) // 00000011
#define CAN_READ_BUFFER_ID      (0x90) // 10010000 (Configured for Buffer 0)   
#define CAN_READ_BUFFER_DATA    (0x92) // 10010010 (Configured for Buffer 0)
#define CAN_WRITE               (0x02) // 00000010
#define CAN_TX_LOAD_BUFFER_ID   (0x40) // 01000000
#define CAN_TX_LOAD_BUFFER_DATA (0x41) // 01000001
#define CAN_RTS                 (0x81) // 10000001
#define CAN_READ_STATUS         (0xA0) // 10100000
#define CAN_RX_STATUS           (0xB0) // 10110000
#define CAN_BIT_MODIFY          (0x05) // 00000101

// SPI Data Lengths
#define CAN_RESET_LENGTH               1
#define CAN_READ_LENGTH                2
#define CAN_READ_BUFFER_ID_LENGTH      1
#define CAN_READ_BUFFER_DATA_LENGTH    1
#define CAN_WRITE_LENGTH               3
#define CAN_TX_LOAD_BUFFER_ID_LENGTH   2
#define CAN_TX_LOAD_BUFFER_LENGTH      2
#define CAN_RTS_LENGTH                 1
#define CAN_READ_STATUS_LENGTH         1
#define CAN_RX_STATUS_LENGTH           1
#define CAN_BIT_MODIFY                 4

// Personal CAN Registers
#define RXFOSIDH 00000000
#define RXFOSIDL 00000001
#define BFPCTRL 00001100
#define TXRTSCTRL 00001101
#define CANSTAT 00001110
#define CANCTRL 00001111
TEC
REC
RXMO
#define CNF1 00101010
#define CNF2 00101001
#define CNF3 00101000
#define CANINTE 00101011
#define CANINTF 00101100
EFLG
#define TXB0CTRL 00110000
#define TXB0SIDH 00110001
#define TXB0SIDL 00110010
#define TXB0DLC 00110101
#define TXB0D0 00110110
#define TXB0D1 00110111
#define TXB0D2 00111000
#define TXB0D3 00111001
#define TXB0D4 00111010
#define TXB0D5 00111011
#define TXB0D6 00111100
#define TXB0D7 00111101


// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void MS_CAN_Initialize(uint8_t * p_spi_data);

#endif // SPI_H

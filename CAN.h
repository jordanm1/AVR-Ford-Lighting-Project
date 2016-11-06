#ifndef CAN_H
#define CAN_H

#include "MCP25625_defs.h"

// #############################################################################
// ------------ CAN DEFINITIONS
// #############################################################################

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

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

//void MS_CAN_Initialize(uint8_t * p_spi_data);

#endif // SPI_H

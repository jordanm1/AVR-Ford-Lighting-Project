#ifndef SPI_service_H
#define SPI_service_H

// #############################################################################
// ------------ POUND DEFINES
// #############################################################################

// Index at which TX and RX Lengths exist
#define TX_LNTH_INDX		0
#define RX_LNTH_INDX		1

// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################

typedef enum {SENDING_STATE, RECEIVING_STATE, NORMAL_STATE} SPI_State;

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_SPI_Service(void);
void Run_SPI_Service(uint32_t event_mask);

#endif // SPI_service_H

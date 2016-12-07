#ifndef UART_Service_H
#define UART_Service_H

// #############################################################################
// ------------ POUND DEFINES
// #############################################################################


// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################

typedef enum {NORMAL_STATE, SENDING_STATE, RECEIVING_STATE} UART_State_t;

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_UART_Service(void);
void Run_UART_Service(uint32_t event_mask);
UART_State_t Query_UART_State(void);

#endif // UART_service_H

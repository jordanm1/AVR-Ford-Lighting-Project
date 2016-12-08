#ifndef UART_Service_H
#define UART_Service_H

// #############################################################################
// ------------ POUND DEFINES
// #############################################################################


// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################

typedef enum {NORMAL_UART_STATE, SENDING_UART_STATE, RECEIVING_UART_STATE} UART_State_t;

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_UART_Service(void);
void Run_UART_Service(uint32_t event_mask);
UART_State_t Query_UART_State(void);

#endif // UART_service_H

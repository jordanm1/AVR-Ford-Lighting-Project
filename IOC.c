/*******************************************************************************
    File:
        IOC.c
 
    Notes:
        This file contains the IOC module. IOC can be detected on two pins namely,
        INT0 : PB6
        INT1 : PA3 

    External Functions Required:

    Public Functions:
        void Init_IOC_Module(void)
            
*******************************************************************************/

// #############################################################################
// ------------ INCLUDES
// #############################################################################

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Config file
#include "config.h"

// Framework
#include "framework.h"

// This module's header file
#include "IOC.h"

// Interrupts
#include <avr/interrupt.h>

#include "SPI.h"

#include "CAN.h"

#include "MCP25625defs.h"

#include "master_service.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################
static uint32_t counter = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_IOC_Module

    Parameters
        None

    Description
        Initializes the IOC module

****************************************************************************/
void Init_IOC_Module(void)
{
    // The I/O Clock has no reason to be halted currently, but if it is
    // i.e. when it is coming out of sleep mode, I/O clock requires to be
    // enabled.
         
    // Setting up PB6 as an input pin
    DDRB &= ~(1<<INT0_PIN);
         
    // Set External Interrupt Control Register A to detect toggles.
    EICRA &= ~(1<<ISC00);
    EICRA |= (1<<ISC01);
         
    // When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) 
    // is set (one), the external pin interrupt is enabled.
    EIMSK |= (1<<INT0);
         
    // Clear External Interrupt Flag
    EIFR |= (1<<INTF0);
}

uint32_t query_counter(void)
{
	return counter;
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

/****************************************************************************
    ISR
        IOC Interrupt Handlers: INT0_vect

    Parameters
        None

    Description
        Handles IOC specific interrupts

****************************************************************************/
ISR(INT0_vect)
{
	counter++;
    uint8_t byte1 = 0;
    uint8_t byte2 = 0;
    uint8_t byte3 = 0;
	uint8_t* Variable_List[3] = {&byte1, &byte2, &byte3};
    uint8_t* RX_Data[1];
    RX_Data[0] = &byte1;
	Post_Event(EVT_MASTER_NEW_CAN_MSG);
	CAN_Read_Message(Variable_List);
    CAN_Read(MCP_EFLG, RX_Data);
    uint8_t TX_Data[1] = {0};
    CAN_Bit_Modify(MCP_EFLG, (1<<6), TX_Data);
    CAN_Read(MCP_EFLG_TXEP, RX_Data);
    CAN_Read(MCP_EFLG_RXEP, RX_Data);
	CAN_Bit_Modify(MCP_CANINTF, 0xFF, TX_Data); 
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

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
static uint8_t Recv2 = 0;
uint8_t* RecvList[1];


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
    // Clear External Interrupt Flag
    // EIFR |= (1<<INTF0);
	RecvList[0] = &Recv2;
	CAN_Read(MCP_RXB0D0, RecvList);
	uint8_t TX_Data[1] = {0};
	CAN_Bit_Modify(MCP_CANINTF, 0xFF, TX_Data); 
	//CAN_Write(MCP_CANINTF, TX_Data);	
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

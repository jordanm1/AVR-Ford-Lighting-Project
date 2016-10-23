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

// Config file
#include "config.h"

// Framework
#include "framework.h"

// This module's header file
#include "IOC.h"

// Atomic Read/Write operations
#include <util/atomic.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################



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
   // We need to ensure no interrupts occur when accessing 16-bit registers
   // (Just for safety, no ISR should be able to access these registers anyways.)
   // Even though the C code is one line for accessing 16-bit registers,
   //    in ASM it will be done in two cycles.
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
	   // The I/O Clock has no reason to be halted currently, but if it is
	   // i.e. when it is coming out of sleep mode, I/O clock requires to be
	   // enabled.
	   
	   // Setting up PB6 as an input pin
	   DDRB &= ~(1<<INT0_PIN);
	   
	   // Set External Interrupt Control Register A to detect toggles.
	   	   
	   EICRA &= ~(1<<ISC01);
	   EICRA |= (1<<ISC00);
	   
	   // When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) 
	   // is set (one), the external pin interrupt is enabled.
	   
	   EIMSK |= (1<<INT0);
	   
	   // Clear External Interrupt Flag
	   
	   EIFR |= (1<<INTF0);
   }
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
	// Clear External Interrupt Flag
	
	EIFR |= (1<<INTF0);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
      Private Function
         

      Parameters
         

      Description
         

****************************************************************************/

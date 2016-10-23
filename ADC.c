/*******************************************************************************
      File:
      ADC.c
     
      Notes:
      This file contains the ADC module. This module initializes,
	  ADC10 : PB7
   
      External Functions Required:

      Public Functions:
	  void Init_ADC_Module(void)
        
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
#include "ADC.h"

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

static uint16_t Last_ADC_Value = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
      Public Function
         Init_ADC_Module

      Parameters
         None

      Description
         Initializes the ADC module

****************************************************************************/
void Init_ADC_Module(void)
{
   // We need to ensure no interrupts occur when accessing 16-bit registers
   // (Just for safety, no ISR should be able to access these registers anyways.)
   // Even though the C code is one line for accessing 16-bit registers,
   //    in ASM it will be done in two cycles.
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
	   // Select voltage reference as internal 2.56 V reference
	   ADMUX |= (1<<REFS1)|(1<<REFS0);
	   AMISCR &= ~(1<<AREFEN);
	   
	   // Set PB7 as A2D input PB7 = ADMUX LSB 1010
	   ADMUX |= (1<<MUX3)|(1<<MUX1);
	   ADMUX &= ~(1<<MUX2)&~(1<<MUX0);
	   
	   // Enable ADC Module and Interrupt after conversion
	   ADCSRA |= (1<<ADEN)|(1<<ADIE);
	   
	   // Set ADC clock pre-scalar to SysClock/2
	   ADCSRA &= ~(1<<ADPS2)&~(1<<ADPS1)&~(1<<ADPS0);
   }
}

/****************************************************************************
      Public Function
         Get_ADC_Value

      Parameters
         None

      Description
         Returns right justified result of the ADC module

****************************************************************************/

uint16_t Get_ADC_Result(void)
{
	return Last_ADC_Value;
}

/****************************************************************************
      Public Function
         Poll_ADC

      Parameters
         None

      Description
         Find ADC value at PB7

****************************************************************************/

void Poll_ADC(void)
{
	ADCSRA |= (1<<ADSC);
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

/****************************************************************************
      ISR
         ADC Interrupt Handlers: ADC_vect

      Parameters
         None

      Description
         Handles ADC specific interrupts

****************************************************************************/
ISR(ADC_vect)
{
	// Clear ADC Interrupt Flag
	
	ADCSRA |= (1<<ADIF);
	Last_ADC_Value = ADCL + ADCH;
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
      Private Function
         

      Parameters
         

      Description
         

****************************************************************************/

/*******************************************************************************
      File:
      ADC.c
     
      Notes:
      This file contains the ADC module. This module initializes,
	   ADC10 : PB5
   
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

// Interrupts
#include <avr/interrupt.h>

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
	// Select voltage reference as Vcc
	ADMUX &= ~(1<<REFS0);
	AMISCR &= ~(1<<AREFEN);
	   
	// Set PB7 as A2D input PB7 = ADMUX LSB 1000
	ADMUX |= (1<<MUX3);
	ADMUX &= ~(1<<MUX2)&~(1<<MUX0)&~(1<<MUX1);
	   
	// Enable ADC Module and Interrupt after conversion
	ADCSRA |= (1<<ADEN)|(1<<ADIE);
	   
	// Set ADC clock pre-scalar to SysClock/2
	ADCSRA &= ~(1<<ADPS2)&~(1<<ADPS1)&~(1<<ADPS0);
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
         Start_ADC_Measurement

      Parameters
         None

      Description
         Find ADC value at PB7

****************************************************************************/
void Start_ADC_Measurement(void)
{
   // Writing this bit kicks off the ADC measurement
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
   // Get ADC from 2, 8-bit regs,
   //    no need for atomic because we are
   //    in an ISR which is technically an
   //    atomic section
   Last_ADC_Value = 0;
   Last_ADC_Value |= (ADCH<<8);
   Last_ADC_Value |= ADCL;
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################


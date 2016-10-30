/*******************************************************************************
    File:
        LED_Module.c
 
    Notes:
        This file contains the functions to be used by the slave to control
        the LED associated with it.

    External Functions Required:

    Public Functions:
        Set_LED_Duty_Cycle(uint8_t duty_cycle)
        Poll_LED_Duty_Cycle(void)
        Poll_LED_On(void)
        
                
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
#include "PWM.h"

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

// LED PWM value variable
static uint8_t LED_Duty_Cycle = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Set_LED_Duty_Cycle

    Parameters
        uint8_t duty_cycle

    Description
        Sets the LED's duty cycle

****************************************************************************/

void Set_LED_Duty_Cycle(uint8_t duty_cycle)
{   
    // Set module level variable to allow for polling
    LED_Duty_Cycle = duty_cycle;

    // Set duty cycle using PWM module
    Set_PWM_Duty_Cycle(LED_PWM_CHANNEL , duty_cycle);
}

/****************************************************************************
    Public Function
        Poll_LED_Duty_Cycle

    Parameters
        

    Description
        Returns the LED's duty cycle

****************************************************************************/

uint8_t Poll_LED_Duty_Cycle(void)
{
    return LED_Duty_Cycle;
}

/****************************************************************************
    Public Function
        Poll_LED_On

    Parameters
        

    Description
        Returns whether LED is on (1) or off(0)

****************************************************************************/

bool Poll_LED_On(void)
{
    return (LED_Duty_Cycle > 0);
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################


// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

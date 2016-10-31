/*******************************************************************************
    File:
        light_drv.c
  
    Notes:
        This file contains upper level driver for the LED lights.

    External Functions Required:
        List external functions needed by this module

    Public Functions:
        List public functions this module provides
          
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
#include "light_drv.h"

// Include other files below:

// PWM
#include "PWM.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Channel for LED PWM
#define LED_PWM_CHANNEL         ((pwm_channel_t) pwm_channel_a)

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
        Set_Light_Intensity

    Parameters
        None

    Description
        This allows other functions to adjust the light intensity
        for a slave node.

****************************************************************************/
void Set_Light_Intensity(uint8_t requested_intensity)
{
    // Set intensity via PWM
    Set_PWM_Duty_Cycle(LED_PWM_CHANNEL, requested_intensity);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################


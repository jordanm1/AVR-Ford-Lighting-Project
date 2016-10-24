/*******************************************************************************
    File:
        LIN_XCVR_WD_Kicker.c
  
    Notes:
        This file contains the LIN transceiver watchdog kicker routine.

    External Functions Required:

    Public Functions:
          
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
#include "LIN_XCVR_WD_Kicker.h"

// Timer
#include "timer.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Time interval that we kick the watchdog
#define LIN_XCVR_WD_KICK_INTERVAL_MS        35
#define KICK_LENGTH_MS                      2

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Timer for the watchdog kicker
static uint32_t LIN_XCVR_Kick_Timer = 0;

// Parity for pin hi or lo state
static uint8_t Parity = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void kick_LIN_XCVR_WD(uint32_t unused);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_LIN_XCVR_WD_Kicker

    Parameters
        None

    Description
        Initializes and starts the watchdog kicker for LIN xcvr

****************************************************************************/
void Init_LIN_XCVR_WD_Kicker(void)
{
    // Set up PINA3 to kick WD
    PORTA |= (1<<PINA3);
    DDRA |= (1<<PINA3);

    // Register timer
    Register_Timer(&LIN_XCVR_Kick_Timer, kick_LIN_XCVR_WD);

    // Start timer
    Start_Timer(&LIN_XCVR_Kick_Timer, LIN_XCVR_WD_KICK_INTERVAL_MS);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        kick_LIN_XCVR_WD

    Parameters
        None

    Description
        Initializes and starts the watchdog kicker for LIN xcvr

****************************************************************************/
static void kick_LIN_XCVR_WD(uint32_t unused)
{
    // Flip Parity
    Parity ^= 1;

    // Kick xcvr watchdog
    if (0 == Parity)
    {
        // PA3 lo
        PORTA &= ~(1<<PINA3);
        // Restart timer for kick pulse length
        Start_Timer(&LIN_XCVR_Kick_Timer, KICK_LENGTH_MS);
    }
    else
    {
        // PA3 hi
        PORTA |= (1<<PINA3);
        // Restart timer for kick frequency
        Start_Timer(&LIN_XCVR_Kick_Timer, LIN_XCVR_WD_KICK_INTERVAL_MS);
    }
}

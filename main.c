/*******************************************************************************
    File:
        main.c
  
    Notes:
        This file contains the main function run on start up.

    External Functions Required:

    Public Functions:
        None
          
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

// #############################################################################
// ------------ MAIN FUNCTION
// #############################################################################

/****************************************************************************
    Main Function
        main.c

    Parameters
        None

    Description
        No-end loop

****************************************************************************/
int main(void)
{
    // *******************************
    // MICROCONTROLLER INITIALIZATIONS
    // *******************************
    // Disable global interrupts        asm("cli");

    // Setup system clock
    // (per p. 26)
    // "At reset, the CKSEL and SUT fuse settings are copied into the CLKSELR register."
    // We want to run at 8 MHz and the correct values are copied from flash
    //      into the correct registers, except for the prescaler.
    // >>> The internal 8 MHz clock is already chosen.

    // CLKDIV8 comes initially programmed which will divide the 8MHz clock by 8.
    // We need to write to the CLKPR to make the chip run at 8 MHz instead of 1 MHz:
    // (p. 38)
    CLKPR = 1 << CLKPCE;
    CLKPR = 0;
    // * Someone online said something about running these two lines
    //      outside of main so the preamble runs at the right speed...

    // Disable watchdog timer, this is done using jumper JP4 on the dev board.
    // @TODO: WE NEED TO MAKE SURE THIS IS DISABLED IN FW ALSO

    // TODO:
    // Disable modules that aren't used (p. 45)
    //      brown-out detection
    //      adc
    //      analog comparator
    //      internal voltage ref
    //      internal current source
    //      watchdog
    //      make sure no port pins drive resistive loads
    
    // *******************************
    // CALL INITIALIZERS
    // *******************************
    Initialize_Framework();
    
    // *******************************
    // ENABLE GLOBAL INTERRUPTS
    // *******************************
    asm("sei");

    // *******************************
    // RUN EVENTS SERVICE
    // *******************************
    // Run the events service
    Run_Events();

    // *******************************
    // C NECESSARY RETURN
    // *******************************
    return 0;
}

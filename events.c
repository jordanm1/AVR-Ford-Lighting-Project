/*******************************************************************************
    File:
        events.c
  
    Notes:
        This file contains the event handler.

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
#include "events.h"

// #############################################################################
// ------------ DEFINITIONS
// #############################################################################

// Maximum number of events possible in the events service
#define MAXIMUM_NUM_EVENTS              32

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Pending Events
static uint32_t Pending_Events = 0;     // Each bit corresponds to type of event

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

// Event Handler
static bool is_event_pending(uint32_t event_mask);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Post_Event

    Parameters
        None

    Description
        Posts an event to the event list

****************************************************************************/
void Post_Event(uint32_t event_mask)
{
    // Set flag in event list
    Pending_Events |= event_mask;
}

/****************************************************************************
    Public Function
        Run_Events

    Parameters
        None

    Description
        Runs a no-end loop to process and clear any pending events

****************************************************************************/
void Run_Events(void)
{
    // Run no-end main loop
    while (1)
    {
        // Loop through all events
        for (int event = 0; event < NUM_EVENTS; event++)
        {
            if (is_event_pending((0x01 << event)))
            {
                Run_Services(event);
            }
        }
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        is_event_pending()

    Parameters
        Event Mask

    Description
        Checks if an particular event is pending and if so, clears it

****************************************************************************/
static bool is_event_pending(uint32_t event_mask)
{
    // If this event is pending
    if (event_mask == (Pending_Events & event_mask))
    {
        // Clear Event
        Pending_Events &= ~event_mask;

        // Return true
        return true;
    }
    // This event is not pending
    else
    {
        return false;
    }
}

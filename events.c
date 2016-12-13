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

// Atomic Read/Write operations
#include <util/atomic.h>

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

static void process_event_if_pending(uint32_t event_mask);

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
    // We must enter a critical section here, because it is possible that
    // while we are modifying the pending events, an interrupt may occur and 
    // post an event. In this situation, we would lose the new event that 
    // was posted.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Set flag in event list
        Pending_Events |= event_mask;
    }
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
        #if (1 <= NUM_EVENTS)
        process_event_if_pending(EVENT_01);
        #endif
        #if (2 <= NUM_EVENTS)
        process_event_if_pending(EVENT_02);
        #endif
        #if (3 <= NUM_EVENTS)
        process_event_if_pending(EVENT_03);
        #endif
        #if (4 <= NUM_EVENTS)
        process_event_if_pending(EVENT_04);
        #endif
        #if (5 <= NUM_EVENTS)
        process_event_if_pending(EVENT_05);
        #endif
        #if (6 <= NUM_EVENTS)
        process_event_if_pending(EVENT_06);
        #endif
        #if (7 <= NUM_EVENTS)
        process_event_if_pending(EVENT_07);
        #endif
        #if (8 <= NUM_EVENTS)
        process_event_if_pending(EVENT_08);
        #endif
        #if (9 <= NUM_EVENTS)
        process_event_if_pending(EVENT_09);
        #endif
        #if (10 <= NUM_EVENTS)
        process_event_if_pending(EVENT_10);
        #endif
        #if (11 <= NUM_EVENTS)
        process_event_if_pending(EVENT_11);
        #endif
        #if (12 <= NUM_EVENTS)
        process_event_if_pending(EVENT_12);
        #endif
        #if (13 <= NUM_EVENTS)
        process_event_if_pending(EVENT_13);
        #endif
        #if (14 <= NUM_EVENTS)
        process_event_if_pending(EVENT_14);
        #endif
        #if (15 <= NUM_EVENTS)
        process_event_if_pending(EVENT_15);
        #endif
        #if (16 <= NUM_EVENTS)
        process_event_if_pending(EVENT_16);
        #endif
        #if (17 <= NUM_EVENTS)
        process_event_if_pending(EVENT_17);
        #endif
        #if (18 <= NUM_EVENTS)
        process_event_if_pending(EVENT_18);
        #endif
        #if (19 <= NUM_EVENTS)
        process_event_if_pending(EVENT_19);
        #endif
        #if (20 <= NUM_EVENTS)
        process_event_if_pending(EVENT_20);
        #endif
        #if (21 <= NUM_EVENTS)
        process_event_if_pending(EVENT_21);
        #endif
        #if (22 <= NUM_EVENTS)
        process_event_if_pending(EVENT_22);
        #endif
        #if (23 <= NUM_EVENTS)
        process_event_if_pending(EVENT_23);
        #endif
        #if (24 <= NUM_EVENTS)
        process_event_if_pending(EVENT_24);
        #endif
        #if (25 <= NUM_EVENTS)
        process_event_if_pending(EVENT_25);
        #endif
        #if (26 <= NUM_EVENTS)
        process_event_if_pending(EVENT_26);
        #endif
        #if (27 <= NUM_EVENTS)
        process_event_if_pending(EVENT_27);
        #endif
        #if (28 <= NUM_EVENTS)
        process_event_if_pending(EVENT_28);
        #endif
        #if (29 <= NUM_EVENTS)
        process_event_if_pending(EVENT_29);
        #endif
        #if (30 <= NUM_EVENTS)
        process_event_if_pending(EVENT_30);
        #endif
        #if (31 <= NUM_EVENTS)
        process_event_if_pending(EVENT_31);
        #endif
        #if (32 <= NUM_EVENTS)
        process_event_if_pending(EVENT_32);
        #endif
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        process_event_if_pending()

    Parameters
        Event Mask

    Description
        Checks if an particular event is pending and if so, clears it, then
            calls the run functions to process the event

****************************************************************************/
static void process_event_if_pending(uint32_t event_mask)
{
    // We must enter a critical section here, because it is possible that
    // while we are clearing the event, an interrupt may occur and post an 
    // event. In this situation, we would lose the new event that was posted.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // If this event is not pending, return immediately.
        if (!(Pending_Events & event_mask)) return;

        // If we are here, the event must be pending.
        // Clear the event and continue.
        Pending_Events &= ~event_mask;
    }

    // If we are here, the event must be pending.
    // Run all services to process the event.
    Run_Services(event_mask);
}


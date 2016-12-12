/*******************************************************************************
    File:
        framework_setup.c
  
    Notes:
        This file contains the framework which feeds data in the __setup.h file
        into the initialization and event handlers.

    External Functions Required:

    Public Functions:
          
*******************************************************************************/

// #############################################################################
// ------------ INCLUDES
// #############################################################################

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>

// Framework
#include "framework.h"

// #############################################################################
// ------------ DEFINITIONS
// #############################################################################



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
        Initialize_Framework

    Parameters
        None

    Description
        Calls all initializer functions, can service up to 99 functions

****************************************************************************/
void Initialize_Framework(void)
{
    // Call all initializers
    #ifdef INITIALIZER_00
    INITIALIZER_00();
    #endif
    #ifdef INITIALIZER_01
    INITIALIZER_01();
    #endif
    #ifdef INITIALIZER_02
    INITIALIZER_02();
    #endif
    #ifdef INITIALIZER_03
    INITIALIZER_03();
    #endif
    #ifdef INITIALIZER_04
    INITIALIZER_04();
    #endif
    #ifdef INITIALIZER_05
    INITIALIZER_05();
    #endif
    #ifdef INITIALIZER_06
    INITIALIZER_06();
    #endif
    #ifdef INITIALIZER_07
    INITIALIZER_07();
    #endif
    #ifdef INITIALIZER_08
    INITIALIZER_08();
    #endif
    #ifdef INITIALIZER_09
    INITIALIZER_09();
    #endif
    #ifdef INITIALIZER_10
    INITIALIZER_10();
    #endif
    #ifdef INITIALIZER_11
    INITIALIZER_11();
    #endif
    #ifdef INITIALIZER_12
    INITIALIZER_12();
    #endif
    #ifdef INITIALIZER_13
    INITIALIZER_13();
    #endif
    #ifdef INITIALIZER_14
    INITIALIZER_14();
    #endif
    #ifdef INITIALIZER_15
    INITIALIZER_15();
    #endif
}

/****************************************************************************
    Public Function
        Run_Services

    Parameters
        None

    Description
        Calls the services which process events, 
            can service up to 99 functions

****************************************************************************/
void Run_Services(void)
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
    // If event is pending
    if (event_mask == (Pending_Events & event_mask))
    {
        // Clear Event
        Pending_Events &= ~event_mask;

        // Run the services with this event mask
        #ifdef SERVICE_00
        SERVICE_00(event_mask);
        #endif
        #ifdef SERVICE_01
        SERVICE_01(event_mask);
        #endif
        #ifdef SERVICE_02
        SERVICE_02(event_mask);
        #endif
        #ifdef SERVICE_03
        SERVICE_03(event_mask);
        #endif
        #ifdef SERVICE_04
        SERVICE_04(event_mask);
        #endif
        #ifdef SERVICE_05
        SERVICE_05(event_mask);
        #endif
        #ifdef SERVICE_06
        SERVICE_06(event_mask);
        #endif
        #ifdef SERVICE_07
        SERVICE_07(event_mask);
        #endif
        #ifdef SERVICE_08
        SERVICE_08(event_mask);
        #endif
        #ifdef SERVICE_09
        SERVICE_09(event_mask);
        #endif
        #ifdef SERVICE_10
        SERVICE_10(event_mask);
        #endif
        #ifdef SERVICE_11
        SERVICE_11(event_mask);
        #endif
        #ifdef SERVICE_12
        SERVICE_12(event_mask);
        #endif
        #ifdef SERVICE_13
        SERVICE_13(event_mask);
        #endif
        #ifdef SERVICE_14
        SERVICE_14(event_mask);
        #endif
        #ifdef SERVICE_15
        SERVICE_15(event_mask);
        #endif
    }
}

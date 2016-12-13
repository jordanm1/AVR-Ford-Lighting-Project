/*******************************************************************************
    File:
        buttons.c
  
    Notes:
        This file contains the module that implements buttons.

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
#include "buttons.h"

// Include other files below:

// Timer
#include "timer.h"

// Interrupts
#include <avr/interrupt.h>

// #############################################################################
// ------------ BUTTON DEFINITIONS
// #############################################################################

// Define buttons being used by their respective events that are generated
// The user links user-defined events to the predefined rise and fall keywords
//  RISE: PZN_BUTTON_RISE_EVT
//  FALL: PZN_BUTTON_RISE_EVT
//  where Z is the port letter, n is the pin number

#if IS_MASTER_NODE
#else
    #define PB6_BUTTON_RISE_EVT         EVT_BTN_MISC_PRESS
    #define PB6_BUTTON_FALL_EVT         EVT_BTN_MISC_RELEASE
#endif

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

#define A                       (0)     // Arbitrarily chosen, just keyword
#define B                       (1)     // Arbitrarily chosen, just keyword
#define RISE                    (0)     // Arbitrarily chosen, just keyword
#define FALL                    (1)     // Arbitrarily chosen, just keyword

// Debounce time in MS
#define DEBOUNCE_TIME_MS        (10)

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Debounce Timer
static uint32_t Debounce_Timer = EVT_BTN_DEBOUNCE_TIMEOUT;

// Last Sampled Pin States
static uint8_t Last_Port_A_State = 0;
static uint8_t Last_Port_B_State = 0;

// Current Sampled Pin States
static uint8_t Current_Port_A_State = 0;
static uint8_t Current_Port_B_State = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void handle_btn_evts(void);
static bool did_pin_change(uint8_t port, uint8_t pin, uint8_t change_type);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_Buttons

    Parameters
        None

    Description
        This function initializes the button module.

****************************************************************************/
void Init_Buttons(void)
{
    // Set bits in PC mask if the btn event was defined for some pin

    // Port A
    #ifdef PA0_BUTTON_RISE_EVT || PA0_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA0);
    DDRA &= ~(1<<PINA0);
    #endif
    #ifdef PA1_BUTTON_RISE_EVT || PA1_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA1);
    DDRA &= ~(1<<PINA1);
    #endif
    #ifdef PA2_BUTTON_RISE_EVT || PA2_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA2);
    DDRA &= ~(1<<PINA2);
    #endif
    #ifdef PA3_BUTTON_RISE_EVT || PA3_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA3);
    DDRA &= ~(1<<PINA3);
    #endif
    #ifdef PA4_BUTTON_RISE_EVT || PA4_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA4);
    DDRA &= ~(1<<PINA4);
    #endif
    #ifdef PA5_BUTTON_RISE_EVT || PA5_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA5);
    DDRA &= ~(1<<PINA5);
    #endif
    #ifdef PA6_BUTTON_RISE_EVT || PA6_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA6);
    DDRA &= ~(1<<PINA6);
    #endif
    #ifdef PA7_BUTTON_RISE_EVT || PA7_BUTTON_FALL_EVT
    PCMSK0 |= (1<<PINA7);
    DDRA &= ~(1<<PINA7);
    #endif

    // Port B
    #ifdef PB0_BUTTON_RISE_EVT || PB0_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB0);
    DDRB &= ~(1<<PINB0);
    #endif
    #ifdef PB1_BUTTON_RISE_EVT || PB1_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB1);
    DDRB &= ~(1<<PINB1);
    #endif
    #ifdef PB2_BUTTON_RISE_EVT || PB2_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB2);
    DDRB &= ~(1<<PINB2);
    #endif
    #ifdef PB3_BUTTON_RISE_EVT || PB3_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB3);
    DDRB &= ~(1<<PINB3);
    #endif
    #ifdef PB4_BUTTON_RISE_EVT || PB4_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB4);
    DDRB &= ~(1<<PINB4);
    #endif
    #ifdef PB5_BUTTON_RISE_EVT || PB5_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB5);
    DDRB &= ~(1<<PINB5);
    #endif
    #ifdef PB6_BUTTON_RISE_EVT || PB6_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB6);
    DDRB &= ~(1<<PINB6);
    #endif
    #ifdef PB7_BUTTON_RISE_EVT || PB7_BUTTON_FALL_EVT
    PCMSK1 |= (1<<PINB7);
    DDRB &= ~(1<<PINB7);
    #endif

    // Sample current state of pins
    Current_Port_A_State = PINA;
    Current_Port_B_State = PINB;

    // Save current pin state as last pin state
    Last_Port_A_State = Current_Port_A_State;
    Last_Port_B_State = Current_Port_B_State;

    // Register our debounce timer
    Register_Timer(&Debounce_Timer, Post_Event);

    // Enable the pin change interrupts for both ports
    PCICR |= ((1<<PCIE1)|(1<<PCIE0));
}

/****************************************************************************
    Public Function
        Run_Buttons

    Parameters
        None

    Description
        This function runs events related to the buttons.

****************************************************************************/
void Run_Buttons(uint32_t event)
{
    switch (event)
    {
        case EVT_BTN_DEBOUNCE_TIMEOUT:
            // The debounce period has ended.

            // Sample the pins
            Current_Port_A_State = PINA;
            Current_Port_B_State = PINB;

            // Check for differences, and post events for them
            handle_btn_evts();

            // Save current pin state as last pin state
            Last_Port_A_State = Current_Port_A_State;
            Last_Port_B_State = Current_Port_B_State;

            // Re-enable pin change interrupts for port A and port B
            PCICR |= ((1<<PCIE1)|(1<<PCIE0));

            break;

        default:
            break;
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        handle_btn_evts

    Parameters
        None

    Description
        Generates the button events for Port A if a change occured

****************************************************************************/
static void handle_btn_evts(void)
{
    // Check for a changed state for all pins:

    // PA0
    #ifdef PA0_BUTTON_RISE_EVT
    if (did_pin_change(A,0,RISE)) Post_Event(PA0_BUTTON_RISE_EVT);
    #endif
    #ifdef PA0_BUTTON_FALL_EVT
    if (did_pin_change(A,0,FALL)) Post_Event(PA0_BUTTON_FALL_EVT);
    #endif
    // PA1
    #ifdef PA1_BUTTON_RISE_EVT
    if (did_pin_change(A,1,RISE)) Post_Event(PA1_BUTTON_RISE_EVT);
    #endif
    #ifdef PA1_BUTTON_FALL_EVT
    if (did_pin_change(A,1,FALL)) Post_Event(PA1_BUTTON_FALL_EVT);
    #endif
    // PA2
    #ifdef PA2_BUTTON_RISE_EVT
    if (did_pin_change(A,2,RISE)) Post_Event(PA2_BUTTON_RISE_EVT);
    #endif
    #ifdef PA2_BUTTON_FALL_EVT
    if (did_pin_change(A,2,FALL)) Post_Event(PA2_BUTTON_FALL_EVT);
    #endif
    // PA3
    #ifdef PA3_BUTTON_RISE_EVT
    if (did_pin_change(A,3,RISE)) Post_Event(PA3_BUTTON_RISE_EVT);
    #endif
    #ifdef PA3_BUTTON_FALL_EVT
    if (did_pin_change(A,3,FALL)) Post_Event(PA3_BUTTON_FALL_EVT);
    #endif
    // PA4
    #ifdef PA4_BUTTON_RISE_EVT
    if (did_pin_change(A,4,RISE)) Post_Event(PA4_BUTTON_RISE_EVT);
    #endif
    #ifdef PA4_BUTTON_FALL_EVT
    if (did_pin_change(A,4,FALL)) Post_Event(PA4_BUTTON_FALL_EVT);
    #endif
    // PA5
    #ifdef PA5_BUTTON_RISE_EVT
    if (did_pin_change(A,5,RISE)) Post_Event(PA5_BUTTON_RISE_EVT);
    #endif
    #ifdef PA5_BUTTON_FALL_EVT
    if (did_pin_change(A,5,FALL)) Post_Event(PA5_BUTTON_FALL_EVT);
    #endif
    // PA6
    #ifdef PA6_BUTTON_RISE_EVT
    if (did_pin_change(A,6,RISE)) Post_Event(PA6_BUTTON_RISE_EVT);
    #endif
    #ifdef PA6_BUTTON_FALL_EVT
    if (did_pin_change(A,6,FALL)) Post_Event(PA6_BUTTON_FALL_EVT);
    #endif
    // PA7
    #ifdef PA7_BUTTON_RISE_EVT
    if (did_pin_change(A,7,RISE)) Post_Event(PA7_BUTTON_RISE_EVT);
    #endif
    #ifdef PA7_BUTTON_FALL_EVT
    if (did_pin_change(A,7,FALL)) Post_Event(PA7_BUTTON_FALL_EVT);
    #endif

    // PB0
    #ifdef PB0_BUTTON_RISE_EVT
    if (did_pin_change(B,0,RISE)) Post_Event(PB0_BUTTON_RISE_EVT);
    #endif
    #ifdef PB0_BUTTON_FALL_EVT
    if (did_pin_change(B,0,FALL)) Post_Event(PB0_BUTTON_FALL_EVT);
    #endif
    // PB1
    #ifdef PB1_BUTTON_RISE_EVT
    if (did_pin_change(B,1,RISE)) Post_Event(PB1_BUTTON_RISE_EVT);
    #endif
    #ifdef PB1_BUTTON_FALL_EVT
    if (did_pin_change(B,1,FALL)) Post_Event(PB1_BUTTON_FALL_EVT);
    #endif
    // PB2
    #ifdef PB2_BUTTON_RISE_EVT
    if (did_pin_change(B,2,RISE)) Post_Event(PB2_BUTTON_RISE_EVT);
    #endif
    #ifdef PB2_BUTTON_FALL_EVT
    if (did_pin_change(B,2,FALL)) Post_Event(PB2_BUTTON_FALL_EVT);
    #endif
    // PB3
    #ifdef PB3_BUTTON_RISE_EVT
    if (did_pin_change(B,3,RISE)) Post_Event(PB3_BUTTON_RISE_EVT);
    #endif
    #ifdef PB3_BUTTON_FALL_EVT
    if (did_pin_change(B,3,FALL)) Post_Event(PB3_BUTTON_FALL_EVT);
    #endif
    // PB4
    #ifdef PB4_BUTTON_RISE_EVT
    if (did_pin_change(B,4,RISE)) Post_Event(PB4_BUTTON_RISE_EVT);
    #endif
    #ifdef PB4_BUTTON_FALL_EVT
    if (did_pin_change(B,4,FALL)) Post_Event(PB4_BUTTON_FALL_EVT);
    #endif
    // PB5
    #ifdef PB5_BUTTON_RISE_EVT
    if (did_pin_change(B,5,RISE)) Post_Event(PB5_BUTTON_RISE_EVT);
    #endif
    #ifdef PB5_BUTTON_FALL_EVT
    if (did_pin_change(B,5,FALL)) Post_Event(PB5_BUTTON_FALL_EVT);
    #endif
    // PB6
    #ifdef PB6_BUTTON_RISE_EVT
    if (did_pin_change(B,6,RISE)) Post_Event(PB6_BUTTON_RISE_EVT);
    #endif
    #ifdef PB6_BUTTON_FALL_EVT
    if (did_pin_change(B,6,FALL)) Post_Event(PB6_BUTTON_FALL_EVT);
    #endif
    // PB7
    #ifdef PB7_BUTTON_RISE_EVT
    if (did_pin_change(B,7,RISE)) Post_Event(PB7_BUTTON_RISE_EVT);
    #endif
    #ifdef PB7_BUTTON_FALL_EVT
    if (did_pin_change(B,7,FALL)) Post_Event(PB7_BUTTON_FALL_EVT);
    #endif
}

/****************************************************************************
    Private Function
        did_pin_change

    Parameters
        uint8_t port: A or B
        uint8_t pin_num: pin number (0-7)
        uint8_t change_type: RISE or FALL

    Description
        Checks if the parameter pin number changed according to the change
            type specified, since the last sample

****************************************************************************/
static bool did_pin_change(uint8_t port, uint8_t pin, uint8_t change_type)
{
    // Create mask for pin
    uint8_t pin_mask = 1<<pin;

    // Based on port param, use different vars
    switch (port)
    {
        case A:
            if (    (RISE == change_type) &&
                    ((Current_Port_A_State&pin_mask) > (Last_Port_A_State&pin_mask))    )
            {
                return true;
            }
            else if ((FALL == change_type) &&
                    ((Current_Port_A_State&pin_mask) < (Last_Port_A_State&pin_mask))    )
            {
                return true;
            }
            else
            {
                return false;
            }
            break;

        case B:
            if (    (RISE == change_type) &&
                    ((Current_Port_B_State&pin_mask) > (Last_Port_B_State&pin_mask))    )
            {
                return true;
            }
            else if ((FALL == change_type) &&
                    ((Current_Port_B_State&pin_mask) < (Last_Port_B_State&pin_mask))    )
            {
                return true;
            }
            else
            {
                return false;
            }
            break;

        default:
            return false;
            break;
    }
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

ISR(PCINT0_vect)
{
    // Disable pin interrupts for this port
    PCICR &= ~(1<<PCIE0);
    // Start debounce timer
    Start_Timer(&Debounce_Timer, DEBOUNCE_TIME_MS);
}

ISR(PCINT1_vect)
{
    // Disable pin interrupts for this port
    PCICR &= ~(1<<PCIE1);
    // Start debounce timer
    Start_Timer(&Debounce_Timer, DEBOUNCE_TIME_MS);
}

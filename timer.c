/*******************************************************************************
    File:
        timer.c
      
    Notes:
        This file contains the timer module.
        Uses Timer 0.
        The current implementation can be off by +- 1 ms.
    
    External Functions Required:

    Public Functions:
        void Init_Timer_Module(void)
        void Timer_ISR(void)
        void Register_Timer(uint32_t * pointer_to_timer_expire_event_type)
        void Start_Timer(uint32_t * pointer_to_timer_expire_event_type, uint32_t ms_to_expire)
        uint32_t Get_Time_Timer(uint32_t * pointer_to_timer_expire_event_type)
        void Stop_Timer(uint32_t * pointer_to_timer_expire_event_type)
          
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
#include "timer.h"

// Interrupts
#include <avr/interrupt.h>

// Atomic Read/Write operations
#include <util/atomic.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Define number of timers used
#define NUM_TIMERS          (8)

// Define value for output compare match value
#define OC_T0_REG_VALUE     (5)

// Null cb func
#define NULL_TIMER_CB       ((timer_cb_t) 0)

// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################

typedef struct
{
    uint32_t        *p_timer_id;
    timer_cb_t      timer_cb_func;
    bool            timer_running_flag;
    uint32_t        ticks_since_start;
    uint32_t        ticks_remaining;
} timer_t;

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Timer Array
static timer_t Timers[NUM_TIMERS];

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_Timer_Module

    Parameters
        None

    Description
        Initializes the timer module

****************************************************************************/
void Init_Timer_Module(void)
{
    // Initialize all timer slots to empty, not running, and zero counts
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        Timers[i].p_timer_id = 0;
        Timers[i].timer_cb_func = NULL_TIMER_CB;
        Timers[i].timer_running_flag = false;
        Timers[i].ticks_since_start = 0;
        Timers[i].ticks_remaining = 0;
    }

    // Configure Timer 1 Channel A for 1 ms ticks
    // We need to ensure no interrupts occur when accessing 16-bit registers
    // (Just for safety, no ISR should be able to access these registers anyways.)
    // Even though the C code is one line for accessing 16-bit registers,
    //      in ASM it will be done in two cycles.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Stop Timer 1 during initialization by removing the clock source
        TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));

        // Clear Control Register C
        // "However, for ensuring compatibility with future devices,
        //      these bits must be set to zero when TCCR1A is written
        //      when operating in a PWM mode."
        TCCR1C = 0;

        // Enable Timer1, Channel A overflow interrupts,
        TIMSK1 |= (1<<OCIE1A);

        // Do not associate any pins with Timer 1 Channel A
        // Do nothing.

        // Set TOP values for A/B counters, executes in 1 asm lines
        ICR1 = TIMER_1_TOP;

        // Set output compare to value that will cause a match in 1 ms
        //  on channel A
        OCR1A = ((TCNT1+OC_T0_REG_VALUE) % TIMER_1_TOP);

        // Set up Timer1 for normal mode on A channel
        // Clear (COM1A[0:1] = 0b00) for normal port operation
        //      (Per Table 12-2 on p. 132)
        // Set WGM1[0:3]=1110b to define TOP in ICR1A register.
        //      Define TOP in ICR1 register, instead of OCR1A (explanation on p. 125)
        //      (We are doing this because we need to use channel B for PWM)
        TCCR1A &= ~((1<<COM1A1)|(1<<COM1A0)|(1<<WGM10));
        TCCR1A |= (1<<WGM11);

        // Set WGM1[0:3]=1110b
        TCCR1B |= ((1<<WGM13)|(1<<WGM12));

        // Start the clock by selecting a prescaler of f_clk/8 (CS11 set)
        // We want to aim for a frequency of 50 Hz
        // PWM freq is: (per p. 125)
        //      f_pwm = f_clk/(prescale*(1+TOP))
        //      50 Hz = 8 Mhz/(8*(1+19999))
        TCCR1B |= (1<<CS11);
    }
}

/****************************************************************************
    Public Function
        Register_Timer

    Parameters
        uint32_t: Pointer to timer variable holding the value passed into the callback

    Description
        Registers the timer

****************************************************************************/
void Register_Timer(uint32_t * p_new_timer, timer_cb_t new_timer_cb_func)
{
    // Flag to determine if timer is new
    bool is_timer_new = true;

    // Make sure timer is not already registered
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (p_new_timer == Timers[i].p_timer_id)
        {
            is_timer_new = false;
            break;
        }
    }

    // Register timer if it is new
    if (true == is_timer_new)
    {
        // Find next available timer
        for (int i = 0; i < NUM_TIMERS; i++)
        {
            if (0 == Timers[i].p_timer_id)
            {
                Timers[i].p_timer_id = p_new_timer;
                Timers[i].timer_cb_func = new_timer_cb_func;
                Timers[i].timer_running_flag = false;
                Timers[i].ticks_since_start = 0;
                Timers[i].ticks_remaining = 0;
                break;
            }
        }
    }

    // If we didn't find a slot, do nothing.
    // Eventually we could post an error.
}

/****************************************************************************
    Public Function
        Start_Timer

    Parameters
        uint32_t: Pointer to timer variable holding the event type to post
        uint32_t: Timer length in ms

    Description
        Starts the timer

****************************************************************************/
void Start_Timer(uint32_t * p_this_timer, uint32_t time_in_ms)
{
    // Start timer
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (p_this_timer == Timers[i].p_timer_id)
        {
            Timers[i].timer_running_flag = true;
            Timers[i].ticks_since_start = 0;
            Timers[i].ticks_remaining = time_in_ms;
            break;
        }
    }
}

/****************************************************************************
    Public Function
        Get_Time_Timer

    Parameters
        uint32_t: Pointer to timer variable holding the event type to post

    Description
        Gets the time since the timer started

****************************************************************************/
uint32_t Get_Time_Timer(uint32_t * p_this_timer)
{
    // Result Val
    uint32_t return_val = UINT32_MAX;
    
    // Get current time of timer
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (p_this_timer == Timers[i].p_timer_id)
        {
            return_val = Timers[i].ticks_since_start;
            break;
        }
    }
    
    // Return
    return return_val;
}

/****************************************************************************
    Public Function
        Stop_Timer

    Parameters
        uint32_t: Pointer to timer variable holding the event type to post

    Description
        Stops the timer and resets the timer

****************************************************************************/
void Stop_Timer(uint32_t * p_this_timer)
{
    // Start timer
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (p_this_timer == Timers[i].p_timer_id)
        {
            Timers[i].timer_running_flag = false;
            break;
        }
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################



// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

/****************************************************************************
    Public Function
        Timer Module Interrupt Handler

    Parameters
        None

    Description
        Handles the timer overflow interrupt

****************************************************************************/
ISR(TIMER1_COMPA_vect)
{
    // No need to clear interrupt b/c it is cleared in HW (p. 138)

    // Write new value into output compare reg for next tick
    OCR1A = ((OCR1A + OC_T0_REG_VALUE) % TIMER_1_TOP);

    //DDRA |= (1<<PINA2);
    //if (0 == (PORTA&(1<<PINA2)))
    //{
        //PORTA |= (1<<PINA2);
    //}
    //else
    //{
        //PORTA &= ~(1<<PINA2);
    //}

    // Service the running registered timers
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (true == Timers[i].timer_running_flag)
        {
            // Add one to time, subtract one from ticks left
            Timers[i].ticks_since_start += 1;
            Timers[i].ticks_remaining -= 1;

            // If the timer has expired
            if (0 == Timers[i].ticks_remaining)
            {
                // Clear running flag
                Timers[i].timer_running_flag = false;
                
                // Execute cb function with value of id pointer's value
                // If cb is not null, execute
                if (Timers[i].timer_cb_func)
                {
                    Timers[i].timer_cb_func(*(Timers[i].p_timer_id));
                }
            }
        }
    }
}

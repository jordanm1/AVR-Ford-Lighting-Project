/*******************************************************************************
    File:
        timer.c
      
    Notes:
        This file contains the timer module.
        Uses Timer 0.
        The current implementation can be off by +- 0.5 ms.
            (That is, if a function started a timer for 0.5ms, the 
            timer could expire in the next clock cycle via an interrupt)
    
    External Functions Required:

    Public Functions:
        void Init_Timer_Module(void)
        void Timer_ISR(void)
        void Register_Timer(uint32_t * pointer_to_timer_expire_event_type)
        void Start_Timer(uint32_t * pointer_to_timer_expire_event_type, uint32_t ms_to_expire)
        uint32_t Get_Time_Timer(uint32_t * pointer_to_timer_expire_event_type)
        void Stop_Timer(uint32_t * pointer_to_timer_expire_event_type)
        void Start_Short_Timer(uint32_t * pointer_to_timer_expire_event_type, uint32_t ms_div_ten_to_expire)

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
// This should be based on a project wide search for the 
//  number of unique Register_Timer() calls
// Master has 5
// Slave has 5
#define NUM_TIMERS          (5)

// Null cb func
#define NULL_TIMER_CB       ((timer_cb_t) 0)

// #############################################################################
// ------------ TIMER SETUP DEFINITIONS
// #############################################################################

// Current setup and resolution
// We are going for 0.5 ms resolution for this timer.

// Define value for clock select prescale value (Page 102)
#define CLOCK_SELECT_VALUE  ((1<<CS01)|(1<<CS00))           // SYSCLK/32

// Define value for output compare match value
#define OC_T0_REG_VALUE     (125)

// Define number of steps per 1 ms
#define TICK_COUNT_PER_MS   2                               // 0.5ms resolution

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

    // Do not associate any pins with timer 0
    TCCR0A = 0;

    // Stop the clock to timer 0
    TCCR0B = 0;

    // Do not write to timer register
    // Writing prevented interrupts from occurring
    // DO NOT DO TCNT0 = 0;

    // OCR0A: Output compare register A
    // Set to value that would give interrupts at desired frequency of 1ms
    // Math: (8MHz/64)/125 = 0.001sec
    OCR0A = 0;
    OCR0A = OCR0A + OC_T0_REG_VALUE;

    // Enable output compare match interrupt
    TIMSK0 = 1<<OCIE0A;

    // Set up prescaling (p.102, figure 10-5)
    // This kicks off the clock.
    TCCR0B = 0;                                 // Clear the force compare
    TCCR0B |= CLOCK_SELECT_VALUE;

    // *Note: the interrupt flag (OCF0A) is cleared in hardware when the ISR runs
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
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                    Timers[i].p_timer_id = p_new_timer;
                    Timers[i].timer_cb_func = new_timer_cb_func;
                    Timers[i].timer_running_flag = false;
                    Timers[i].ticks_since_start = 0;
                    Timers[i].ticks_remaining = 0;
                }
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
        uint32_t: Timer length in ms, max is (uint32_t/TICK_COUNT_PER_MS)

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
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                Timers[i].timer_running_flag = true;
                Timers[i].ticks_since_start = 0;
                Timers[i].ticks_remaining = (time_in_ms*TICK_COUNT_PER_MS);
            }
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
        Gets the time since the timer started in milliseconds

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
            return_val = ((Timers[i].ticks_since_start)/TICK_COUNT_PER_MS);
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
        Stops the timer, the time remaining is saved, so one could
            stop the timer, then pull the time value when it was stopped

****************************************************************************/
void Stop_Timer(uint32_t * p_this_timer)
{
    // Start timer
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (p_this_timer == Timers[i].p_timer_id)
        {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                Timers[i].timer_running_flag = false;
            }
            break;
        }
    }
}

/****************************************************************************
    Public Function
        Start_Short_Timer

    Parameters
        uint32_t: Pointer to timer variable holding the event type to post
        uint32_t: Timer length in ms/TICK_COUNT_PER_MS, max is uint32_t

    Description
        Starts the short timer (milliseconds/TICK_COUNT_PER_MS)

        Effectively TICK_COUNT_PER_MS is the highest resolution possible
            with the current timer setup.

****************************************************************************/
void Start_Short_Timer(uint32_t * p_this_timer, uint32_t time_in_ms_div_ticksperms)
{
    // Start timer
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (p_this_timer == Timers[i].p_timer_id)
        {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                Timers[i].timer_running_flag = true;
                Timers[i].ticks_since_start = 0;
                Timers[i].ticks_remaining = time_in_ms_div_ticksperms;
            }
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
ISR(TIMER0_COMPA_vect)
{
    // No need to clear interrupt b/c it is cleared in HW (p. 104)

    // *Note: all timer cb functions need to execute within the resolution
    //      of this timer. Otherwise, time will distort. An example is
    //      if we get into this ISR and we take longer than the resolution,
    //      then we will miss interrupts for ticks because they will be 
    //      disabled while we are here. Then we would have to wait for the
    //      timer to roll over which would cause time warp.

    // Write new value into output compare reg for next tick
    OCR0A = OCR0A + OC_T0_REG_VALUE;

    // Service the running registered timers
    for (int i = 0; i < NUM_TIMERS; i++)
    {
        if (true == Timers[i].timer_running_flag)
        {
            // If the timer was started with a non zero time, service the ticks,
            //      otherwise, process the cb immediately
            if (0 < Timers[i].ticks_remaining)
            {
                // Add one to time, subtract one from ticks left
                Timers[i].ticks_since_start += 1;
                Timers[i].ticks_remaining -= 1;
            }

            // If the timer has expired
            if (0 == Timers[i].ticks_remaining)
            {
                // Clear running flag
                Timers[i].timer_running_flag = false;
                
                // Execute cb function with value of id pointer's value
                // If cb is not null, execute
                if (Timers[i].timer_cb_func)
                {
                    // Execute callback
                    Timers[i].timer_cb_func(*(Timers[i].p_timer_id));
                }
            }
        }
    }
}

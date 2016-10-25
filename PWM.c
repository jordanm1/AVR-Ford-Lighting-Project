/*******************************************************************************
    File:
        PWM.c
      
    Notes:
        This file contains the pwm module.
        Uses Timer 1A/B, (two channels of timer 1)
    
    External Functions Required:

    Public Functions:
        void Init_PWM_Module(void)
        void Set_PWM_Duty_Cycle(uint8_t channel, uint8_t new_duty_cycle)
          
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

// Atomic Read/Write operations
#include <util/atomic.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Define OCR values for 0% and 100% duty cycle
#define OCR_DC_HUNDRED      (0x0000)    // With two channels, this is best we can 
                                        //  do unless we just set the line high
#define OCR_DC_ZERO         (0xFFFF)

// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################



// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static uint16_t calc_T1_OCR_count(uint8_t duty_cycle);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_PWM_Module

    Parameters
        None

    Description
        Initializes the PWM module

****************************************************************************/
void Init_PWM_Module(void)
{
    // GOAL: Configure Timer 0 Channel A as our LED PWM channel (ch. A)
    //          and configure Timer 1 Channel B as our Servo PWM channel (ch. B)

    //// Configure Timer 0 Channel A as our LED PWM Channel
    //// Do not associate any pins with timer 0
    //TCCR0A = 0;
//
    //// Stop the clock to timer 0
    //TCCR0B = 0;
//
    //// Do not write to timer register
    //// Writing prevented interrupts from occuring
    //// DO NOT DO TCNT0 = 0;
//
    //// OCR0A: Output compare register A
    //// Set to value that would give interrupts at desired frequency of 1ms
    //// Math: (8MHz/64)/125 = 0.001sec
    //OCR0A = 0;
    //OCR0A = OCR0A + OC_T0_REG_VALUE;
//
    //// Enable output compare match interrupt
    //TIMSK0 = 1<<OCIE0A;
//
    //// Set up 1/64 prescaling (p.102, figure 10-5)
    //// This kicks off the clock.
    //TCCR0B = 1<<CS02;

    // Configure Timer 1 Channel B for our Servo PWM Channel
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

        // Disable Timer1, Channel B overflow interrupts,
        //  We don't need interrupts for PWM.
        TIMSK1 &= ~(1<<OCIE1B);

        // Set up pins for PWM output (p. 85)
        TCCR1D |= (1<<PWM_CH_B_PIN_ENABLE);
        DDRB |= (1<<PWM_CH_B_PIN);

        // Set TOP values for A/B counters, executes in 1 asm lines
        ICR1 = TIMER_1_TOP;

        // Set output compare to value that sets line low (0% duty cycle)
        //  for T1CB
        OCR1B = OCR_DC_ZERO;

        // Set up Timer1 for Fast PWM mode (single slope) on B channel
        // * Note depending on how we implement current sense,
        //      we might need to do phase correct PWM to sample
        //      in the middle of the signal.
        // Set (COM1B[0:1] = 0b11) for output pin high on match, low on TOP, not set for COM1A
        //      (Per Table 12-2 on p. 132)
        // Set WGM1[0:3]=1110b to define TOP in ICR1A register.
        //      Define TOP in ICR1 register, instead of OCR1A (explanation on p. 125)
        //      (Run at a fixed frequency with varying duty cycles)
        TCCR1A &= ~(1<<WGM10);
        TCCR1A |= ((1<<COM1B1)|(1<<COM1B0) \
                    |(1<<WGM11));

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
        Set_PWM_Duty_Cycle

    Parameters
        uint8_t: Definition for channel A or B
        uint8_t: 0-100 duty cycle

    Description
        Sets the duty cycle

****************************************************************************/
void Set_PWM_Duty_Cycle(pwm_channel_t this_channel, uint8_t new_duty_cycle)
{
    // Set OCR1 for the requested channel
    // *Note: no need for atomic operation since OCR is double buffered

    switch(this_channel)
    {
        case pwm_channel_a:
            // Load new value into Timer 0, Ch. A overflow register
            
            break;

        case pwm_channel_b:
            // Load new value into Timer 1, Ch. B overflow register
            OCR1B = calc_T1_OCR_count(new_duty_cycle);
            break;

        default:
            break;
    }

    // TODO: check that we don't need to manually update OCR on every match
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        calc_T1_OCR_count()

    Parameters
        None

    Description
        returns count for OCR based on specified duty cycle

****************************************************************************/
static uint16_t calc_T1_OCR_count(uint8_t duty_cycle)
{
    // Clamp the duty cycle
    if (100 < duty_cycle)
    {
        duty_cycle = 100;
    }
    
    // Return the calculated value
    if (0 < duty_cycle)
    {
        return ((TIMER_1_TOP+1)-((TIMER_1_TOP+1)/100)*duty_cycle);
    }
    return OCR_DC_ZERO;
}

/*******************************************************************************
    File:
        analog_servo_drv.c
  
    Notes:
        This file contains the driver to control an analog servo with PPM.
        This driver allows for 0.001 ms control of the pulse width.
        This follows the standard 50 Hz (20 ms frame length) servo commands.

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
#include "analog_servo_drv.h"

// Interrupts
#include <avr/interrupt.h>

// Standard defs
#include <stddef.h>

// PWM
#include "PWM.h"

// Timer
#include "timer.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Steps for signal generation
#define STEP0                       (0x00)
#define STEP1                       (0x01)
#define STEP2                       (0x02)
#define STEP3                       (0x03)

// Step Bits mask
#define STEP_BITS_XOR_MASK          (0x03)

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Timer for the move
static uint32_t Move_Timer = NON_EVENT;

// Signal generate step
static uint8_t Step = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static uint8_t get_pulse_width(uint8_t this_position);
static void stop_signal(uint32_t unused);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_Analog_Servo_Driver

    Parameters
        None

    Description
        Initializes the signal generation for driving an analog servo

****************************************************************************/
void Init_Analog_Servo_Driver(void)
{
    // Stop the signal to the servo in case it was a non-zero duty cycle
    stop_signal(NON_EVENT);

    // Disable PWM output on the servo pin and drive the line low
    TCCR1D &= ~(1<<ANALOG_SERVO_PWM_EN);
    ANALOG_SERVO_DRV_PORT &= ~(1<<ANALOG_SERVO_DRV_PIN);

    // Initialize step to step 0
    Step = 0;

    // Enable TOP interrupts for channel B of timer 1
    // *Note: According to page 133, this interrupt occurs at TOP
    TIMSK1 |= (1<<TOIE1);

    // Register move length timer
    Register_Timer(&Move_Timer, stop_signal);

    // Signal generation will occur when PWM gets enabled. But,
    //  first we will unlink the pin from the PWM module. So no
    //  signal will be generated.

    // Testing
    Set_PWM_Duty_Cycle(ANALOG_SERVO_PWM_CH, 40);
}

/****************************************************************************
    Public Function
        Move_Analog_Servo_To_Position

    Parameters
        None

    Description
        Moves servo to position, then disables the signals,
        when the SERVO_DRIVE_TIME_MS have elapsed

****************************************************************************/
void Move_Analog_Servo_To_Position(uint8_t requested_position)
{
    // Only execute if position is valid
    if (SERVO_STAY != requested_position)
    {
        // Change duty cycle of analog servo pwm channel


        // Start move timer (this module will send signals for this amount of time)
        // The cb function for this timer is stop_signal()
        Start_Timer(&Move_Timer, SERVO_DRIVE_TIME_MS);
    }
}

/****************************************************************************
    Public Function
        Hold_Analog_Servo_Position

    Parameters
        None

    Description
        Holds the desired servo position by continuously sending servo commands

****************************************************************************/
void Hold_Analog_Servo_Position(uint8_t requested_position)
{
    // Only execute if position is valid
    if (SERVO_STAY != requested_position)
    {
        // Change duty cycle of analog servo pwm channel

    }
}

/****************************************************************************
    Public Function
        Release_Analog_Servo

    Parameters
        None

    Description
        Disables the analog servo (i.e. it stops any signals being sent)

****************************************************************************/
void Release_Analog_Servo(void)
{
    // Stop signal
    stop_signal(NON_EVENT);
}

/****************************************************************************
    Private Function
        Is_Servo_Position_Valid

    Parameters
        None

    Description
        Determines if position requested is a valid position,
            based on the slave parameters

****************************************************************************/
bool Is_Servo_Position_Valid(const slave_parameters_t * p_slave_params, uint8_t requested_position)
{
    // If requested position is servo stay, the position is immediately invalid
    if (SERVO_STAY == requested_position) return false;

    // If the max position is greater than the min position
    // (Example: min = 1, max = 6, requested = 10)
    if (p_slave_params->position_max > p_slave_params->position_min)
    {
        if  (   (p_slave_params->position_min > requested_position)
                ||
                (p_slave_params->position_max < requested_position)
            )
        {
            // Position is outside of possible range, return false
            return false;
        }
        else
        {
            return true;
        }
    }
    // If the min position is greater than the max position
    // (Example: min = 8, max = 0, requested = 10)
    else if (p_slave_params->position_min > p_slave_params->position_max)
    {
        if  (   (p_slave_params->position_min < requested_position)
                ||
                (p_slave_params->position_max > requested_position)
            )
        {
            // Position is outside of possible range, return false
            return false;
        }
        else
        {
            return true;
        }
    }
    // Otherwise the positions are equal
    // (Example: min = 5, max = 5, requested = 10)
    else
    {
        if (p_slave_params->position_min != requested_position)
        {
            // The requested position does not match the single
            //  possible position
            return false;
        }
        else
        {
            return true;
        }
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        get_pulse_width

    Parameters
        None

    Description
        Returns the pulse width in tenths of milliseconds based on
        positions and min/max pulse lengths

****************************************************************************/
static uint8_t get_pulse_width(uint8_t this_position)
{
//     // Example: Valid Inputs are 0-10 (11 positions)
//     if ((MAX_PULSE_WIDTH_TENTHMS-MIN_PULSE_WIDTH_TENTHMS) < this_position)
//     {
//         return MAX_PULSE_WIDTH_TENTHMS;
//     }
//     else
//     {
//         return MIN_PULSE_WIDTH_TENTHMS+this_position;
//     }
    return 0;
}

/****************************************************************************
    Private Function
        stop_signal

    Parameters
        None

    Description
        stops the PPM command pulses being sent to the servo

****************************************************************************/
static void stop_signal(uint32_t unused)
{
    // Set 0% duty cycle on PWM channel b
    Set_PWM_Duty_Cycle(ANALOG_SERVO_PWM_CH, 0);
}

/****************************************************************************
    Private Function
        generate_signal

    Parameters
        None

    Description
        generates the pulses for the analog servo PPM

        Steps:
        -       TOP Interrupt
        0       Enable PWM on Pin
        -       TOP Interrupt
        1       Disable PWM on Pin
        -       TOP Interrupt
        2       Do nothing
        -       TOP Interrupt
        3       Do nothing

****************************************************************************/
ISR(TIMER1_OVF_vect)
{
    // Switch for fastest execution time
    switch (Step)
    {
        case STEP0:
            // Enable PWM out on the analog servo drive pin
            TCCR1D |= (1<<ANALOG_SERVO_PWM_EN);
            break;

        case STEP1:
            // Disable PWM out on the analog servo drive pin
            TCCR1D &= ~(1<<ANALOG_SERVO_PWM_EN);
            // Drive line low (This order is okay because at TOP
            //  the line should be low, this also seems to have
            //  less jitter based on o'scope-ing)
            ANALOG_SERVO_DRV_PORT &= ~(1<<ANALOG_SERVO_DRV_PIN);
            break;

        default:
            break;
    }

    // Increment step number for next TOP interrupt
    Step++;
    Step &= STEP_BITS_XOR_MASK;
}

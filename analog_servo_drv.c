/*******************************************************************************
    File:
        analog_servo_drv.c
  
    Notes:
        This file contains the driver to control an analog servo with PPM.
        This driver allows for 0.1 ms control of the pulse width.
        This follows the standard 50 Hz (25 ms frame length) servo commands.

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

// Standard defs
#include <stddef.h>

// Timer
#include "timer.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Steps for signal generation
#define STEP0                       (0x00)
#define STEP1                       (0x01)

// Step Bits mask
#define STEP_BITS_XOR_MASK          (0x01)

// Servo command pulse period
#define PULSE_PERIOD_TENTHMS        200

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Timer for the move
static uint32_t Move_Timer = NON_EVENT;

// Timer for the pulse
static uint32_t Signal_Timer = NON_EVENT;

// Pulse width during current pulse period
static uint8_t Current_Pulse_Width_TenthMS = MIN_PULSE_WIDTH_TENTHMS;

// Pulse width that is requested
static uint8_t Requested_Pulse_Width_TenthMS = MIN_PULSE_WIDTH_TENTHMS;

// Flag to determine whether we should send commands
static bool Signal_Enable = false;

// Signal generate step
static uint8_t Step = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static uint8_t get_pulse_width(uint8_t this_position);
static void stop_signal(uint32_t unused);
static void generate_signal(uint32_t unused);

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
    // Set up pin to drive servo, set up as digital out, low state initially
    ANALOG_SERVO_CH1_PORT &= ~(1<<ANALOG_SERVO_CH1_PIN);
    ANALOG_SERVO_CH1_PDIR |= (1<<ANALOG_SERVO_CH1_PIN);

    // Register move length timer
    Register_Timer(&Move_Timer, stop_signal);

    // Register signal timer
    Register_Timer(&Signal_Timer, generate_signal);

    // Initialize pulse width to valid time
    Requested_Pulse_Width_TenthMS = MIN_PULSE_WIDTH_TENTHMS;

    // Disable signal generation
    Signal_Enable = false;

    // Intialize step to step 0
    Step = 0;

    // Call signal generator to start signal generation 
    // (Signal will initially be off)
    generate_signal(NON_EVENT);
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
    if (SERVO_NO_MOVE != requested_position)
    {
        // Set requested pulse width
        Requested_Pulse_Width_TenthMS = get_pulse_width(requested_position);

        // Enable signal generation
        Signal_Enable = true;

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
void Hold_Analog_Servo_Position(uint8_t position)
{
    // Only execute if position is valid
    if (SERVO_NO_MOVE != position)
    {
        // Stop the move timer (just in case it's running)
        Stop_Timer(&Move_Timer);

        // Set requested pulse width
        Requested_Pulse_Width_TenthMS = get_pulse_width(position);

        // Set flag that tells the module to send commands
        Signal_Enable = true;
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
    // Example: Valid Inputs are 0-10 (11 positions)
    if ((MAX_PULSE_WIDTH_TENTHMS-MIN_PULSE_WIDTH_TENTHMS) < this_position)
    {
        return MAX_PULSE_WIDTH_TENTHMS;
    }
    else
    {
        return MIN_PULSE_WIDTH_TENTHMS+this_position;
    }
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
    // Clear Signal_Enable flag
    Signal_Enable = false;
}

/****************************************************************************
    Private Function
        generate_signal

    Parameters
        None

    Description
        generates the pulses for the analog servo PPM

        Steps:
        -       Timer Expires
        0       Start timer for pulse width, set line high
        -       Timer Expires
        1       Start timer for (pulse period-pulse width), set line low

****************************************************************************/
static void generate_signal(uint32_t unused)
{
    // Switch for fastest execution time
    switch (Step)
    {
        case STEP0:
            // Get requested pulse width for this pulse frame
            Current_Pulse_Width_TenthMS = Requested_Pulse_Width_TenthMS;
            // If signal is enabled, set line high, otherwise the line will stay low
            if (Signal_Enable)
            {
                // Set line high
                ANALOG_SERVO_CH1_PORT |= (1<<ANALOG_SERVO_CH1_PIN);
            }
            // Restart timer for pulse length
            Start_Short_Timer(&Signal_Timer, Current_Pulse_Width_TenthMS);
            break;

        case STEP1:
            // Set line low
            ANALOG_SERVO_CH1_PORT &= ~(1<<ANALOG_SERVO_CH1_PIN);
            // Restart timer for the rest of the pulse period
            Start_Short_Timer(&Signal_Timer, (PULSE_PERIOD_TENTHMS-Current_Pulse_Width_TenthMS));
            break;

        default:
            break;
    }

    // Step number for next timer expired cycle
    Step ^= STEP_BITS_XOR_MASK;
}

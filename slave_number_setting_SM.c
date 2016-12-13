/*******************************************************************************
    File:
        slave_number_setting_SM.c
  
    Notes:
        This file contains the state machine to set the slave number.

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
#include "slave_number_setting_SM.h"

// Include other files below:

// Timer
#include "timer.h"

// Light Driver
#include "light_drv.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

#define ENTER_MODE_HOLD_TIME_MS         (10000)
#define EXIT_MODE_HOLD_TIME_MS          (5000)
#define MAX_TIME_IN_MODE_MS             (60000)
#define SETTING_MODE_LIGHT_INTENSITY    (30)
#define NUM_UNAVOIDABLE_RELEASES        (1)

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Current State
slave_number_setting_state_t Current_State = IDLE_STATE;

// Last_Set_Slave_Number
uint8_t Last_Set_Slave_Number = INVALID_SLAVE_NUMBER;

// Counter to hold number of times the button was pressed
uint8_t Release_Counter = 0;

// We need two timers, one for macro look on SM and one for use inside
uint32_t Main_Timer = EVT_SETTING_MODE_MAIN_TIMEOUT;
uint32_t Auxiliary_Timer = EVT_SETTING_MODE_AUX_TIMEOUT;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_Slave_Number_Setting_SM

    Parameters
        None

    Description
        This just registers the timers we need for this SM.

****************************************************************************/
void Init_Slave_Number_Setting_SM(void)
{
    // Register timers
    Register_Timer(&Main_Timer, Post_Event);
    Register_Timer(&Auxiliary_Timer, Post_Event);
}


/****************************************************************************
    Public Function
        Run_Slave_Number_Setting_SM

    Parameters
        None

    Description
        This processes events for the slave number setting state machine.

****************************************************************************/
void Run_Slave_Number_Setting_SM(uint32_t event)
{
    switch (Current_State)
    {
        case IDLE_STATE:

            switch (event)
            {
                case EVT_BTN_MISC_PRESS:

                    // Start hold time timer
                    Start_Timer(&Auxiliary_Timer, ENTER_MODE_HOLD_TIME_MS);

                    // Change state to ENTERING_SETTING_MODE_STATE
                    Current_State = ENTERING_SETTING_MODE_STATE;

                    break;

                default:
                    break;
            }

            break;

        case ENTERING_SETTING_MODE_STATE:

            switch (event)
            {
                case EVT_BTN_MISC_RELEASE:

                    // Stop hold time timer
                    Stop_Timer(&Auxiliary_Timer);

                    // Change state to IDLE_STATE
                    Current_State = IDLE_STATE;

                    break;

                case EVT_SETTING_MODE_AUX_TIMEOUT:

                    // Start overall setting mode timer
                    Start_Timer(&Main_Timer, MAX_TIME_IN_MODE_MS);

                    // Turn on LED
                    Set_Light_Intensity(SETTING_MODE_LIGHT_INTENSITY);

                    // Change state to SETTING_MODE_STATE
                    Current_State = SETTING_MODE_STATE;

                    break;

                default:
                    break;
            }

            break;

        case SETTING_MODE_STATE:

            switch (event)
            {
                case EVT_BTN_MISC_PRESS:

                    // The user pressed the button. Turn on the LED
                    //  and start the timer to check for hold to exit.

                    // Turn on LED
                    Set_Light_Intensity(SETTING_MODE_LIGHT_INTENSITY);

                    // Start aux timer for exit hold time
                    Start_Timer(&Auxiliary_Timer, EXIT_MODE_HOLD_TIME_MS);

                    break;

                case EVT_BTN_MISC_RELEASE:

                    // The user released the button. Turn off the LED,
                    //  increment our counter, and stop the hold timer.

                    // Turn off LED
                    Set_Light_Intensity(LIGHT_OFF);

                    // Increment release counter
                    Release_Counter++;

                    // Stop aux timer for exit hold time
                    Stop_Timer(&Auxiliary_Timer);

                    break;

                case EVT_SETTING_MODE_AUX_TIMEOUT:

                    // The user held the button and wants
                    //  to exit the mode and set the slave number.

                    // If the Release Counter is greater than number of
                    //  NUM_UNAVOIDABLE_RELEASES, then we got a new slave number
                    if (NUM_UNAVOIDABLE_RELEASES < Release_Counter)
                    {
                        // Save the count-1 as the last set slave number
                        // It is count-1 because we enter this state with the button pressed
                        // so the first release doesn't mean anything
                        Last_Set_Slave_Number = Release_Counter-NUM_UNAVOIDABLE_RELEASES;

                        // Post event to slave service to alert that new
                        //  slave number has been set
                        Post_Event(EVT_SLAVE_NUM_SET);
                    }

                    // Stop the main timer
                    Stop_Timer(&Main_Timer);

                    // Turn off LED
                    Set_Light_Intensity(LIGHT_OFF);

                    // Clear the number counter
                    Release_Counter = 0;

                    // Change state to IDLE_STATE:
                    Current_State = IDLE_STATE;

                    break;

                case EVT_SETTING_MODE_MAIN_TIMEOUT:

                    // The user did not exit the mode properly within
                    //  the max mode time. Just go back to idle and
                    //  clear the number counter.

                    // Stop the aux timer, in case it was running
                    Stop_Timer(&Auxiliary_Timer);

                    // Turn off LED
                    Set_Light_Intensity(LIGHT_OFF);

                    // Clear the number counter
                    Release_Counter = 0;

                    // Change state to IDLE_STATE:
                    Current_State = IDLE_STATE;

                    break;

                default:
                    break;
            }

            break;
    }
}

/****************************************************************************
    Public Function
        In_Slave_Number_Setting_Mode

    Parameters
        None

    Description
        This just returns whether the node is in the slave number 
        setting mode.

        @TODO: This would ideally be solved using shared access
                to the LED.

****************************************************************************/
bool In_Slave_Number_Setting_Mode(void)
{
    if (SETTING_MODE_STATE == Current_State) return true;
    return false;
}

/****************************************************************************
    Public Function
        Get_Last_Set_Slave_Number

    Parameters
        None

    Description
        This function returns the last set slave number.

****************************************************************************/
uint8_t Get_Last_Set_Slave_Number(void)
{
    if ((LOWEST_SLAVE_NUMBER <= Last_Set_Slave_Number) && (MAX_NUM_SLAVES >= Last_Set_Slave_Number))
    {
        return Last_Set_Slave_Number;
    }
    else
    {
        return INVALID_SLAVE_NUMBER;
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################


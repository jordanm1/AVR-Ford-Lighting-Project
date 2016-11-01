/*******************************************************************************
    File:
        slave_service.c
      
    Notes:
        This file contains the main service to be run on the slave nodes.
    
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
#include "slave_service.h"

// LIN top layer
#include "MS_LIN_top_layer.h"

// ADC
#include "ADC.h"

// Slave Parameters
#include "slave_parameters.h"

// Light
#include "light_drv.h"

// Servo
#include "analog_servo_drv.h"

// Atomic Read/Write operations
#include <util/atomic.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// These values should only exist in a single module for each node
static uint8_t My_Node_ID;                          // This node's ID
static uint8_t My_Command_Data[LIN_PACKET_LEN];     // This node's current command
static uint8_t My_Status_Data[LIN_PACKET_LEN];      // This node's status

static const slave_parameters_t * p_My_Parameters;  // Pointer to this node's parameters

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void set_slave_id(void);
static void process_intensity_cmd(void);
static void process_position_cmd(void);
static bool is_cmd_valid(uint8_t cmd_index);
static bool is_position_valid(const slave_parameters_t * p_slave_params, uint8_t requested_position);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_Slave_Service

    Parameters
        None

    Description
        Initializes the slave

****************************************************************************/
void Init_Slave_Service(void)
{
    // Initialize command and status arrays
    // TODO
    My_Command_Data[INTENSITY_INDEX] = NON_COMMAND;
    My_Command_Data[POSITION_INDEX] = NON_COMMAND;
    My_Status_Data[INTENSITY_INDEX] = LIGHT_OFF;
    My_Status_Data[POSITION_INDEX] = SERVO_STAY;

    // Initialize ADC, read slave number, create & store slave ID in RAM
    // TODO
    My_Node_ID = 0x06;

    // Update the pointer to our slave parameters
    p_My_Parameters = Get_Slave_Parameters(My_Node_ID);

    // Disable ADC
    // TODO

    // Initialize light to LIGHT_OFF
    // TODO

    // Initialize servo to SERVO_STAY
    // TODO

    // Initialize LIN
    MS_LIN_Initialize(&My_Node_ID, &My_Command_Data[0], &My_Status_Data[0]);

    // Post an event that forces the slave to get a valid ID before moving on
    // We need to post an event because all initializers are in a critical
    //    section. Run_Events() occurs after enabling global interrupts.
    Post_Event(EVT_SLAVE_GET_ID);
}

/****************************************************************************
    Public Function
        Run_Slave_Service

    Parameters
        None

    Description
        Processes events for the this slave node

****************************************************************************/
void Run_Slave_Service(uint32_t event_mask)
{
    switch(event_mask)
    {
        case EVT_SLAVE_GET_ID:
            // We need to get this slave's ID before continuing
            // This function calls blocking code
            // TODO: Temporarily comment this out.
            // set_slave_id();

            // Get the pointer to our parameters struct
            // p_My_Parameters = Get_Slave_Parameters(My_Node_ID);
            break;

        case EVT_SLAVE_NEW_CMD:
            // We got a new command

            // Always process intensity command
            process_intensity_cmd();

            // Alway process the position command,
            process_position_cmd();

            break;

        case EVT_SLAVE_OTHER:
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
        set_slave_id()

    Parameters
        None

    Description
        Polls the ADC until a reasonable value is obtained,
        then sets this slave's id number based on the ADC count

****************************************************************************/
static void set_slave_id(void)
{
    // Kick off ADC measurement
    Start_ADC_Measurement();

    // Wait until we get a legitimate ADC count
    while (IMPOSSIBLE_ADC_COUNT == Get_ADC_Result())
    {
        // Do nothing, just wait and block.
    }

    // Update our ID based on the ADC count
    // There are 0-59 possible ID's under LIN 2.x
    // We reserve 0-1 for the Master, so we have
    //    2-59 ID's left (58 unique ID's)
    // We divide 58 by 2 to get the maximum number
    //    of slaves in this system as 29.
    // Thus, to get 29 unique ID's, we only need
    //    the top 5 bits of the ADC data.
    
    // Shift the ADC count right by 5
    uint8_t temp_ADC_count = (Get_ADC_Result()>>5);

    // Set our ID to ADC count
    if (30 > temp_ADC_count)
    {
        // Set this node's ID
        My_Node_ID = temp_ADC_count;
    }
    else
    {
        // Invalid ADC count, we only support 29 ID's.
        // Do nothing.
    }

    // *Note: it is 33 counts per ID, #1 = 33, #2 = 66, ... , #29 = 957
}

/****************************************************************************
    Private Function
        process_intensity_cmd()

    Parameters
        none

    Description
        processes the intensity command

****************************************************************************/
static void process_intensity_cmd(void)
{
    // Enter critical section so when we are copying data, we know the data
    //      we are copying is the same data we checked.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // General Flow:
        // If the command is valid, then we copy the command to our status
        //      then we execute whatever is in our status
        if (is_cmd_valid(INTENSITY_INDEX))
        {
            // If command differs from our status execute intensity command
            if (My_Status_Data[INTENSITY_INDEX] != My_Command_Data[INTENSITY_INDEX])
            {
                // Update our status as the command
                My_Status_Data[INTENSITY_INDEX] = My_Command_Data[INTENSITY_INDEX];

                // Set light intensity
                Set_Light_Intensity(My_Status_Data[INTENSITY_INDEX]);
            }
        }
    }
}

/****************************************************************************
    Private Function
        process_position_cmd()

    Parameters
        none

    Description
        processes the position command

****************************************************************************/
static void process_position_cmd(void)
{
    // Enter critical section so when we are copying data, we know the data
    //      we are copying is the same data we checked.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // General Flow:
        // If the command is valid, then we copy the command to our status
        //      then we execute whatever is in our status
        if (is_cmd_valid(POSITION_INDEX))
        {
            // If command differs from our status and position is valid, execute move command
            if  (   (My_Status_Data[POSITION_INDEX] != My_Command_Data[POSITION_INDEX])
                    &&
                    is_position_valid(p_My_Parameters, My_Command_Data[POSITION_INDEX])
                )
            {
                // Update our status as the command
                My_Status_Data[POSITION_INDEX] = My_Command_Data[POSITION_INDEX];

                // Change servo position
                Move_Analog_Servo_To_Position(My_Status_Data[POSITION_INDEX]);
            }
        }
    }
}

/****************************************************************************
    Private Function
        is_cmd_valid()

    Parameters
        cmd_index

    Description
        returns bool based on if command is enabled

****************************************************************************/
static bool is_cmd_valid(uint8_t cmd_index)
{
    // If command is valid
    if (NON_COMMAND != My_Command_Data[cmd_index])
    {
        return true;
    }
    else
    {
        return false;
    }
}

/****************************************************************************
    Private Function
        is_position_valid

    Parameters
        None

    Description
        Determines if position requested is a valid position,
            based on the slave parameters

****************************************************************************/
static bool is_position_valid(const slave_parameters_t * p_slave_params, uint8_t requested_position)
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


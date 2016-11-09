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

// Command and Status Helpers
#include "cmd_sts_helpers.h"

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

// *Note: We have set up the system so the slaves don't need their parameters.
// (We don't need a pointer to our slave parameters.)

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void set_slave_id(void);
static void process_intensity_cmd(void);
static void process_position_cmd(void);

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
    Write_Intensity_Data(My_Command_Data, INTENSITY_NON_COMMAND);
    Write_Position_Data(My_Command_Data, POSITION_NON_COMMAND);
    Write_Intensity_Data(My_Status_Data, LIGHT_OFF);
    Write_Position_Data(My_Status_Data, SERVO_STAY);

    // Initialize ADC, read slave number, create & store slave ID in RAM
    // TODO
    My_Node_ID = 0x02;

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
        if (INTENSITY_NON_COMMAND != Get_Intensity_Data(My_Command_Data))
        {
            // If command differs from our status execute intensity command
            if (Get_Intensity_Data(My_Status_Data) != Get_Intensity_Data(My_Command_Data))
            {
                // Update our status as the command
                Write_Intensity_Data(My_Status_Data, Get_Intensity_Data(My_Command_Data));

                // Set light intensity
                Set_Light_Intensity(Get_Intensity_Data(My_Status_Data));
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
        if (POSITION_NON_COMMAND != Get_Position_Data(My_Command_Data))
        {
            // If command differs from our status and position is valid, execute move command
            if (Get_Position_Data(My_Status_Data) != Get_Position_Data(My_Command_Data))
            {
                // Update our status as the command
                Write_Position_Data(My_Status_Data, Get_Position_Data(My_Command_Data));

                // Change servo position, based on our new status
                Move_Analog_Servo_To_Position(Get_Position_Data(My_Status_Data));
            }
        }
    }
}




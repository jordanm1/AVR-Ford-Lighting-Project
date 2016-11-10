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

// Slave Number Setting
#include "slave_number_setting_SM.h"

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
static uint8_t My_Node_ID;                              // This node's ID
static uint8_t My_Command_Data[LIN_PACKET_LEN];         // This node's current command
static uint8_t My_Status_Data[LIN_PACKET_LEN];          // This node's status
static uint8_t * p_My_Command_Data = My_Command_Data;
static uint8_t * p_My_Status_Data = My_Status_Data;

// *Note: We have set up the system so the slaves don't need their parameters.
// (We don't need a pointer to our slave parameters.)

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void save_our_id_to_flash(uint8_t * p_node_id);
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
    Write_Intensity_Data(p_My_Command_Data, INTENSITY_NON_COMMAND);
    Write_Position_Data(p_My_Command_Data, POSITION_NON_COMMAND);
    Write_Intensity_Data(p_My_Status_Data, LIGHT_OFF);
    Write_Position_Data(p_My_Status_Data, SERVO_STAY);

    // Read our slave number from flash
    // @TODO:
    My_Node_ID = GET_SLAVE_BASE_ID(1);

    // Initialize light to LIGHT_OFF
    Set_Light_Intensity(LIGHT_OFF);

    // Release the servo so it does not move
    Release_Analog_Servo();

    // Initialize LIN
    MS_LIN_Initialize(&My_Node_ID, p_My_Command_Data, p_My_Status_Data);
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
        case EVT_SLAVE_NUM_SET:
            // A new ID has been set for us.

            // Only do something if the # is different than our current #
            if  (   (Get_Last_Set_Slave_Number() != GET_SLAVE_NUMBER(My_Node_ID))
                    &&
                    (INVALID_SLAVE_NUMBER != Get_Last_Set_Slave_Number())
                )
            {
                // Set our ID based on the user set slave number
                My_Node_ID = GET_SLAVE_BASE_ID(Get_Last_Set_Slave_Number());

                // Save our new ID in flash memory
                save_our_id_to_flash(&My_Node_ID);
            }

            break;

        case EVT_SLAVE_NEW_CMD:
            // We got a new command.

            // Process commands if we're not in the setting mode
            if (!In_Slave_Number_Setting_Mode())
            {
                // Process the intensity command
                process_intensity_cmd();

                // Process the position command,
                process_position_cmd();
            }

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
        save_our_id_to_flash()

    Parameters
        None

    Description
        Saves our current ID to flash

****************************************************************************/
static void save_our_id_to_flash(uint8_t * p_node_id)
{
    // @TODO: call external module that handles flash storage
    return;
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
        if (INTENSITY_NON_COMMAND != Get_Intensity_Data(p_My_Command_Data))
        {
            // If command differs from our status execute intensity command
            if (Get_Intensity_Data(p_My_Status_Data) != Get_Intensity_Data(p_My_Command_Data))
            {
                // Update our status as the command
                Write_Intensity_Data(p_My_Status_Data, Get_Intensity_Data(p_My_Command_Data));

                // Set light intensity
                Set_Light_Intensity(Get_Intensity_Data(p_My_Status_Data));
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
        if (POSITION_NON_COMMAND != Get_Position_Data(p_My_Command_Data))
        {
            // If command differs from our status and position is valid, execute move command
            if (Get_Position_Data(p_My_Status_Data) != Get_Position_Data(p_My_Command_Data))
            {
                // Update our status as the command
                Write_Position_Data(p_My_Status_Data, Get_Position_Data(p_My_Command_Data));

                // Change servo position, based on our new status
                Move_Analog_Servo_To_Position(Get_Position_Data(p_My_Status_Data));
            }
        }
    }
}




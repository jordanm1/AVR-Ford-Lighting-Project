/*******************************************************************************
    File:
        MS_LIN_top_layer.c
  
    Notes:
        This file contains upper level implementation for LIN communication
        between the master and slave nodes of the 360 lighting system.

    External Functions Required:
        Post_Event_Slave_Service()

    Public Functions:
          
*******************************************************************************/

// #############################################################################
// ------------ INCLUDES
// #############################################################################

# if 0

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>

// Config file
#include "config.h"

// Framework
#include "framework.h"

// This module's header file
#include "MS_LIN_top_layer.h"

// The Atmel AVR LIN driver library
#include "lin_drv.h"

// Interrupts
#include <avr/interrupt.h>

// Command/Status helpers
#include "cmd_sts_helpers.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Use pointers so the values can only exist in one place
static uint8_t * p_My_Node_ID;          // Pointer to this node's ID
static uint8_t * p_My_Command_Data;     // Pointer to this node's command store
static uint8_t * p_My_Status_Data;      // Pointer to this node's status store

// LIN error counters
static uint8_t My_LIN_Error_Count = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void lin_id_task(void);
static void lin_rx_task(void);
static void lin_tx_task(void);
static void lin_err_task(void);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        MS_LIN_Initialize

    Parameters
        uint8_t * p_this_node_id: pointer to this node's id
        uint8_t * p_command_data: pointer to this node's command data store
        uint8_t * p_status_data: pointer to this node's status data store

    Description
        Initializes the LIN bus for the nodes based on ATtiny167

****************************************************************************/
void MS_LIN_Initialize(uint8_t * p_this_node_id, uint8_t * p_command_data, \
    uint8_t * p_status_data)
{
    // 0. Enable the LIN transceiver via PB0 which is connected to 
    // ENABLE (ATA6617C: Pin 18) on our custom PCBs.
    // For the development boards we need to use an external wire jumper.
    // For the chip we use to interface with the modem, we can just disable
    // the entire LIN XCVR because we are using the LIN peripheral for UART
    // and thus do not need to use the XCVR.
    PORTB |= (1<<PINB0);
    DDRB |= (1<<PINB0);

    // 1. Call the LIN init function from the driver layer
    // * Arguments are found in lin_drv.h, config.h
    lin_init((OUR_LIN_SPEC), (CONF_LINBRR));

    // 2. Save the pointer to this node's ID
    p_My_Node_ID = p_this_node_id;

    // 3. Save the pointers to the data stores
    p_My_Command_Data = p_command_data;
    p_My_Status_Data = p_status_data;
}

/****************************************************************************
    Public Function
        Master_LIN_Broadcast_ID

    Parameters
        uint8_t slave_id: (slave_base_id) or (slave_base_id+0x01)

    Description
        Broadcasts the appropriate LIN ID to the LIN bus

****************************************************************************/
void Master_LIN_Broadcast_ID(uint8_t slave_id)
{
    // Broadcast the LIN header
    lin_tx_header((OUR_LIN_SPEC), slave_id, 0);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        lin_id_task

    Parameters
             

    Description
        Process the LIN ID and does the appropriate task for that ID

****************************************************************************/
static void lin_id_task(void)
{
    // Create copy of ID, make sure this gives only the lower 6 bits
    uint8_t temp_id = Lin_get_id();

    // This ID matches my ID. It must be a command sent from the master.
    if (temp_id == *p_My_Node_ID)
    {
        // Prepare LIN module for receive.
        lin_rx_response((OUR_LIN_SPEC), (LIN_PACKET_LEN));
    }

    // This ID matches my ID. It must be a status request from the master.
    else if (temp_id == ((*p_My_Node_ID)|REQUEST_MASK))
    {
        // Prepare LIN module for transmit.
        // We must be a slave so My_Command_Data is LIN_PACKET_LEN bytes long only.
        lin_tx_response((OUR_LIN_SPEC), p_My_Status_Data, (LIN_PACKET_LEN));
    }

    // This ID doesn't match my ID.
    else
    {
        // If we're the master, we must have sent this ID
        if (MASTER_NODE_ID == *p_My_Node_ID)
        {
            // Prepare LIN module for transmit if we sent a command.
            if (0 == (temp_id & REQUEST_MASK))
            {
                // Make sure we send the right command based on the slave ID.
                // The master has a My_Command_Data array that is LIN_PACKET_LEN*n bytes long.
                // Where n is the number of slaves in the system.
                lin_tx_response((OUR_LIN_SPEC), Get_Pointer_To_Slave_Data(p_My_Command_Data, GET_SLAVE_NUMBER(temp_id)), (LIN_PACKET_LEN));
            }
            // Prepare LIN module for receive if we sent a request.
            else
            {
                lin_rx_response((OUR_LIN_SPEC), (LIN_PACKET_LEN));
            }
        }
        // Otherwise, we are a slave and the ID isn't for us.
        else
        {
            // Do nothing
        }
    }
}

/****************************************************************************
    Private Function
        lin_rx_task

    Parameters
             

    Description
        Get the received data

****************************************************************************/
static void lin_rx_task(void)
{
    // Copy the rx data to our appropriate data store
    // If we're the master, copy to our status array and post event
    if (MASTER_NODE_ID == *p_My_Node_ID)
    {
        // TODO: Not entirely sure if the ID is saved during the receive...
        lin_get_response(Get_Pointer_To_Slave_Data(p_My_Status_Data, GET_SLAVE_NUMBER(Lin_get_id())));

        // Post event
        Post_Event(EVT_MASTER_NEW_STS);
    }
    // If we're a slave, copy to our command array and post event
    else
    {
        // Copy command
        lin_get_response(p_My_Command_Data);

        // Post event
        Post_Event(EVT_SLAVE_NEW_CMD);
    }
}

/****************************************************************************
    Private Function
        lin_tx_task

    Parameters
             

    Description
        Updates the outgoing data if necessary

****************************************************************************/
static void lin_tx_task(void)
{
    // Update the outgoing data arrays, if necessary.
    // We don't need to do this, because we are always pulling from
    //  our real data stores.
    // (When we tx, we will just send whatever is in the data store.)

    // Do nothing.
}

/****************************************************************************
    Private Function
        lin_err_task

    Parameters
             

    Description
        Processes the LIN error

****************************************************************************/
static void lin_err_task(void)
{
    // Increment error count
    My_LIN_Error_Count++;

    // TODO: Deal with other errors accoridng to LIN 2.x spec.
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

/****************************************************************************
    ISR
        LIN Interrupt Handlers: LIN_TC_vect & LIN_ERR_vect

    Parameters
        None

    Description
        Handles LIN specific interrupts

****************************************************************************/

ISR(LIN_TC_vect)
{
    // Get interrupt cause
    switch (Lin_get_it())
    {
        // We received an ID
        case LIN_IDOK:
            lin_id_task();
            Lin_clear_idok_it();
            break;

        // We received a data packet
        case LIN_RXOK:
            lin_rx_task();
            Lin_clear_rxok_it();
            break;

        // We transmitted a data packet
        case LIN_TXOK:
            lin_tx_task();
            Lin_clear_txok_it();
            break;

        // The interrupt did not correspond to LIN
        default:
            break;
    } // End Switch
}

ISR(LIN_ERR_vect)
{
    // Get Error Status, do task, and clear int
    Lin_get_error_status();
    lin_err_task();
    Lin_clear_err_it();
}
# endif
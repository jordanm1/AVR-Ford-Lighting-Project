/*******************************************************************************
    File:
        UART_Service.c
      
    Notes:
        This file contains the main service that shall execute UART commands.
    
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

// UART Module
#include "UART.h"

// This module's header file
#include "UART_Service.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################


// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Always enter UART service with normal state
static UART_State_t Current_State = NORMAL_UART_STATE;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################


// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_UART_Service

    Parameters
        None

    Description
        Initializes the UART Service

****************************************************************************/
void Init_UART_Service(void)
{
	// Start State Machine from normal state
	Current_State = NORMAL_UART_STATE;
}

/****************************************************************************
    Public Function
        Run_UART_Service

    Parameters
        None

    Description
        Processes events for UART Message transmit/receive

****************************************************************************/
void Run_UART_Service(uint32_t event_mask)
{
	switch(Current_State)
    {	
		case NORMAL_UART_STATE:
			if (EVT_UART_START == event_mask)
			{			
                // Initialize UART for particular command
                UART_Start_Command();
				// Switch to sending state
				Current_State = SENDING_UART_STATE;
				// Post event to initiate transition
				Post_Event(EVT_UART_SEND_BYTE);
			}
            else
            {
                // Do Nothing
            }	
			break;
		
		case SENDING_STATE:
			if (EVT_UART_SEND_BYTE == event_mask)
			{
    			UART_Transmit();
			}
			else if (EVT_UART_RECV_BYTE == event_mask)
			{
                UART_Transmit();
				Current_State = RECEIVING_UART_STATE;				
			}
			else if (EVT_UART_END == event_mask)
			{
                UART_End_Command();
				Current_State = NORMAL_UART_STATE;

			}
            else
            {
                // Do Nothing
            }
			break;
		
		case RECEIVING_STATE:
            if (EVT_UART_RECV_BYTE == event_mask)
            {
                UART_Transmit();
            }
            if (EVT_UART_END == event_mask)
            {
                UART_End_Command();
                Current_State = NORMAL_UART_STATE;
            }
			break;
			
		default:
			// Do Nothing.
			break;
	}
}

/****************************************************************************
    Public Function
        Query_UART_State

    Parameters
        None

    Description
        Returns value for Current_State

****************************************************************************/

UART_State_t Query_UART_State(void)
{
    return Current_State;
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################



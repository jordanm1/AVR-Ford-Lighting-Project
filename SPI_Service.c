/*******************************************************************************
    File:
        SPI_Service.c
      
    Notes:
        This file contains the main service that shall execute SPI commands.
    
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

// SPI Module
#include "SPI.h"

// This module's header file
#include "SPI_service.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################


// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Always enter SPI service with normal state
static SPI_State_t Current_State = NORMAL_STATE;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################


// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_SPI_Service

    Parameters
        None

    Description
        Initializes the SPI Service

****************************************************************************/
void Init_SPI_Service(void)
{
	// Start State Machine from normal state
	Current_State = NORMAL_STATE;

    // Initialize SPI
    SPI_Initialize();
}

/****************************************************************************
    Public Function
        Run_SPI_Service

    Parameters
        None

    Description
        Processes events for SPI Message transmit/receive

****************************************************************************/
void Run_SPI_Service(uint32_t event_mask)
{
	switch(Current_State)
    {	
		case NORMAL_STATE:
			if (EVT_SPI_START == event_mask)
			{			
                // Initialize SPI for particular command
                SPI_Start_Command();
				// Switch to sending state
				Current_State = SENDING_STATE;
				// Post event to initiate transition
				Post_Event(EVT_SPI_SEND_BYTE);
			}
            else
            {
                // Do Nothing
            }	
			break;
		
		case SENDING_STATE:
			if (EVT_SPI_SEND_BYTE == event_mask)
			{
    			SPI_Transmit();
			}
			else if (EVT_SPI_RECV_BYTE == event_mask)
			{
                SPI_Transmit();
				Current_State = RECEIVING_STATE;				
			}
			else if (EVT_SPI_END == event_mask)
			{
                SPI_End_Command();
				Current_State = NORMAL_STATE;
			}
            else
            {
                // Do Nothing
            }
			break;
		
		case RECEIVING_STATE:
            if (EVT_SPI_RECV_BYTE == event_mask)
            {
                SPI_Transmit();
            }
            if (EVT_SPI_END == event_mask)
            {
                SPI_End_Command();
                Current_State = NORMAL_STATE;
            }
			break;
			
		default:
			// Do Nothing.
			break;
	}
}

/****************************************************************************
    Public Function
        Query_SPI_State

    Parameters
        None

    Description
        Returns value for Current_State

****************************************************************************/

SPI_State_t Query_SPI_State(void)
{
    return Current_State;
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

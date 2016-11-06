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
static SPI_State Current_State = NORMAL_STATE;

// Length of transmit and receive
static uint8_t TX_Length = 0;
static uint8_t RX_Length = 0;

// SPI buffer
static uint8_t * SPI_TX;

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
	// SPI Initialization can be moved to this location in the future.
	
	// Point SPI_TX buffer to the SPI buffer to find data lengths from element 0 and 1
	// of the buffer
	
	SPI_TX = Get_SPI_TX_Buffer();	
	
	// Start State Machine from normal state
	Current_State = NORMAL_STATE;
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
			if (EVT_SEND_BYTE == event_mask)
			{
				// Extract expected transmit length from SPI TX Buffer
				TX_Length = *(SPI_TX + TX_LNTH_INDX);
				// Extract expected receive length from SPI TX Buffer
				RX_Length = *(SPI_TX + RX_LNTH_INDX);
				// Initialize SPI Start Command
				SPI_Start_Command();				
				// Switch to sending state
				Current_State = SENDING_STATE;
				// Post event to initiate transition
				Post_Event(EVT_SEND_BYTE);
			}	
			break;
		
		case SENDING_STATE:
			if (EVT_SEND_BYTE == event_mask)
			{
				SPI_Transmit();
				Current_State = SENDING_STATE;
			}
			else if (EVT_RECV_BYTE == event_mask)
			{
				Current_State = RECEIVING_STATE;				
			}
			else if (EVT_END_TRANS == event_mask)
			{
				Current_State = NORMAL_STATE;
			}
			break;
		
		case RECEIVING_STATE:
			break;
			
		case default:
			// Do Nothing.
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

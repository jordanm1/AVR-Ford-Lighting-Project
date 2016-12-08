/*******************************************************************************
    File:
    ModemService.c
  
    Notes:
    This file contains the main service to be run on the master node.

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

// memset
#include <string.h>

// UART
#include "UART.h"

// Timer
#include "timer.h"

#include "ModemService.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// MODEM INITIALIZATION
/*
AT^SICA=1,3/r
AT^SISS=0,”srvType”,”socket”/r
AT^SISS=0,”conId”,3/r
AT^SISS=0,”address”,”socktcp://listener:2000;etx=26;autoconnect=1”/r
AT^SISO=0/r
*/

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

static uint32_t Testing_Timer = EVT_TEST_TIMEOUT;
static uint8_t Recv_Byte = 0;
static uint8_t* RX_Data[1] = {&Recv_Byte};
static uint8_t TX_Data[2] = {0xA5, 0xB5};

static bool do_init_modem = true;
static bool flipper = true;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_Master_Service

    Parameters
        None

    Description
        Initializes the master node

****************************************************************************/
void Init_Modem_Service(void)
{
   // Initialize UART
	UART_Initialize();
	Register_Timer(&Testing_Timer, Post_Event);
	Start_Timer(&Testing_Timer, 5000);
    PORTB &= ~(1<<PINB2);
    DDRB |= (1<<PINB2);
}

/****************************************************************************
    Public Function
        Run_Modem_Service

    Parameters
        None

    Description
        Processes events for the master node

****************************************************************************/
void Run_Modem_Service(uint32_t event_mask)
{
    switch(event_mask)
    {
        case EVT_TEST_TIMEOUT:
			if (do_init_modem)
			{
				Write_UART(3, 0, &TX_Data[0], &RX_Data[0], true);
				do_init_modem = false;
			}
			else
			{
				//Write_UART(2, 0, &TX_Data[0], &RX_Data[0], false);
			}
			
			if (flipper)
			{
				PORTB |= (1<<PINB2);
				flipper = false;
			}
			else
			{
				PORTB &= ~(1<<PINB2);
				flipper = true;
			}
				
			Start_Timer(&Testing_Timer, 250);
			break;

        default:
            break;
    }
}


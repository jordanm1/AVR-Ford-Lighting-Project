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

#define CAN_INIT_1_MS (200)

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
// CAN_Init_1 Timer
static uint32_t CAN_Timer = EVT_CAN_INIT_1_COMPLETE;

// Arrays to hold CAN packets
// @TODO: if the packet is short, we should host
// a previous copy to reduce compute overhead if the
// new CAN packet is the same
static uint8_t CAN_Volatile_Msg[CAN_MODEM_PACKET_LEN] = {0};
static uint8_t * a_p_CAN_Volatile_Msg[CAN_MODEM_PACKET_LEN] =
{&CAN_Volatile_Msg[0],
	&CAN_Volatile_Msg[1],
	&CAN_Volatile_Msg[2],
	&CAN_Volatile_Msg[3],
&CAN_Volatile_Msg[4]};
static uint8_t CAN_Last_Processed_Msg[CAN_MODEM_PACKET_LEN] = {0};
	
static uint32_t Testing_Timer = EVT_TEST_TIMEOUT;
static uint8_t Recv_Byte = 0;
static uint8_t* RX_Data[1] = {&Recv_Byte};
static uint8_t TX_Data[2] = {0xA5, 0xB5};
static uint8_t Modem_Recv_Data [MAX_MODEM_RECEIVE] = {0};

static bool do_init_modem = true;
static bool flipper = true;

static uint8_t TX_Away[5] = {0xa0, 0x56, 0xfd, 0x00, 0x11};

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
	UART_Initialize(Modem_Recv_Data);
	Register_Timer(&Testing_Timer, Post_Event);
	
    PORTB &= ~(1<<PINB2);
    DDRB |= (1<<PINB2);
	
	// Register CAN Init 1 timer with Post_Event()
	Register_Timer(&CAN_Timer, Post_Event);

	// Kick off CAN Init 1 Timer
	Start_Timer(&CAN_Timer, CAN_INIT_1_MS);

	// Call 1st step of the CAN initialization
	// This will only start once we exit initialization context
	CAN_Initialize_1(a_p_CAN_Volatile_Msg);
	
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

		 case EVT_CAN_INIT_1_COMPLETE:
		        // The time for CAN 1 has expired

		        // Call step two of the CAN init
		        CAN_Initialize_2();
				
				// Start Testing Timer now once we've begun the CAN 2 Init Process
				Start_Timer(&Testing_Timer, 200);
		        
		        break;
				
        case EVT_TEST_TIMEOUT:
			if (do_init_modem)
			{
				Write_UART(3, 0, &TX_Data[0], &RX_Data[0], true);
				do_init_modem = false;
			}
			else
			{
				//Write_UART(2, 0, &TX_Data[0], &RX_Data[0], false);
				CAN_Send_Message(5, TX_Away);
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
		
		case EVT_MODEM_NEW_PACKAGE:
		
			//Modem_Recv_Data =  //{CAN_MODEM_POS_TYPE, 0x00, 0x00, 0x00, 0x00};
			//write_rect_vect(&TX_Away[CAN_MODEM_POS_VECT_IDX], test_positions[test_counter]);
			CAN_Send_Message(5, Modem_Recv_Data);
			break;
		
        default:
            break;
    }
}


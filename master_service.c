/*******************************************************************************
      File:
      master_service.c
     
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

// Events
#include "events.h"

// Timer
#include "timer.h"

// This module's header file
#include "master_service.h"

// LIN top layer
#include "MS_LIN_top_layer.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Schedule Start ID
#define SCHEDULE_START_ID        0x02     // Start at 2 since ID of #1 is 0x02

// Schedule Interval
#define SCHEDULE_INTERVAL_MS     500

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// These values should only exist in a single module for each node
static uint8_t My_Node_ID = 0;                        // This node's ID
static uint8_t My_Command_Data[NUM_SLAVES*2] = {0};   // Commands for slaves
static uint8_t My_Status_Data[NUM_SLAVES*2] = {0};    // Slaves' stati

// Scheduling Timer
static uint32_t Scheduling_Timer = EVT_MASTER_SCH_TIMEOUT;

// Curr_Schedule_ID
// The schedule is simple:
//    1. Command Slave Node #1 (ID = 0x02)
//    2. Request status from Slave Node #1 (ID = 0x03)
//    3. Command Slave Node #2 (ID = 0x04)
//    4. Request status from Slave Node #2 (ID = 0x05)
//    ...
//    Y. Command Slave Node #N (ID = NUM_SLAVES*2)
//    Z. Request status from Slave Node #N (ID = (NUM_SLAVES*2)+1)
//    >>> Repeat 1-X.
static uint8_t Curr_Schedule_ID = SCHEDULE_START_ID;

// TEST
static uint8_t dc_counter = 0;
// END TEST BLOCK

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void update_curr_schedule_id(void);

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
void Init_Master_Service(void)
{
   // Set LIN ID, no need for ADC, we are the master node
   My_Node_ID = MASTER_NODE_ID;

   // Initialize the data arrays to proper things
   // TODO:

   // Initialize LIN
   MS_LIN_Initialize(&My_Node_ID, &My_Command_Data[0], &My_Status_Data[0]);

   // Register scheduling timer
   Register_Timer(&Scheduling_Timer, Post_Event);

   // Kick off scheduling timer
   Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);
}

/****************************************************************************
      Public Function
         Run_Master_Service

      Parameters
         None

      Description
         Processes events for the master node

****************************************************************************/
void Run_Master_Service(uint32_t event_mask)
{
   switch(event_mask)
   {
      case EVT_MASTER_SCH_TIMEOUT:
         // Transmit next header in schedule
         Master_LIN_Broadcast_ID(Curr_Schedule_ID);
         // Update schedule
         update_curr_schedule_id();
         // Restart timer
         Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);

         //// TEST BLOCK
		 //Start_Timer(&Scheduling_Timer, 100);
		 //Set_PWM_Duty_Cycle(LED_PWM_CHANNEL, dc_counter);
		 //if (100 == dc_counter)
		 //{
			 //dc_counter = 0;
		 //}
		 //else
		 //{
			 //dc_counter++;
		 //}
         //// END TEST BLOCK
         
         break;

      case EVT_MASTER_NEW_STS:
         // New status
         // Do nothing for now.
         break;

      case EVT_MASTER_OTHER:
         // Just an example.
         // Do nothing.
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
         update_curr_schedule_id()

      Parameters
         None

      Description
         Loops through schedule counter

****************************************************************************/
static void update_curr_schedule_id(void)
{
   // If we hit boundary condition, reset counter; otherwise increment
   if (((NUM_SLAVES<<1)+1) == Curr_Schedule_ID)
   {
      Curr_Schedule_ID = SCHEDULE_START_ID;
   }
   else
   {
      Curr_Schedule_ID++;
   }
}

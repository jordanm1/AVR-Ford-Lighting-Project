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

// This module's header file
#include "slave_service.h"

// LIN top layer
#include "MS_LIN_top_layer.h"

// Events
#include "events.h"

// Timer
#include "timer.h"

// PWM
#include "PWM.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// These values should only exist in a single module for each node
static uint8_t My_Node_ID;                         // This node's ID
static uint8_t My_Command_Data[LIN_PACKET_LEN];    // This node's current command
static uint8_t My_Status_Data[LIN_PACKET_LEN];     // This node's status

static uint8_t Last_Command = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



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

   // Initialize ADC, read slave number, create & store slave ID in RAM
   // TODO
   My_Node_ID = 0x02;

   // Disable ADC
   // TODO

   // Initialize light, (LED PWM module)
   // TODO

   // Initialize motor, (Motor PWM module)
   // TODO

   // Initialize LIN
   MS_LIN_Initialize(&My_Node_ID, &My_Command_Data[0], &My_Status_Data[0]);
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
      case EVT_SLAVE_NEW_CMD:
         // We got a new command
         if (Last_Command != My_Command_Data[0])
         {
            Set_PWM_Duty_Cycle(LED_PWM_CHANNEL, My_Command_Data[0]);
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



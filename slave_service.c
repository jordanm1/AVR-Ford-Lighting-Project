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

// Events
#include "events.h"

// LIN top layer
#include "MS_LIN_top_layer.h"

// This module's header file
#include "slave_service.h"

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
         //// TEST BLOCK
         //static uint8_t parity = 0x01;
         //parity ^= 0x01;
         //if (parity)
         //{
            //Set_PWM_Duty_Cycle(LED_PWM_CHANNEL, 100);
         //}
         //else
         //{
            //Set_PWM_Duty_Cycle(LED_PWM_CHANNEL, 0);
         //}
         //// END TEST BLOCK
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



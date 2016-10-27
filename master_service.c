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

// Framework
#include "framework.h"

// This module's header file
#include "master_service.h"

// LIN top layer
#include "MS_LIN_top_layer.h"

// Timer
#include "timer.h"

// PWM
#include "PWM.h"

// Light Setting Algorithm
#include "light_setting_alg.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Schedule Start ID
#define SCHEDULE_START_ID       0x02    // Start at 2 since ID of #1 is 0x02

// Schedule Interval
// Minimum for Interval is:
//    T_Frame_Nominal = T_Header_Nominal + T_Response_Nominal
//    T_Header_Nominal = 34*Bit_Time = 34*(1/19200)
//    T_Response_Nominal = 10*(Num_Data_Bytes+1)*Bit_time = 10*(2+1)*(1/19200)
//    T_Frame_Nominal = 0.00333333333 seconds
#define SCHEDULE_INTERVAL_MS    4

// *Note:
//      Our schedule service time is then 2*#Slave_Nodes*SCHEDULE_INTERVAL_MS

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// These values should only exist in a single module for each node
static uint8_t My_Node_ID = 0;                          // This node's ID
static uint8_t My_Command_Data[NUM_SLAVES*2] = {0};     // Commands for slaves
static uint8_t My_Status_Data[NUM_SLAVES*2] = {0};      // Slaves' stati

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

// TEST TIMER
static uint32_t Testing_Timer = EVT_TEST_TIMEOUT;
static uint8_t test_counter = 0;
static uint8_t position_counter = 1;
static uint8_t parity = 0;

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
    // TEST! Pause LIN service for now
    // Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);

    // Register test timer & start
    Register_Timer(&Testing_Timer, Post_Event);
    Start_Timer(&Testing_Timer, 500);
    PORTB &= ~(1<<PINB6);
    DDRB |= (1<<PINB6);
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
            break;

        // *Note: we should make sure all slaves are online before sending
        //      legitimate commands (blocking code?)

        case EVT_MASTER_NEW_STS:
            // New status
            // Do nothing for now.
            break;

        case EVT_MASTER_OTHER:
            // Just an example.
            // Do nothing.
            break;

        case EVT_TEST_TIMEOUT:
            // Just a test

            // TEST LIGHT SET ALG
            PORTB |= (1<<PINB6);
            slave_parameters_t test_slave;
            test_slave.fov = 100;
            test_slave.rect_position.x = 0;
            test_slave.rect_position.y = 0;
            test_slave.position_max = 0;
            test_slave.position_min = 10;
            test_slave.theta_max = 135;
            test_slave.theta_min = 45;
            rect_vect_t test_point;
            test_point.x = 0;
            test_point.y = 1;
            slave_settings_t test_slave_settings;
            test_slave_settings = Compute_Individual_Light_Settings(&test_slave, test_point);
            PORTB &= ~(1<<PINB6);

            // TEST SERVO
            // Hold servo position
            Hold_Analog_Servo_Position(position_counter);
            if ((10 == position_counter) || (0 == position_counter)) {parity ^= 1;};
            if (parity)
            {
                position_counter--;
            }
            else
            {
                position_counter++;
            }
            for (int i = 0; i < NUM_SLAVES; i++)
            {
                uint8_t temp_index = i<<1;
                My_Command_Data[temp_index] = test_counter;
                My_Command_Data[temp_index+1] = 0xFF;
            }

            // TEST PWM
            Set_PWM_Duty_Cycle(pwm_channel_b, test_counter);
            test_counter++;
            if (100 < test_counter) {test_counter = 0;};
            Start_Timer(&Testing_Timer, 500);
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

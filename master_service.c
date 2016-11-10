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

// memset
#include <string.h>

// LIN top layer
#include "MS_LIN_top_layer.h"

// Timer
#include "timer.h"

// PWM
#include "PWM.h"

// Light Setting Algorithm
#include "light_setting_alg.h"

// Slave Parameters
#include "slave_parameters.h"

// SPI 
#include "SPI.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Master array length
#define MASTER_DATA_LENGTH      (NUM_SLAVES*LIN_PACKET_LEN)

// Schedule Start/End IDs
#define SCHEDULE_START_ID       (GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER))
#define SCHEDULE_END_ID         (GET_SLAVE_BASE_ID(HIGHEST_SLAVE_NUMBER)|REQUEST_MASK)

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
static uint8_t My_Node_ID = 0;                              // This node's ID
static uint8_t My_Command_Data[MASTER_DATA_LENGTH] = {0};   // Commands for slaves
static uint8_t My_Status_Data[MASTER_DATA_LENGTH] = {0};    // Slaves' stati

// Scheduling Timer
static uint32_t Scheduling_Timer = NON_EVENT;

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
#define NUM_TEST_POSITIONS          5
static rect_vect_t test_positions[NUM_TEST_POSITIONS] = {
                                                        {.x = 5, .y = -4},
                                                        {.x = 5, .y = -2},
                                                        {.x = 5, .y = 0},
                                                        {.x = 5, .y = 2},
                                                        {.x = 5, .y = 4},
                                                        };
														
// SPI Test									
static uint8_t SPI_TX[3] = {0x0A, 0x02, 0x07};
static uint8_t Recv1 = 0;
static uint8_t Recv2 = 0;
uint8_t* RecvList[2];

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void ID_schedule_handler(uint32_t unused);           // Called from int context
static void update_curr_schedule_id(void);
static void clear_cmds(void);
static void update_cmds(rect_vect_t requested_location);
static bool did_single_slave_obey(uint8_t slave_number);
static bool did_all_slaves_obey(void);
static void put_LIN_to_sleep(void);

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
    clear_cmds();

    // Initialize LIN
    MS_LIN_Initialize(&My_Node_ID, &My_Command_Data[0], &My_Status_Data[0]);
	
	// Initialize SPI
	MS_SPI_Initialize(&My_Node_ID);

    // Register scheduling timer with ID_schedule_handler as 
    //      callback function
    Register_Timer(&Scheduling_Timer, ID_schedule_handler);

    // Kick off scheduling timer
    Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);

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

        // *Note: we should make sure all slaves are online before sending
        //      legitimate commands (blocking code?)

        case EVT_MASTER_NEW_STS:
            // New status

            #if 0
            // Check for slave obedience
            if (false == did_all_slaves_obey())
            {
                // Stop sending ID's, go to sleep
                put_LIN_to_sleep();
            }
            #endif

            break;
		
        case EVT_MASTER_OTHER:
            // Just an example.
            // Do nothing.
            break;

        case EVT_TEST_TIMEOUT:
			//RecvList[0] = &Recv1;
			//RecvList[1] = &Recv2;
			//// SPI Test
			//
			//Write_SPI(3, 2, SPI_TX, &RecvList[0]);
			//SPI_Transmit();
			Start_Timer(&Testing_Timer, 500);
		
		
            // Just a test

            //// TEST SERVO
            //// Hold servo position
            //Hold_Analog_Servo_Position(position_counter);
            //if ((10 == position_counter) || (0 == position_counter)) {parity ^= 1;};
            //if (parity)
            //{
                //position_counter--;
            //}
            //else
            //{
                //position_counter++;
            //}
            //for (int i = 0; i < NUM_SLAVES; i++)
            //{
                //uint8_t temp_index = i<<1;
                //My_Command_Data[temp_index] = test_counter;
                //My_Command_Data[temp_index+1] = 0xFF;
            //}
//
            //// TEST PWM
            //Set_PWM_Duty_Cycle(pwm_channel_b, test_counter);
            //test_counter++;
            //if (100 < test_counter) {test_counter = 0;};
            //Start_Timer(&Testing_Timer, 500);

            // Not yet
            // Hold_Analog_Servo_Position(10);
            #if 1
            parity ^= 1;
            if (parity)
            {
                PORTB |= (1<<PINB6);
            }
            else
            {
                PORTB &= ~(1<<PINB6);
            }
            Start_Timer(&Testing_Timer, 500);
            // EXAMPLE FOR NEW_REQ_LOCATION over CAN
//             // Reset the schedule counter
//             Curr_Schedule_ID = SCHEDULE_START_ID;
//             // Reset our commands to NON_COMMANDs
//             //      (will be ignored by the slaves)
//             clear_cmds();
//             // Start transmitting headers
//             Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);
            // Begin updating the commands, which will
            //      be sent in the background
//             PORTB |= (1<<PINB6);
//             update_cmds(test_positions[test_counter]);
//             test_counter++;
//             if (NUM_TEST_POSITIONS <= test_counter) test_counter = 0;
//             PORTB &= ~(1<<PINB6);
            // *Note: While we are sending, we will
            //      check to see if the slaves have
            //      obeyed whenever we receive a
            //      new status.
            #endif

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
        ID_schedule_handler()

    Parameters
        None

    Description
        Handles sending the next ID within interrupt context (called from
        the timer expired interrupt)

****************************************************************************/
static void ID_schedule_handler(uint32_t unused)
{
    // Transmit next header in schedule
    Master_LIN_Broadcast_ID(Curr_Schedule_ID);
    // Update schedule id
    update_curr_schedule_id();
    // Restart timer
    Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);
}

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
    if (SCHEDULE_END_ID == Curr_Schedule_ID)
    {
        Curr_Schedule_ID = SCHEDULE_START_ID;
    }
    else
    {
        Curr_Schedule_ID++;
    }
}

/****************************************************************************
    Private Function
       clear_cmds()

    Parameters
        None

    Description
        Updates all commands for slaves to be NON_COMMANDs

****************************************************************************/
static void clear_cmds(void)
{
    // Clear all of our commands
    memset(&My_Command_Data, NON_COMMAND, MASTER_DATA_LENGTH);
}

/****************************************************************************
    Private Function
        update_cmds()

    Parameters
        None

    Description
        Updates all commands for slaves based on requested light location

****************************************************************************/
static void update_cmds(rect_vect_t requested_location)
{
    // temp_id for readability
    uint8_t temp_id;

    // p_temp_cmd
    uint8_t * p_temp_cmd;

    // Loop through all slaves
    for (int slave_num = LOWEST_SLAVE_NUMBER; slave_num <= NUM_SLAVES; slave_num++)
    {
        // Get slave base id for slave num
        temp_id = GET_SLAVE_BASE_ID(slave_num);
        // Compute the pointer to the correct location in command array
        p_temp_cmd = (&(My_Command_Data[0]))+temp_id-SCHEDULE_START_ID;
        // Run algorithm to compute the individual light settings
        Compute_Individual_Light_Settings(Get_Slave_Parameters(temp_id), p_temp_cmd, requested_location);
    }
}

/****************************************************************************
    Private Function
        did_single_slave_obey()

    Parameters
        None

    Description
        Checks to see if a particular slave has received and executed
        the command previously commanded from the master.

****************************************************************************/
static bool did_single_slave_obey(uint8_t slave_number)
{
    // Ensure the slave_number is within slave bounds
    if ((LOWEST_SLAVE_NUMBER > slave_number) || (HIGHEST_SLAVE_NUMBER < slave_number))
    {
        // Return false
        return false;
    }

    // Check byte by byte, memcmp won't work b/c we ignore non-commands

    // IF The slave's intensity doesn't match my commanded intensity AND my command was valid
    // (if the command we sent was a NON_COMMAND, then we don't care about the slave's status)
    if (    (My_Command_Data[GET_SLAVE_BASE_ID(slave_number)-GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER)]
            != My_Status_Data[GET_SLAVE_BASE_ID(slave_number)-GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER)])
            &&
            (My_Command_Data[GET_SLAVE_BASE_ID(slave_number)-GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER)]
            != NON_COMMAND)
       )
    {
        // The slave has not obeyed my intensity command, return false
        return false;
    }
    
     // IF The slave's position doesn't match my commanded position AND my command was valid
     // (if the command we sent was a NON_COMMAND, then we don't care about the slave's status)
     if (    (My_Command_Data[GET_SLAVE_BASE_ID(slave_number)-GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER)+1]
             != My_Status_Data[GET_SLAVE_BASE_ID(slave_number)-GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER)+1])
             &&
             (My_Command_Data[GET_SLAVE_BASE_ID(slave_number)-GET_SLAVE_BASE_ID(LOWEST_SLAVE_NUMBER)+1]
             != NON_COMMAND)
        )
     {
        // The slave has not obeyed my position command, return false
        return false;
     }

     // IF none of the cases above were satisfied, then the slave obeyed my commands
     return true;
}

/****************************************************************************
    Private Function
        did_all_slaves_obey()

    Parameters
        None

    Description
        Checks if all of the slaves have obeyed

****************************************************************************/
static bool did_all_slaves_obey(void)
{
    // Loop through all slaves
    for (int slave_num = 1; slave_num <= NUM_SLAVES; slave_num++)
    {
        // Check individual slave for obedience
        if (false == did_single_slave_obey(slave_num))
        {
            return false;
        }
    }

    // Return true if all slaves obeyed
    return true;
}

/****************************************************************************
    Private Function
        put_LIN_to_sleep()

    Parameters
        None

    Description
        Puts the LIN bus to sleep by stopping LIN headers from being sent
        from the master

****************************************************************************/
static void put_LIN_to_sleep(void)
{
    // Stop the scheduling timer
    Stop_Timer(&Scheduling_Timer);

    // @TODO: More housekeeping to put the bus to sleep
}
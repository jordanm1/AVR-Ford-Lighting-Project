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

// CAN layer
#include "CAN.h"

// Command and Status Helpers
#include "cmd_sts_helpers.h"

// Timer
#include "timer.h"

// PWM
#include "PWM.h"

// Light Setting Algorithm
#include "light_setting_alg.h"

// Slave Parameters
#include "slave_parameters.h"

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
//    T_Response_Nominal = 10*(Num_Data_Bytes+1)*Bit_time = 10*(3+1)*(1/19200)
//    T_Frame_Nominal = 0.00385 seconds
#define SCHEDULE_INTERVAL_MS    (5)             // Because our system timer
                                                //  has resolution of 0.5 ms

// *Note:
//      Our schedule service time is then 2*#Slave_Nodes*SCHEDULE_INTERVAL_MS

// Time in ms it takes to complete the CAN step 1 initializations
#define CAN_INIT_1_MS           (200)

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// These values should only exist in a single module for each node
static uint8_t My_Node_ID = 0;                              // This node's ID
static uint8_t My_Command_Data[MASTER_DATA_LENGTH] = {0};   // Commands for slaves
static uint8_t My_Status_Data[MASTER_DATA_LENGTH] = {0};    // Slaves' stati
static uint8_t * p_My_Command_Data = My_Command_Data;
static uint8_t * p_My_Status_Data = My_Status_Data;

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

// CAN_Init_1 Timer
static uint32_t CAN_Init_1_Timer = EVT_CAN_INIT_1_COMPLETE;

// Arrays to hold CAN packets
// @TODO: if the packet is short, we should host
// a previous copy to reduce compute overhead if the
// new CAN packet is the same
static uint8_t CAN_Message[5] = {0};

// TEST TIMER
static uint32_t Testing_Timer = EVT_TEST_TIMEOUT;
static uint16_t test_counter = 0;
static uint8_t up_count = 1;
static uint16_t position_counter = 1;
static uint8_t parity = 0;
#define NUM_TEST_POSITIONS          8
static rect_vect_t test_positions[NUM_TEST_POSITIONS] = {
                                                        {.x = 0, .y = -100},
                                                        {.x = 70, .y = -70},
                                                        {.x = 100, .y = 0},
                                                        {.x = 70, .y = 70},
                                                        {.x = 0, .y = 100},
                                                        {.x = -70, .y = 70},
                                                        {.x = -100, .y = 0},
                                                        {.x = -70, .y = -70},
                                                        };
static position_data_t position_to_watch;
static intensity_data_t intensity_to_watch;

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
    MS_LIN_Initialize(&My_Node_ID, p_My_Command_Data, p_My_Status_Data);

    // Register scheduling timer with ID_schedule_handler as 
    //      callback function
    Register_Timer(&Scheduling_Timer, ID_schedule_handler);

    // Kick off scheduling timer
    Start_Timer(&Scheduling_Timer, SCHEDULE_INTERVAL_MS);

    // Register CAN Init 1 timer with Post_Event()
    Register_Timer(&CAN_Init_1_Timer, Post_Event);

    // Kick off CAN Init 1 Timer
    Start_Timer(&CAN_Init_1_Timer, CAN_INIT_1_MS);

    // Call 1st step of the CAN initialization
    // This will only start once we exit initialization context
    CAN_Initialize_1();

    // Register test timer & start
    Register_Timer(&Testing_Timer, Post_Event);
    Start_Timer(&Testing_Timer, 5000);
    //Set_PWM_Duty_Cycle(pwm_channel_a, 10);
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

        case EVT_CAN_INIT_1_COMPLETE:
            // The time for CAN 1 has expired

            // Call step two of the CAN init
            CAN_Initialize_2();

            // Do not restart the timer!
            
            break;

        case EVT_MASTER_NEW_CAN_MSG:
            // We received a new CAN message

            // Check to see if the message is different than
            // our current instance of the message
            // @TODO
            //  IF CAN MSG TYPE == USER_REQ_LIGHT_POS
            //      Call command to run algo on all slave nodes
            //  @TODO
            //  IF CAN MSG TYPE == SLAVE_CMD_PACKET
            //      Update only this slave's command
        
            break;

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
            // Just a test
            
            // Restart test timer
            Start_Timer(&Testing_Timer, 1000);

            // TEST PWM
            Set_PWM_Duty_Cycle(pwm_channel_a, 80);
            Hold_Analog_Servo_Position(750+position_counter);
            if ((1 == position_counter) || (4 == position_counter)) {up_count ^= 1;};
            if (up_count)
            {
                position_counter--;
            }
            else
            {
                position_counter++;
            }

            //#if 0
            //parity ^= 1;
            //if (parity)
            //{
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),2250);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 2),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 2),2250);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 3),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 3),2250);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 4),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 4),2250);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 5),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 5),2250);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 6),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 6),2250);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 7),75);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 7),2250);
            //}
            //else
            //{
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),1450);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 2),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 2),1450);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 3),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 3),1450);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 4),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 4),1450);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 5),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 5),1450);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 6),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 6),1450);
                //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 7),0);
                //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 7),1450);
            //}
            //#endif

            #if 1
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
//             Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1), 98);
//             Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1), 1589);
            update_cmds(test_positions[test_counter]);
            //Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),50);
            //Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),2250);
            //position_to_watch = Get_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1));
            //intensity_to_watch = Get_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1));
            test_counter++;
            if (NUM_TEST_POSITIONS <= test_counter) test_counter = 0;
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
    // Loop through all slaves
    for (int slave_num = LOWEST_SLAVE_NUMBER; slave_num <= NUM_SLAVES; slave_num++)
    {
        // Write non-commands
        Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), INTENSITY_NON_COMMAND);
        Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), POSITION_NON_COMMAND);
    }
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
    // Loop through all slaves
    for (int slave_num = LOWEST_SLAVE_NUMBER; slave_num <= NUM_SLAVES; slave_num++)
    {
        // Run algorithm to compute the individual light settings
        Compute_Individual_Light_Settings(Get_Pointer_To_Slave_Parameters(slave_num),
            Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), requested_location);
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
    if (    (Get_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Status_Data, slave_number))
            != Get_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_number)))
            &&
            (INTENSITY_NON_COMMAND != Get_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_number)))
       )
    {
        // The slave has not obeyed my intensity command, return false
        return false;
    }
    
     // IF The slave's position doesn't match my commanded position AND my command was valid
     // (if the command we sent was a NON_COMMAND, then we don't care about the slave's status)
     if (    (Get_Position_Data(Get_Pointer_To_Slave_Data(p_My_Status_Data, slave_number))
             != Get_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_number)))
             &&
             (POSITION_NON_COMMAND != Get_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_number)))
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
    for (int slave_num = LOWEST_SLAVE_NUMBER; slave_num <= NUM_SLAVES; slave_num++)
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
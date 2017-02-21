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

// Atomic Read/Write operations
#include <util/atomic.h>

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

// Time interval that passes between polling our volatile instance of the CAN
// message
#define CAN_POLL_INTERVAL_MS    (50)

// MEMCMP equal
#define MEMCMP_EQUAL            (0)

// System State On/Off
#define SYSTEM_ENABLED          (1)
#define SYSTEM_DISABLED         (0)

// Animation States & Time Intervals
#define ANIM_STATE_OFF          (0xFF)
#define ANIM_STATE_RADIAL       (0)
#define ANIM_STATE_HONING       (1)
#define ANIM_STATE_BLINK        (2)
#define ANIM_STATE_SYSOFF       (3)
#define ANIM_TRANSITION_MS      (10)
#define ANIM_HONING_INTERVAL_MS (15)
#define ANIM_BLINK_INTERVAL_MS  (200)

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
static rect_vect_t Last_Processed_User_Position = {0};

// State of whether we allow processing of CAN messages
static uint8_t System_State = SYSTEM_DISABLED;

// Testing Variables
#if 0
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
#endif

// ANIMATION VARIABLES
static uint32_t Animation_Timer = EVT_ANIM_TIMEOUT;
#define NUM_ANIM_RADIAL_POSITIONS 8                                                       
static rect_vect_t Anim_Radial_Positions[NUM_ANIM_RADIAL_POSITIONS] = {
    {.x = 0, .y = -100},
    {.x = 70, .y = -70},
    {.x = 100, .y = 0},
    {.x = 70, .y = 70},
    {.x = 0, .y = 100},
    {.x = -70, .y = 70},
    {.x = -100, .y = 0},
    {.x = -70, .y = -70},
};
static uint8_t Anim_Radial_Position_Index = 0;    
static uint8_t Anim_Radial_Num_Cycles = 0;
static intensity_data_t Anim_Honing_Intensity = 0;
static bool Anim_Honing_Count_Up = true;
static uint8_t Anim_Honing_Num_Cycles = 0;
static uint8_t Anim_Blink_On_Off = 0;
static uint8_t Anim_Blink_Num_Cycles = 0;
static uint8_t Animation_State = 0;

// Variables for debugging purposes
static rect_vect_t vect_2_watch = {0};
static position_data_t position_2_watch;
static intensity_data_t intensity_2_watch;

// Upstream CAN Testing Node Variables
// Message to send out
static uint8_t Upstream_Test_Message[5] = {0};
// Index to keep track of what message we should send out
static uint8_t Upstream_Test_Message_Index = 0;
// Number of upstream can test messages
#define NUM_UPSTREAM_TEST_MESSAGES 11
#define UPSTREAM_HONING_MESSAGE_INDEX 8
#define UPSTREAM_BLINK_MESSAGE_INDEX 9
#define UPSTREAM_SYSOFF_MESSAGE_INDEX 10

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void ID_schedule_handler(uint32_t unused);           // Called from int context
static void update_curr_schedule_id(void);
static void clear_cmds(void);
static void update_cmds(rect_vect_t requested_location);
static void write_all_off_cmds(void);
static void write_all_data_cmds(intensity_data_t intensity, position_data_t position);
static bool did_single_slave_obey(uint8_t slave_number);
static bool did_all_slaves_obey(void);
static void put_LIN_to_sleep(void);
static rect_vect_t get_CAN_pos_vect(void);
static void write_rect_vect(uint8_t * p_target, rect_vect_t vect);
static intensity_data_t get_CAN_spec_intensity_data(void);
static position_data_t get_CAN_spec_position_data(void);

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
    Register_Timer(&CAN_Timer, Post_Event);

    // Kick off CAN Init 1 Timer
    Start_Timer(&CAN_Timer, CAN_INIT_1_MS);

    // Call 1st step of the CAN initialization
    // This will only start once we exit initialization context
    CAN_Initialize_1(a_p_CAN_Volatile_Msg);

    // Read state of system switch
    System_State = SYSTEM_DISABLED;
    if ((1<<PINB4) == (PINB & (1<<PINB4))) System_State = SYSTEM_ENABLED;

    // Register test timer
    Register_Timer(&Animation_Timer, Post_Event);
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

            // Change the Event type that the CAN timer will post
            CAN_Timer = EVT_CAN_POLLING_TIMEOUT;

            // Start the CAN timer which will now be used to poll our CAN msg var
            Start_Timer(&CAN_Timer, CAN_INIT_1_MS);
            
            break;

        case EVT_CAN_POLLING_TIMEOUT:
            // The CAN polling timer has expired
            // We have to poll the CAN message, because we don't yet have a system
            // implemented to know when we truly have received a CAN message

            // Restart the CAN polling timer
            Start_Timer(&CAN_Timer, CAN_POLL_INTERVAL_MS);

            // Set new message flag to false
            ;
            bool new_msg = false;

            // Enter critical section so the message we check is the same as the message we copy
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                // Check to see if the volatile instance of the message is 
                // different than the last processed instance of the message
                if (MEMCMP_EQUAL != memcmp(&CAN_Last_Processed_Msg, &CAN_Volatile_Msg, CAN_MODEM_PACKET_LEN))
                {
                    // The message is new. Process the message.
                
                    // If the ID is something we expect...
                    switch (CAN_Volatile_Msg[CAN_MODEM_TYPE_IDX])
                    {
                        case CAN_MODEM_POS_TYPE:
                        case CAN_MODEM_SPEC_TYPE:
                        case CAN_ANIM_HONING_TYPE:
                        case CAN_ANIM_BLINK_TYPE:
                        case CAN_ANIM_SYSOFF_TYPE:
                            // Copy the message
                            // @TODO: This might need to be in a critical section
                            memcpy(&CAN_Last_Processed_Msg, &CAN_Volatile_Msg, CAN_MODEM_PACKET_LEN);
                            // Set flag to alert us to continue
                            new_msg = true;
                            break;

                        default:
                            break;
                    }
                }
            }

            // If we have a new message and the system is enabled
            if (new_msg && (SYSTEM_ENABLED == System_State))
            {
                // Process based on the message type
                switch (CAN_Last_Processed_Msg[CAN_MODEM_TYPE_IDX])
                {
                    case CAN_MODEM_POS_TYPE:
                        // Stop the animation, just in case it was running
                        Animation_State = ANIM_STATE_OFF;
                        Stop_Timer(&Animation_Timer);
                        // Set last processed location
                        Last_Processed_User_Position = get_CAN_pos_vect();
                        // Run light setting algo for all slave nodes
                        update_cmds(Last_Processed_User_Position);
                        break;

                    case CAN_MODEM_SPEC_TYPE:
                        // Stop the animation, just in case it was running
                        Animation_State = ANIM_STATE_OFF;
                        Stop_Timer(&Animation_Timer);
                        // Update the command for only the slave specified
                        Write_Intensity_Data(   Get_Pointer_To_Slave_Data(p_My_Command_Data, CAN_Last_Processed_Msg[CAN_MODEM_SPEC_NUM_IDX]),
                                                get_CAN_spec_intensity_data()
                                                );
                        Write_Position_Data(    Get_Pointer_To_Slave_Data(p_My_Command_Data, CAN_Last_Processed_Msg[CAN_MODEM_SPEC_NUM_IDX]),
                                                get_CAN_spec_position_data()
                                                );
                        break;

                    case CAN_ANIM_HONING_TYPE:
                        // Begin the honing animation
                        Anim_Honing_Count_Up = true;
                        Anim_Honing_Intensity = 0;
                        Animation_State = ANIM_STATE_HONING;
                        Start_Timer(&Animation_Timer, ANIM_TRANSITION_MS);
                        break;

                    case CAN_ANIM_BLINK_TYPE:
                        // Begin the blink animation
                        Anim_Blink_On_Off = 0;
                        Animation_State = ANIM_STATE_BLINK;
                        Start_Timer(&Animation_Timer, ANIM_TRANSITION_MS);
                        break;

                    case CAN_ANIM_SYSOFF_TYPE:
                        // Begin all off animation
                        Animation_State = ANIM_STATE_SYSOFF;
                        Start_Timer(&Animation_Timer, ANIM_TRANSITION_MS);
                        break;

                    default:
                        break;
                }
            }
        
            break;

        case EVT_BTN_SYS_ON:
            // Set system state to be enabled
            // At this point, the system is awaiting a new CAN message
            System_State = SYSTEM_ENABLED;
            break;

        case EVT_BTN_SYS_OFF:
            // Set system state to be disabled
            System_State = SYSTEM_DISABLED;
            // Stop the animation timer
            Stop_Timer(&Animation_Timer);
            // Write commands to turn off all lights
            write_all_off_cmds();
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

        case EVT_ANIM_TIMEOUT:
            // The animation timer has expired.
            // Execute the next animation step.

            switch (Animation_State)
            {
                #if 0 // Comment out because we no longer support the radial animations
                // Radial Animation
                case ANIM_STATE_RADIAL:
                    update_cmds(Anim_Radial_Positions[Anim_Radial_Position_Index]);
                    Anim_Radial_Position_Index++;
                    if (0 == (Anim_Radial_Position_Index%NUM_ANIM_RADIAL_POSITIONS)) Anim_Radial_Num_Cycles++;
                    Anim_Radial_Position_Index %= NUM_ANIM_RADIAL_POSITIONS;
                    if (2 <= Anim_Radial_Num_Cycles)
                    {
                        Animation_State += 1;
                        Anim_Radial_Position_Index = 0;
                        Anim_Radial_Num_Cycles = 0;
                    }
                    Start_Timer(&Animation_Timer, 1000);
                    break;
                #endif

                // Honing Animation
                case ANIM_STATE_HONING:
                    write_all_data_cmds(Anim_Honing_Intensity, 1450);
                    // Overwrite positions only for the 1 and 9 because they are special nodes (360 deg capability)
                    Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 9),1295);
                    Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),1295);
                    if (Anim_Honing_Count_Up)
                    {
                        Anim_Honing_Intensity += 1;
                    }
                    else
                    {
                        Anim_Honing_Intensity -= 1;
                    }
                    if ((0 == Anim_Honing_Intensity) || (100 == Anim_Honing_Intensity))
                    {
                        Anim_Honing_Count_Up ^= 1;
                        Anim_Honing_Num_Cycles += 1;
                    }
                    #if 0 // Comment this out because we want to allow the animation to continue indefinitely
                    if (16 == Anim_Honing_Num_Cycles)
                    {
                        Animation_State += 1;
                        Anim_Honing_Intensity = 0;
                        Anim_Honing_Num_Cycles = 0;
                    }
                    #endif
                    Start_Timer(&Animation_Timer, ANIM_HONING_INTERVAL_MS);
                    break;

                // Blink Animation
                case ANIM_STATE_BLINK:
                    if (Anim_Blink_On_Off)
                    {
                        write_all_data_cmds(0, 1450);
                    }
                    else
                    {
                        write_all_data_cmds(75, 1450);
                    }
                    Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 9),1295);
                    Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, 1),1295);
                    Anim_Blink_On_Off ^= 1;
                    Anim_Blink_Num_Cycles += 1;
                    #if 0 // Comment this out because we want to allow the animation to continue indefinitely
                    if (100 == Anim_Blink_Num_Cycles)
                    {
                        Animation_State += 1;
                        Anim_Blink_On_Off = 0;
                        Anim_Blink_Num_Cycles = 0;
                    }
                    #endif
                    Start_Timer(&Animation_Timer, ANIM_BLINK_INTERVAL_MS);
                    break;

                case ANIM_STATE_SYSOFF:
                    // Write commands to turn off all lights
                    write_all_off_cmds();
                    break;

                default:
                    // Do nothing. Unsupported animation type
                    break;
            }

            break;
        
        // Allow the following to compile only if this build is intended
        // to run on the CAN upstream testing node
        #if 0
        case EVT_UPSTREAM_CAN_NODE_BTN_PRESS:
            // We got a button press, so increment through the positions,
            // or if on last position, increment through the animation types

            // Increment the message index type
            Upstream_Test_Message_Index++;
            if (NUM_UPSTREAM_TEST_MESSAGES <= Upstream_Test_Message_Index)
                Upstream_Test_Message_Index = 0;
            
            // Based on our message index, construct the appropriate CAN message
            if (NUM_ANIM_RADIAL_POSITIONS > Upstream_Test_Message_Index)
            {
                Upstream_Test_Message[CAN_MODEM_TYPE_IDX] = CAN_MODEM_POS_TYPE;
                write_rect_vect(&Upstream_Test_Message[CAN_MODEM_POS_VECT_IDX], Anim_Radial_Positions[Upstream_Test_Message_Index]);
            }
            else if (UPSTREAM_HONING_MESSAGE_INDEX == Upstream_Test_Message_Index)
            {
                // Just set the header, the master node will just ignore the rest
                Upstream_Test_Message[CAN_MODEM_TYPE_IDX] = CAN_ANIM_HONING_TYPE;
            }
            else if (UPSTREAM_BLINK_MESSAGE_INDEX == Upstream_Test_Message_Index)
            {
                // Just set the header, the master node will just ignore the rest
                Upstream_Test_Message[CAN_MODEM_TYPE_IDX] = CAN_ANIM_BLINK_TYPE;
            }
            else if (UPSTREAM_SYSOFF_MESSAGE_INDEX == Upstream_Test_Message_Index)
            {
                // Just set the header, the master node will just ignore the rest
                Upstream_Test_Message[CAN_MODEM_TYPE_IDX] = CAN_ANIM_SYSOFF_TYPE;
            }
            else
            {
                // Should never occur. Do nothing.
            }

            // Send the CAN message
            CAN_Send_Message(CAN_MODEM_PACKET_LEN, Upstream_Test_Message);

            break;
        #endif

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
       write_all_off_cmds()

    Parameters
        None

    Description
        Updates all commands for slaves to be LIGHT_OFF

****************************************************************************/
static void write_all_off_cmds(void)
{
    // Loop through all slaves
    for (int slave_num = LOWEST_SLAVE_NUMBER; slave_num <= NUM_SLAVES; slave_num++)
    {
        // Write non-commands
        Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), LIGHT_OFF);
        Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), POSITION_NON_COMMAND);
    }
}

/****************************************************************************
    Private Function
       write_all_data_cmds()

    Parameters
        None

    Description
        Updates commands for all nodes based on requested intensity and position

****************************************************************************/
static void write_all_data_cmds(intensity_data_t intensity, position_data_t position)
{
    // Loop through all slaves
    for (int slave_num = LOWEST_SLAVE_NUMBER; slave_num <= NUM_SLAVES; slave_num++)
    {
        // Write non-commands
        Write_Intensity_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), intensity);
        Write_Position_Data(Get_Pointer_To_Slave_Data(p_My_Command_Data, slave_num), position);
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

/****************************************************************************
    Private Function
        get_CAN_pos_vect

    Parameters
        None

    Description
        This returns an instance of the rect vect from a CAN user position 
        frame

****************************************************************************/
static rect_vect_t get_CAN_pos_vect(void)
{
    // Init result position as the origin (all zeros)
    rect_vect_t result = {0};
    // Ensure the CAN message type is a position type
    if (CAN_MODEM_POS_TYPE == CAN_Last_Processed_Msg[CAN_MODEM_TYPE_IDX])
    {
        // Copy the vector info from CAN message into result
        memcpy(&result, &CAN_Last_Processed_Msg[CAN_MODEM_POS_VECT_IDX], sizeof(result));
    }
    // Return result
    return result;
}

/****************************************************************************
    Private Function
        write_rect_vect

    Parameters
        None

    Description
        This writes the vect info to the location of target

****************************************************************************/
static void write_rect_vect(uint8_t * p_target, rect_vect_t vect)
{
    // Copy the vector into the location provided by the caller
    memcpy(p_target, &vect, sizeof(vect));
}

/****************************************************************************
    Private Function
        get_CAN_spec_intensity_data

    Parameters
        None

    Description
        This returns an instance of the intensity data from a CAN slave spec 
        frame

****************************************************************************/
static intensity_data_t get_CAN_spec_intensity_data(void)
{
    // Ensure the CAN message type is a slave specific type
    if (CAN_MODEM_SPEC_TYPE == CAN_Last_Processed_Msg[CAN_MODEM_TYPE_IDX])
    {
        // Return the intensity data only
        return Get_Intensity_Data(&CAN_Last_Processed_Msg[CAN_MODEM_SPEC_CMD_INDEX]);
    }
    // Return non command if the message type isn't a slave spec frame
    return INTENSITY_NON_COMMAND;
}

/****************************************************************************
    Private Function
        get_CAN_spec_position_data

    Parameters
        None

    Description
        This returns an instance of the position data from a CAN slave spec 
        frame

****************************************************************************/
static position_data_t get_CAN_spec_position_data(void)
{
    // Ensure the CAN message type is a slave specific type
    if (CAN_MODEM_SPEC_TYPE == CAN_Last_Processed_Msg[CAN_MODEM_TYPE_IDX])
    {
        // Return the position data only
        return Get_Position_Data(&CAN_Last_Processed_Msg[CAN_MODEM_SPEC_CMD_INDEX]);
    }
    // Return non command if the message type isn't a slave spec frame
    return POSITION_NON_COMMAND;
}

/*******************************************************************************
    File:
        config.h

    Notes:
        This file contains the configuration settings for the master and slave
        nodes of the 360 Lighting System.

    External Functions Required:

    Public Functions:
        
*******************************************************************************/

#ifndef config_H
#define config_H

// #############################################################################
// ------------ SYSTEM SETTINGS
// #############################################################################

#define NUM_SLAVES          2

// #############################################################################
// ------------ NODE SETTINGS
// #############################################################################

#define IS_MASTER_NODE      YES

// #############################################################################
// ------------ INCLUDES
// #############################################################################

// AVR Library
#include <avr/io.h>                         // Use AVR-GCC library

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>

// #############################################################################
// ------------ LIN ID MAP
// #############################################################################

// Define the node id's that each slave will respond to.
// !!! The master ID is always 0x00. !!!
//
// The ID's are defined in the following way,
//      Each slave has two ID's it handles:
//          One ID for receiving commands and one ID for sending commands.
//      The ID for receiving commands will be an even number,
//          and the ID for sending commands will be the next integer above it.
//      The two ID's when AND'd with ~(0x01) correspond to one slave's ID.
//      We will define ID's for 16 possible slave nodes,
//          this leaves 60-16 id's left on the network.
//    
//      The formula for a slave node's base ID is as follows:
//          slave_base_id = (slave_number)*2
//          where the slave_number must be an integer >= 1.
//
//      *Note: We will always service the slave nodes in numerical order,
//          starting with slave number one (slave_base_id = 0x02)
//
//      There are 0-59 possible ID's under LIN 2.x
//      We reserve 0-1 for the Master, so we have
//          2-59 ID's left (58 unique ID's)
//      We divide 58 by 2 to get the maximum number
//          of slaves in this system as 29.

// Master ID
#define MASTER_NODE_ID          (0x00)          // Master is the first ID

// Max number of slaves
#define MAX_NUM_SLAVES          (29)

// First slave number
#define LOWEST_SLAVE_NUMBER     (0x01)
#define HIGHEST_SLAVE_NUMBER    (NUM_SLAVES)
#define INVALID_SLAVE_NUMBER    (0xFF) 

// Request Mask (the LSB will be high for status requests)
#define REQUEST_MASK            (0x01)
#define SLAVE_BASE_MASK         ~(0x01)

// Define a macro to get the slave number based on ID
// *Note: GET_SLAVE_NUMBER is agnostic to whether the ID is a command or request,
//      the last bit is removed in either case so the number ends up being the 
//      same
#define GET_SLAVE_NUMBER(slave_id)              (slave_id>>1)
#define GET_SLAVE_BASE_ID(slave_number)         (slave_number<<1)

// #############################################################################
// ------------ COMMANDS AND STATI
// #############################################################################

// Command/status packet byte indices and lengths
#define LIN_PACKET_LEN          (3)                 // number of bytes in packet
#define INTENSITY_DATA_INDEX    (0)                 // Start index for this command
#define POSITION_DATA_INDEX     (1)                 // Start index for this command
#define INTENSITY_DATA_LEN      (1)                 // Number of bytes
#define POSITION_DATA_LEN       (2)                 // Number of bytes
typedef uint8_t                 intensity_data_t;   // Right now we are encoding % intensity here
typedef uint16_t                position_data_t;    // Right now we are encoding pulse width here

// Specific Command Keywords
#define INTENSITY_NON_COMMAND   (0xFF)       
#define POSITION_NON_COMMAND    (0xFFFF)        // All commands on master
                                                //  should be initialized
                                                //  to this value.

// Extremes for Intensities, must be contained in INTENSITY_DATA_LEN
#define LIGHT_OFF               (0x00)
#define MIN_LIGHT_INTENSITY     (0)
#define MAX_LIGHT_INTENSITY     (100)      

// Stay Command for Servo (does not cause servo to move), must be contained
//  in POSITION_DATA_LEN
#define SERVO_STAY              (POSITION_NON_COMMAND)                 

// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################

// node_type_t (unused)
typedef enum
{
    master_node = 0,
    slave_node = 1
} node_type_t;

// rect_vect_t
typedef struct
{
    int16_t        x;                  // x position
    int16_t        y;                  // y position
} rect_vect_t;

// slave_parameters_t
// *Note: Zero angle is pointing to front of truck.
//        Thetas are always measured CLOCKWISE from zero angle,
//        when viewed from above.
// TODO: We need to define the origin.
typedef struct
{
    rect_vect_t         rect_position;      // x,y position
    uint16_t            theta_min;          // min movement angle
    uint16_t            theta_max;          // max movement angle (always > theta_min)
    position_data_t     position_min;       // min position on servo
    position_data_t     position_max;       // max position on servo
    uint16_t            fov;                // field of view of lens on light (angle)
    bool                move_equipped;      // whether the node can move
} slave_parameters_t;

// #############################################################################
// ------------ DO NOT MODIFY
// #############################################################################

// LIN Settings
#define FOSC                    8000            // in KHz
#define LIN_BAUDRATE            19200           // in bit/s

// YES/NO directives
#define NO                      0
#define YES                     1

// #############################################################################
// ------------ END CONFIG
// #############################################################################

#endif // config_H

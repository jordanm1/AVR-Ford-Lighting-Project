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

#define NUM_SLAVES          3

// #############################################################################
// ------------ NODE SETTINGS
// #############################################################################

#define THIS_NODE_TYPE      MASTER

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

// Master ID
#define MASTER_NODE_ID      (0x00)          // Master is the first ID

// Request Mask (the LSB will be high for status requests)
#define REQUEST_MASK        (0x01)
#define SLAVE_BASE_MASK     ~(0x01)

// Define a macro to get the slave number based on ID
#define Get_slave_number(slave_id)          (slave_id>>1)
#define Get_slave_base_id(slave_number)     (slave_number<<1)
                                                                                                                           
// #############################################################################
// ------------ INCLUDES
// #############################################################################

// AVR Library
#include <avr/io.h>                         // Use AVR-GCC library

// #############################################################################
// ------------ ENUMERATION TYPES
// #############################################################################

// node_type_t
typedef enum
{
    master_node = 0,
    slave_node = 1
} node_type_t;

// #############################################################################
// ------------ DO NOT MODIFY
// #############################################################################

// LIN Settings
#define FOSC            8000            // in KHz
#define LIN_BAUDRATE    19200           // in bit/s
#define LIN_PACKET_LEN  (2)             // always send 2 bytes of data

// Node Defs
#define MASTER          0
#define SLAVE           1

// #############################################################################
// ------------ END CONFIG
// #############################################################################

#endif // config_H

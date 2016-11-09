/*******************************************************************************
    File:
        cmd_sts_helpers.c
  
    Notes:
        This file contains helper functions for writing and parsing command
        and status arrays.

    External Functions Required:
        List external functions needed by this module

    Public Functions:
        List public functions this module provides
          
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
#include "cmd_sts_helpers.h"

// Include other files below:

// string for memset, memcpy, memcpr
#include <string.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################



// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Get_Intensity_Data

    Parameters
        None

    Description
        This returns an instance of the intensity data in a LIN-sized packet

****************************************************************************/
intensity_data_t Get_Intensity_Data(uint8_t * p_LIN_packet)
{
    intensity_data_t result;
    memcpy(&result, p_LIN_packet+INTENSITY_DATA_INDEX, INTENSITY_DATA_LEN);
    return result;
}

/****************************************************************************
    Public Function
        Get_Position_Data

    Parameters
        None

    Description
        This returns an instance of the position data in a LIN-sized packet

****************************************************************************/
position_data_t Get_Position_Data(uint8_t * p_LIN_packet)
{
    position_data_t result;
    memcpy(&result, p_LIN_packet+POSITION_DATA_INDEX, POSITION_DATA_LEN);
    return result;
}

/****************************************************************************
    Public Function
        Write_Intensity_Data

    Parameters
        None

    Description
        This write the intensity data to the correct location in the LIN-sized packet

****************************************************************************/
void Write_Intensity_Data(uint8_t * p_LIN_packet, intensity_data_t data_to_write)
{
    intensity_data_t temp = data_to_write;
    memcpy(p_LIN_packet+INTENSITY_DATA_INDEX, &temp, INTENSITY_DATA_LEN);
}

/****************************************************************************
    Public Function
        Write_Position_Data

    Parameters
        None

    Description
        This write the intensity data to the correct location in the LIN-sized packet

****************************************************************************/
void Write_Position_Data(uint8_t * p_LIN_packet, position_data_t data_to_write)
{
    position_data_t temp = data_to_write;
    memcpy(p_LIN_packet+POSITION_DATA_INDEX, &temp, POSITION_DATA_LEN);
}

/****************************************************************************
    Public Function
        Get_Pointer_To_Slave_Data

    Parameters
        None

    Description
        This returns a pointer to a slave specific section of a master array

****************************************************************************/
uint8_t * Get_Pointer_To_Slave_Data(uint8_t * p_master_array, uint8_t slave_num)
{
    // This assumes the first section of the master array corresponds
    //  to the lowest slave number (the first slave)
    return (p_master_array+((slave_num-LOWEST_SLAVE_NUMBER)*LIN_PACKET_LEN));
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

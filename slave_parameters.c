/*******************************************************************************
    File:
        slave_parameters.c
  
    Notes:
        This file contains the individual parameters for the slaves.

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
#include "__template.h"

// Include other files below:

// NULL
#include <stddef.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

// Array of structs to hold slave parameters
static const slave_parameters_t Slave_Parameters[NUM_SLAVES] =  {

// Slave One
#if (1 <= NUM_SLAVES)
{
    .rect_position.x =  1,
    .rect_position.y =  0,

    .fov =              30,

    .move_equipped =    true,

    .theta_min =        45,
    .position_min =     2250,

    .theta_max =        135,
    .position_max =     750,
},
#endif

// Slave Two
#if (2 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  1,

    .fov =              5,

    .move_equipped =    false,

    .theta_min =        0,
    .position_min =     0,

    .theta_max =        0,
    .position_max =     0
},
#endif

// Slave Three
#if (3 <= NUM_SLAVES)
{
    .rect_position.x =  -1,
    .rect_position.y =  0,

    .fov =              180,

    .move_equipped =    true,

    .theta_min =        270,
    .position_min =     10,

    .theta_max =        90,
    .position_max =     1
},
#endif

// Slave Four
#if (4 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  -1,

    .fov =              100,

    .move_equipped =    false,

    .theta_min =        180,
    .position_min =     0,

    .theta_max =        180,
    .position_max =     0
},
#endif

// Slave Five
#if (5 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              100,

    .move_equipped =    true,

    .theta_min =        90,
    .position_min =     0,

    .theta_max =        100,
    .position_max =     10
},
#endif

// Slave Six
#if (6 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              100,

    .move_equipped =    true,

    .theta_min =        90,
    .position_min =     0,

    .theta_max =        100,
    .position_max =     10
},
#endif

// Slave Seven
#if (7 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              100,

    .move_equipped =    true,

    .theta_min =        90,
    .position_min =     0,

    .theta_max =        100,
    .position_max =     10
},
#endif

// Slave Eight
#if (8 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              100,

    .move_equipped =    true,

    .theta_min =        90,
    .position_min =     0,

    .theta_max =        100,
    .position_max =     10
},
#endif
                                            
}; // END slave_parameters

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Get_Slave_Parameters

    Parameters
        None

    Description
        Gives a pointer to a struct variable containing the parameters
        of the requested slave (based on the slave base ID).

****************************************************************************/
const slave_parameters_t * Get_Pointer_To_Slave_Parameters(uint8_t slave_num)
{
    // Ensure the slave_base_id is within slave bounds
    if  (   (LOWEST_SLAVE_NUMBER > slave_num)
            ||
            (HIGHEST_SLAVE_NUMBER < slave_num)
        )
    {
        // Return false
        return NULL;
    }

    // Return the pointer to the requested slave parameters
    return (&Slave_Parameters[0]+slave_num-LOWEST_SLAVE_NUMBER);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################


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

// Program Memory
#include <avr/pgmspace.h>

// NULL
#include <stddef.h>

// #############################################################################
// ------------ TRUCK DEFINITION
// #############################################################################
/*
                                 | <-- 0 degrees
                    *-------------------------*
                    |                         |
                    |                         |
                    |                         |
                    |                         |
                    6-------------------------4     Row: 29
                    |                         |
          +y        |                         |
           ^        |                         |
           ^        7------------5------------3     Row: 1
           ^        |                         |
           -        |          (0,0)          |
                    |                         |
                    |                         |
                    |                         |
                    8                         2     Row: -21   
                    |                         |
                    |                         |
                    |                         |
                    |                         |
                    9-------------------------1     Row: -48


                negative x <<<<< | >>>>> positive x
*/
// #############################################################################
// ------------ SLAVE PARAMETERS
// #############################################################################

// Array of structs to hold slave parameters
// *Note: We store this array in program memory to save space in RAM;
//  each slave parameters member is 15 bytes

// rect_position:               [inches]
// fov:                         [degrees]
// move_equipped:               [bool]
// theta_min/max:               [degrees (0,359)]
//                              IMPORTANT: Always defined clockwise and positive!
// position_min/max:            [microseconds]

// NOTE: If a node is not move equipped, its theta_min = theta_max absolutely.

#define ULTRAFIRE_XM_L_FOV          (30)        // [degrees]
#define MG995_POSITION_LIMIT_MIN    (600)       // [microseconds]
#define MG995_POSITION_LIMIT_MAX    (2250)      // [microseconds]
#define JOYSWAY_POSITION_LIMIT_MIN  (960)       // [microseconds]
#define JOYSWAY_POSITION_LIMIT_MAX  (1630)      // [microseconds]

#define Y_POS_1_9                   (-48)
#define Y_POS_2_8                   (-21)
#define Y_POS_3_5_7                 (1)
#define Y_POS_4_6                   (29)

#define X_POS_1_2_3_4               (24)
#define X_POS_5                     (0)
#define X_POS_6_7_8_9               (-24)

static const slave_parameters_t Slave_Parameters[NUM_SLAVES] PROGMEM =  {

// Slave One
#if (1 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_1_2_3_4,
    .rect_position.y =  Y_POS_1_9,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        270,
    .position_min =     JOYSWAY_POSITION_LIMIT_MIN,

    .theta_max =        270,
    .position_max =     JOYSWAY_POSITION_LIMIT_MAX,
},
#endif

// Slave Two
#if (2 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_1_2_3_4,
    .rect_position.y =  Y_POS_2_8,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        0,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        180,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Three
#if (3 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_1_2_3_4,
    .rect_position.y =  Y_POS_3_5_7,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        0,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        180,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Four
#if (4 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_1_2_3_4,
    .rect_position.y =  Y_POS_4_6,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        0,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        180,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Five
#if (5 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_5,
    .rect_position.y =  Y_POS_3_5_7,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        90,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        270,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Six
#if (6 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_6_7_8_9,
    .rect_position.y =  Y_POS_4_6,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        180,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        0,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Seven
#if (7 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_6_7_8_9,
    .rect_position.y =  Y_POS_3_5_7,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        180,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        0,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Eight
#if (8 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_6_7_8_9,
    .rect_position.y =  Y_POS_2_8,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        180,
    .position_min =     MG995_POSITION_LIMIT_MAX,

    .theta_max =        0,
    .position_max =     MG995_POSITION_LIMIT_MIN
},
#endif

// Slave Nine
#if (9 <= NUM_SLAVES)
{
    .rect_position.x =  X_POS_6_7_8_9,
    .rect_position.y =  Y_POS_1_9,

    .fov =              ULTRAFIRE_XM_L_FOV,

    .move_equipped =    true,

    .theta_min =        90,
    .position_min =     JOYSWAY_POSITION_LIMIT_MIN,

    .theta_max =        90,
    .position_max =     JOYSWAY_POSITION_LIMIT_MAX
},
#endif

// Slave Ten
#if (10 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              0,

    .move_equipped =    false,

    .theta_min =        0,
    .position_min =     1500,

    .theta_max =        0,
    .position_max =     1500
},
#endif

// Slave Eleven
#if (11 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              0,

    .move_equipped =    false,

    .theta_min =        0,
    .position_min =     1500,

    .theta_max =        0,
    .position_max =     1500
},
#endif

// Slave Twelve
#if (12 <= NUM_SLAVES)
{
    .rect_position.x =  0,
    .rect_position.y =  0,

    .fov =              0,

    .move_equipped =    false,

    .theta_min =        0,
    .position_min =     1500,

    .theta_max =        0,
    .position_max =     1500
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
        of the requested slave (based on the slave base ID), that resides
        in the device's flash memory.

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


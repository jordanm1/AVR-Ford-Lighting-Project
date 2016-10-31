/*******************************************************************************
    File:
        light_setting_alg.c
  
    Notes:
        This file contains the algorithm to set individual lights
        based on the location of requested light.

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
#include <string.h>

// Light
#include "light_drv.h"

// Servo
#include "analog_servo_drv.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Intensity scaling factor
#define INTENSITY_SCALING_FACTOR    10

// Zero Degree Unit Vector (unused)
#define ZERO_DEG_X                  0
#define ZERO_DEG_Y                  1

// Degrees in circle
#define DEGS_FULL_CIRCLE            360

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################



// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static uint16_t invcos(uint16_t x);
static uint16_t norm2_rect_vect(rect_vect_t vect);
uint8_t interpolate_slave_position(const slave_parameters_t * p_slave_params, uint16_t desired_angle);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Compute_Individual_Light_Settings

    Parameters
        None

    Description
        This function computes the light settings for an individual node
        based on a user's desired location for light.

        Computes the light settings and copies the settings to p_cmd_data.

****************************************************************************/
void Compute_Individual_Light_Settings(
                                        const slave_parameters_t * p_slave_params,
                                        uint8_t * p_cmd_data,
                                        rect_vect_t v_desired_location)
{
    // If NULL pointers, return immediately
    if (!p_slave_params) return;

    // INITIALIZE VARS:
    uint16_t desired_theta;
    int16_t theta_light_min, theta_light_max;
    uint16_t norm2_desired_relative;
    rect_vect_t v_desired_relative;
    uint8_t result_settings[2];

    // COMPUTE:
    //   desired_theta (angle between desired location and zero)
    //  Since our zero degrees vector is (0,1) the dot product is
    //      v_zero_degrees * v_desired_location = v_desired_location.y = cos(theta)
    if (0 > v_desired_location.x)
    {
        // The vector is in the left half
        // Subtract computed angle from 360
        desired_theta = DEGS_FULL_CIRCLE-invcos(v_desired_location.y);
    }
    else
    {
        // The vector is in the right half.
        // Leave computed angle between as is
        desired_theta = invcos(v_desired_location.y);
    }

    // COMPUTE:
    //  Theta_Light_Min = Theta_Node_Min - (Field_Of_View / 2)
    //  Theta_Light_Max = Theta_Node_Max + (Field_Of_View / 2)
    theta_light_min = (p_slave_params->theta_min-(p_slave_params->fov/2));
    theta_light_max = (p_slave_params->theta_max+(p_slave_params->fov/2));

    // CHECK TO SEE IF ANGLE IS BETWEEN MIN AND MAX:
    if ((theta_light_min <= (int16_t) desired_theta) && (theta_light_max >= (int16_t) desired_theta))
    {
        // The desired position can be reached with light via this node.
        //  1. Set slave settings with some intensity > 0 and <= 100
        //  2. Set slave position via interpolation

        // v_User_Relative = (v_User-v_Node)
        v_desired_relative.x = v_desired_location.x-p_slave_params->rect_position.x;
        v_desired_relative.y = v_desired_location.y-p_slave_params->rect_position.y;

        // Compute distance^2 from node to desired location
        norm2_desired_relative = norm2_rect_vect(v_desired_relative);

        // Compute and set light intensity
        result_settings[INTENSITY_INDEX] = (MAX_LIGHT_INTENSITY*INTENSITY_SCALING_FACTOR)/norm2_desired_relative;
        if (MIN_LIGHT_INTENSITY > result_settings[INTENSITY_INDEX])
        {
            result_settings[INTENSITY_INDEX] = MIN_LIGHT_INTENSITY;
        }
        else if (MAX_LIGHT_INTENSITY < result_settings[INTENSITY_INDEX])
        {
            result_settings[INTENSITY_INDEX] = MAX_LIGHT_INTENSITY;
        }
        else
        {
            // Leave as is. Valid light intensity.
        }

        // Interpolate position between min and max if this slave is equipped to move
        if (p_slave_params->move_equipped)
        {
            result_settings[POSITION_INDEX] = interpolate_slave_position(p_slave_params, desired_theta);
        }
        else
        {
            // This slave is not equipped to move so return a non-command, so the servo
            // doesn't move
            result_settings[POSITION_INDEX] = NON_COMMAND;
        }
    }
    // Otherwise, this slave cannot put any light in the requested location
    else
    {
        // The desired position cannot be reached with light via this node.
        // Turn off the light and keep the servo where it is (send non command to servo)
        result_settings[INTENSITY_INDEX] = LIGHT_OFF;
        result_settings[POSITION_INDEX] = NON_COMMAND;
    }

    // *Note: after this algorithm runs, the light intensity will be set,
    //  and the servo position may or may not be a real position,
    //  (either NON_COMMAND or a valid position)

    // Copy data to location provided by caller
    memcpy(p_cmd_data, &result_settings, LIN_PACKET_LEN);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        invcos

    Parameters
        uint16_t x:

    Description
        Computes the inverse consine of the input parameter as 3rd order
        Taylor Series approximation.

        From:
        http://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm

****************************************************************************/
static uint16_t invcos(uint16_t x)
{
    return 57*((-0.69813170079773212 * x * x - 0.87266462599716477) * x + 1.5707963267948966);
}

/****************************************************************************
    Private Function
        norm2_rect_vect

    Parameters
        rect_vect_t vect

        Keep in mind,
            the range of 16-bit integer types is -32,768 through 32,767.

    Description
        Returns the squared norm of a 2-D vector

****************************************************************************/
static uint16_t norm2_rect_vect(rect_vect_t vect)
{
    return ((vect.x*vect.x) + (vect.y*vect.y));
}

/****************************************************************************
    Private Function
        interpolate_slave_position

    Parameters
        slave_parameters_t * p_slave_params
        uint16_t desired_angle

        Keep in mind,
            the range of 16-bit integer types is -32,768 through 32,767.

    Description
        Returns the squared norm of a 2-D vector

****************************************************************************/
uint8_t interpolate_slave_position(const slave_parameters_t * p_slave_params, uint16_t desired_angle)
{
    /* CALCULATE RANGE OF SLAVE IN DEGREES */
    // Calculate the range of degrees between min and max
    uint16_t slave_range_degs;
    if (p_slave_params->theta_max > p_slave_params->theta_min)
    {
        // Get normal range
        slave_range_degs = p_slave_params->theta_max - p_slave_params->theta_min;
    }
    else if (p_slave_params->theta_min > p_slave_params->theta_max)
    {
        // Get complimented range
        slave_range_degs = DEGS_FULL_CIRCLE - (p_slave_params->theta_max - p_slave_params->theta_min);
    }
    else
    {
        // This only occurs when the start and end angles are the same.
        // @TODO: Maybe make this zero instead.
        slave_range_degs = 0;
    }

    /* COMPUTE RATIO OF DESIRED ANGLE ABOVE THE MINIMUM ANGLE*/
    uint8_t num_positions;
    if (p_slave_params->theta_min >= desired_angle)
    {
        return p_slave_params->position_min;
    }
    else if (p_slave_params->theta_max <= desired_angle)
    {
        return p_slave_params->position_max;
    }
    else
    {
        // If positions are increasing from min to max
        if (p_slave_params->position_max > p_slave_params->position_min)
        {
            // Classic interpolation (add 0.5 to round to closest integer)
            // Since the positions are increasing, we add to min position
            num_positions = p_slave_params->position_max-p_slave_params->position_min;
            return p_slave_params->position_min+(0.5+(((desired_angle-p_slave_params->theta_min)*num_positions)/slave_range_degs));
        }
        // If positions are decreasing from min to max
        else if (p_slave_params->position_min > p_slave_params->position_max)
        {
            // Classic interpolation (add 0.5 to round to closest integer)
            // Since the positions are decreasing, we subtract from min position
            num_positions = p_slave_params->position_min-p_slave_params->position_max;
            return p_slave_params->position_min-(0.5+(((desired_angle-p_slave_params->theta_min)*num_positions)/slave_range_degs));
        }
        else
        {
            // Same angle so just use either
            return p_slave_params->position_min;
        }
    }
}

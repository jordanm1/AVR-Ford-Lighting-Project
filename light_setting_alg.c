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


// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

// Intensity scaling factor
#define INTENSITY_SCALING_FACTOR    10

// Zero Degree Unit Vector
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
uint8_t interpolate_slave_position(slave_parameters_t * p_slave_params, uint16_t desired_angle);

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

****************************************************************************/
void Compute_Individual_Light_Settings( \
                                        slave_parameters_t * p_slave_params, \
                                        uint8_t * p_cmd_data,
                                        rect_vect_t v_desired_location)
{
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
            result_settings[POSITION_INDEX] = SERVO_NO_MOVE;
        }
    }
    else
    {
        // The desired position cannot be reached with light via this node.
        result_settings[INTENSITY_INDEX] = 0;
        result_settings[POSITION_INDEX] = SERVO_NO_MOVE;
    }

    // Save data in location
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
uint8_t interpolate_slave_position(slave_parameters_t * p_slave_params, uint16_t desired_angle)
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
        slave_range_degs = DEGS_FULL_CIRCLE;
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
        if (p_slave_params->position_max > p_slave_params->position_min)
        {
            num_positions = p_slave_params->position_max-p_slave_params->position_min;
            return (0.5+(((desired_angle-p_slave_params->theta_min)*num_positions)/slave_range_degs));
        }
        else if (p_slave_params->position_min > p_slave_params->position_max)
        {
            num_positions = p_slave_params->position_min-p_slave_params->position_max;
            return num_positions-(0.5+(((desired_angle-p_slave_params->theta_min)*num_positions)/slave_range_degs));
        }
        else
        {
            return p_slave_params->position_min;
        }
    }
}

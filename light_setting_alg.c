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

// Command/Status helpers
#include "cmd_sts_helpers.h"

// memcpy
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

// INITIALIZE VARS:
uint16_t desired_theta, light_range;
int16_t theta_light_min, theta_light_max;
uint16_t norm2_desired_relative;
rect_vect_t v_desired_relative;
uint8_t result_settings[LIN_PACKET_LEN];

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static uint16_t norm2_rect_vect(rect_vect_t vect);
static uint16_t compute_our_rel_angle(rect_vect_t v_rel, uint16_t norm2_v_rel);
static position_data_t interpolate_slave_position(const slave_parameters_t * p_slave_params, uint16_t desired_angle);
static uint16_t compute_cw_angular_distance(uint16_t start_angle, uint16_t end_angle);

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
    if (!p_cmd_data) return;

//     // INITIALIZE VARS:
//     uint16_t desired_theta, light_range;
//     int16_t theta_light_min, theta_light_max;
//     uint16_t norm2_desired_relative;
//     rect_vect_t v_desired_relative;
//     uint8_t result_settings[2];

    // COMPUTE:
    // v_User_Relative = (v_User-v_Node)
    // This is because we want to see the angle relative to where we are,
    // For example, we could be at (1,0) with 180 FOV and the requested
    //  loc. could be (0.8,1) and the angle would be within our bounds,
    //  but if we use the desired location relative to us, the angle would
    //  be outside of our bounds.
    v_desired_relative.x = v_desired_location.x-p_slave_params->rect_position.x;
    v_desired_relative.y = v_desired_location.y-p_slave_params->rect_position.y;

    // COMPUTE:
    // distance^2 from node to desired location
    norm2_desired_relative = norm2_rect_vect(v_desired_relative);

    // COMPUTE:
    // the relative angle of the desired location to us
    desired_theta = compute_our_rel_angle(v_desired_relative, norm2_desired_relative);

    // COMPUTE:
    //  Light range in degs
    if (p_slave_params->move_equipped)
    {
        // Theta_min/max can be anything, treats equal angles as full 360 movement capable
        light_range = compute_cw_angular_distance(p_slave_params->theta_min, p_slave_params->theta_max)
                        + p_slave_params->fov;
    }
    else
    {
        // The theta_min/max must be the same angle in slave_parameters
        light_range = p_slave_params->fov;
    }

    // COMPUTE:
    //  Theta_Light_Min = Theta_Node_Min - (Field_Of_View / 2)
    //  Theta_Light_Max = Theta_Node_Max + (Field_Of_View / 2)
    theta_light_min = (p_slave_params->theta_min-(p_slave_params->fov/2)+DEGS_FULL_CIRCLE)%DEGS_FULL_CIRCLE;
    theta_light_max = (p_slave_params->theta_max+(p_slave_params->fov/2)+DEGS_FULL_CIRCLE)%DEGS_FULL_CIRCLE;

    // CHECK TO SEE IF ANGLE IS BETWEEN MIN AND MAX:
    // COMPUTE:
    //  Distance from min to desired
    //  Distance from min to max
    if  (   (compute_cw_angular_distance(theta_light_min, desired_theta)
            <=
            compute_cw_angular_distance(theta_light_min, theta_light_max))

            ||

            (DEGS_FULL_CIRCLE <= light_range)
        )
    {
        // The desired position can be reached with light via this node.
        //  1. Set slave settings with some intensity > 0 and <= 100
        //  2. Set slave position via interpolation

        // Compute and set light intensity
        intensity_data_t temp_intensity = INTENSITY_NON_COMMAND;
        temp_intensity = (MAX_LIGHT_INTENSITY*INTENSITY_SCALING_FACTOR)/norm2_desired_relative;
        if (MIN_LIGHT_INTENSITY > temp_intensity)
        {
            temp_intensity = MIN_LIGHT_INTENSITY;
        }
        else if (MAX_LIGHT_INTENSITY < temp_intensity)
        {
            temp_intensity = MAX_LIGHT_INTENSITY;
        }
        else
        {
            // Leave as is. Valid light intensity.
        }
        Write_Intensity_Data(result_settings, temp_intensity);

        // Interpolate position between min and max if this slave is equipped to move
        position_data_t temp_position = POSITION_NON_COMMAND;
        if (p_slave_params->move_equipped)
        {
            temp_position = interpolate_slave_position(p_slave_params, desired_theta);
        }
        else
        {
            // This slave is not equipped to move so return a non-command, so the servo
            // doesn't move
            temp_position = POSITION_NON_COMMAND;
        }
        Write_Position_Data(result_settings, temp_position);
    }
    // Otherwise, this slave cannot put any light in the requested location
    else
    {
        // The desired position cannot be reached with light via this node.
        // Turn off the light and keep the servo where it is (send non command to servo)
        Write_Intensity_Data(result_settings, INTENSITY_NON_COMMAND);
        Write_Position_Data(result_settings, POSITION_NON_COMMAND);
    }

    // *Note: after this algorithm runs, the light intensity will be set,
    //  and the servo position may or may not be a real position,
    //  (either NON_COMMAND or a valid position)

    // Copy data to location provided by caller
    memcpy(p_cmd_data, result_settings, LIN_PACKET_LEN);
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

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
        compute_our_rel_angle()

    Parameters
        uint16_t y: y coordinate relative to us

    Description
        Computes the angle based on our coordinate system

        From:
        (invcos)
        http://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
        (inv sqrt)
        https://en.wikipedia.org/wiki/Fast_inverse_square_root

****************************************************************************/
static uint16_t compute_our_rel_angle(rect_vect_t v_rel, uint16_t norm2_v_rel)
{
    //
    // Compute 1/sqrt(norm2)
    //
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = norm2_v_rel * 0.5F;
	y  = norm2_v_rel;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    //
    // Multiply 1/sqrt(norm2) by the y so we get normalized y component
    //
    float normalized_y = v_rel.y*y;
    if (1 < normalized_y) normalized_y = 1;     // clamp since we are dealing with approximations
    if (-1 > normalized_y) normalized_y = -1;   // clamp since we are dealing with approximations

    //
    // Take inverse cosine to get angle, via taylor3 approximation
    //
    // We only care about our relative y component because our zero degree vector is <0,1>
    //  and the dot product is just the y component
    // Wolfram Alpha: plot 57*arccos(x) for x = {-1,1}
    //  acos ranges from 0 to 180 degrees
    uint16_t angle = 57*((-0.69813170079773212 * normalized_y * normalized_y - 0.87266462599716477) * normalized_y + 1.5707963267948966);

    //
    // Output angle
    //
    if (0 > v_rel.x)
    {
        // The vector is in the left half
        // Subtract computed angle from 360 to get cw angle
        return DEGS_FULL_CIRCLE-angle;
    }
    else
    {
        // The vector is in the right half.
        // Leave computed angle between as is
        return angle;
    }
}

/****************************************************************************
    Private Function
        interpolate_slave_position

    Parameters
        slave_parameters_t * p_slave_params
        uint16_t desired_angle

    Description
        Returns the position of a servo to get the servo as close
            to the desired angle as possible. If the desired angle
            is outside of the range, the servo will move to one of
            its limits.

****************************************************************************/
static position_data_t interpolate_slave_position(const slave_parameters_t * p_slave_params, uint16_t desired_angle)
{
    // Initialize result
    position_data_t result = POSITION_NON_COMMAND;

    /* CALCULATE RANGE OF SLAVE IN DEGREES */
    // Calculate the range of degrees between min and max
    uint16_t slave_range_degs;
    slave_range_degs = compute_cw_angular_distance(p_slave_params->theta_min, p_slave_params->theta_max);

    /* COMPUTE RATIO OF DESIRED ANGLE ABOVE THE MINIMUM ANGLE*/
    position_data_t position_range;
    // If the desired angle is outside of our move ability
    if  (   compute_cw_angular_distance(p_slave_params->theta_min, desired_theta)
            >=
            slave_range_degs
        )
    {
        // If the desired angle is closer to our min
        if  (   compute_cw_angular_distance(p_slave_params->theta_max, desired_theta)
                >
                compute_cw_angular_distance(desired_theta, p_slave_params->theta_min)
            )
        {
            // Return our min position
            result = p_slave_params->position_min;
        }
        // Otherwise, the desired angle is closer to our max
        else
        {
            // Return our max position
            result = p_slave_params->position_max;
        }
    }
    // Otherwise, angle is actually between our min and max (going CW)
    //      so... interpolate
    else
    {
        // If positions are increasing from min to max
        if (p_slave_params->position_max > p_slave_params->position_min)
        {
            // Classic interpolation (add 0.5 to round to closest integer)
            // Since the positions are increasing, we add to min position
            // @TODO: Make sure these won't overflow
            position_range = p_slave_params->position_max-p_slave_params->position_min;
            result = p_slave_params->position_min + (0.5+((compute_cw_angular_distance(p_slave_params->theta_min, desired_theta) * (uint32_t) position_range)/slave_range_degs));
        }
        // If positions are decreasing from min to max
        else if (p_slave_params->position_min > p_slave_params->position_max)
        {
            // Classic interpolation (add 0.5 to round to closest integer)
            // Since the positions are decreasing, we subtract from min position
            // @TODO: Make sure these won't overflow
            position_range = p_slave_params->position_min-p_slave_params->position_max;
            result = p_slave_params->position_min - (0.5+((compute_cw_angular_distance(p_slave_params->theta_min, desired_theta) * (uint32_t) position_range)/slave_range_degs));
        }
        else
        {
            // Same angle so just use either
            // *Note: This should never be reached,
            // One angle should not be able to span
            //  two locations.
            // Anyways, just return the one position
            //  since it is constant across the angles
            result = p_slave_params->position_min;
        }
    }

    // Check for position validity before returning
    // @TODO: We might want to still move it to the closest edge.
    if (Is_Servo_Position_Valid(p_slave_params, result)) 
    {
        return result;
    }
    else
    {
        return POSITION_NON_COMMAND;
    }
}

/****************************************************************************
    Private Function
        compute_cw_angular_distance()

    Parameters
        uint16_t: start angle
        uint16_t: end angle

        Keep in mind,
            the range of 16-bit integer types is -32,768 through 32,767.

    Description
        Returns the angular distance in the clockwise direction:
            The start angle + return angle equals end angle
            (start_angle+return_angle)%360=end+angle

****************************************************************************/
static uint16_t compute_cw_angular_distance(uint16_t start_angle, uint16_t end_angle)
{
    if (end_angle > start_angle)
    {
        // Just do the positive biased subtraction
        return end_angle-start_angle;
    }
    else if (end_angle < start_angle)
    {
        // Take the compliment of the positive biased subtraction
        // *Note: this is for cases where the end angle is right of 0 degs
        //      and start is left of 0 degs
        return DEGS_FULL_CIRCLE-(start_angle-end_angle);
    }
    else
    {
        // The angles are equal
        return DEGS_FULL_CIRCLE;
    }
}

// #############################################################################
// ------------ NOTES/LEGACY
// #############################################################################

// /****************************************************************************
//     Private Function
//         invcos
// 
//     Parameters
//         uint16_t x:
// 
//     Description
//         Computes the inverse cosine of the input parameter as 3rd order
//         Taylor Series approximation.
// 
//         From:
//         http://stackoverflow.com/questions/3380628/fast-arc-cos-algorithm
// 
// ****************************************************************************/
// static uint16_t invcos(uint16_t x)
// {
//     return 57*((-0.69813170079773212 * x * x - 0.87266462599716477) * x + 1.5707963267948966);
// }

//     // COMPUTE:
//     //   desired_theta (angle between desired location and zero)
//     //  Since our zero degrees vector is (0,1) the dot product is
//     //      v_zero_degrees * v_desired_relative = v_desired_relative.y = cos(theta)
//     if (0 == v_desired_relative.x)
//     {
//         // The requested location is along the vector (0,1)
//         //  so the angle is zero degrees or 180
//         if (0 <= v_desired_relative.y)
//         {
//             desired_theta = 0;
//         }
//         else
//         {
//             desired_theta = 180;
//         }
//     }
//     else if (0 > v_desired_relative.x)
//     {
//         // The vector is in the left half
//         // Subtract computed angle from 360
//         desired_theta = DEGS_FULL_CIRCLE-invcos(v_desired_relative.y);
//     }
//     else
//     {
//         // The vector is in the right half.
//         // Leave computed angle between as is
//         desired_theta = invcos(v_desired_relative.y);
//     }

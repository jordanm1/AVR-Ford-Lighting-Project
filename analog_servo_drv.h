#ifndef ANALOG_SERVO_DRV_H
#define ANALOG_SERVO_DRV_H

// #############################################################################
// ------------ INCLUDES
// #############################################################################

#include "config.h"

// #############################################################################
// ------------ SERVO DEFINITIONS
// #############################################################################

// Stay Command for Servo (does not cause servo to move)
#define SERVO_STAY                  (NON_COMMAND)

// Servo Control Pin
#define ANALOG_SERVO_CH1_PORT       PORTB
#define ANALOG_SERVO_CH1_PDIR       DDRB
#define ANALOG_SERVO_CH1_PIN        PINB2

// Max time for servo to move between extreme positions
#define SERVO_DRIVE_TIME_MS         2000

// Servo command min pulse width, max pulse width
#define MIN_PULSE_WIDTH_TENTHMS     10
#define MAX_PULSE_WIDTH_TENTHMS     20

// *Note: Number of positions possible is:
//
//      (MAX_PULSE_WIDTH_TENTHMS-MIN_PULSE_WIDTH_TENTHMS)+1
//
//      where positions are:
//
//      0 to (MAX_PULSE_WIDTH_TENTHMS-MIN_PULSE_WIDTH_TENTHMS)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_Analog_Servo_Driver(void);
void Move_Analog_Servo_To_Position(uint8_t requested_position);
void Hold_Analog_Servo_Position(uint8_t requested_position);
void Release_Analog_Servo(void);
bool Is_Servo_Position_Valid(const slave_parameters_t * p_slave_params, uint8_t requested_position);

#endif // ANALOG_SERVO_DRV_H

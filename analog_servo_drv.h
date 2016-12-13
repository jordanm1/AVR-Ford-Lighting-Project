#ifndef ANALOG_SERVO_DRV_H
#define ANALOG_SERVO_DRV_H

// #############################################################################
// ------------ INCLUDES
// #############################################################################

#include "config.h"

// #############################################################################
// ------------ SERVO DEFINITIONS
// #############################################################################

// Servo Control Pin, (Use PWM channel B)
#define ANALOG_SERVO_PWM_CH         pwm_channel_b
#define ANALOG_SERVO_PWM_EN         PWM_CH_B_PIN_ENABLE
#define ANALOG_SERVO_DRV_PORT       PORTB
#define ANALOG_SERVO_DRV_PDIR       DDRB
#define ANALOG_SERVO_DRV_PIN        PWM_CH_B_PIN

// Max time for servo to move between extreme positions
#define SERVO_DRIVE_TIME_MS         2000

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_Analog_Servo_Driver(void);
void Move_Analog_Servo_To_Position(position_data_t requested_position);
void Hold_Analog_Servo_Position(position_data_t requested_position);
void Release_Analog_Servo(void);
bool Is_Servo_Position_Valid(const slave_parameters_t * p_slave_params, position_data_t requested_position);

#endif // ANALOG_SERVO_DRV_H

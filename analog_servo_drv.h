#ifndef ANALOG_SERVO_DRV_H
#define ANALOG_SERVO_DRV_H

// #############################################################################
// ------------ SERVO DEFINITIONS
// #############################################################################

// Servo no move position
#define SERVO_NO_MOVE               (0x7F)

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
void Hold_Analog_Servo_Position(uint8_t position);
void Release_Analog_Servo(void);

#endif // ANALOG_SERVO_DRV_H

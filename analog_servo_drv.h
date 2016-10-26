#ifndef ANALOG_SERVO_DRV_H
#define ANALOG_SERVO_DRV_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_Analog_Servo_Driver(void);
void Move_Analog_Servo_To_Position(uint8_t requested_position);
void Hold_Analog_Servo_Position(uint8_t position);
void Release_Analog_Servo(void);

#endif // ANALOG_SERVO_DRV_H

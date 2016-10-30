#ifndef SERVO_MODULE_H
#define SERVO_MODULE_H

// #############################################################################
// ------------ SERVO_MODULE DEFINITIONS
// #############################################################################


// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Set_Servo_Duty_Cycle(uint8_t duty_cycle);
uint8_t Poll_Servo_Duty_Cycle(void);
bool Poll_Servo_On(void);

#endif // LED_MODULE_H

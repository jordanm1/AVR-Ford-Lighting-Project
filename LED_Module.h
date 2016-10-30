#ifndef LED_MODULE_H
#define LED_MODULE_H

// #############################################################################
// ------------ LED_MODULE DEFINITIONS
// #############################################################################


// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Set_LED_Duty_Cycle(uint8_t duty_cycle);
uint8_t Poll_LED_Duty_Cycle(void);
bool Poll_LED_On(void);

#endif // LED_MODULE_H

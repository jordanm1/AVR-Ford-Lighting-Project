#ifndef PWM_H
#define PWM_H

// #############################################################################
// ------------ PWM DEFINITIONS
// #############################################################################

// Channel Enum Types
typedef enum
{
   pwm_channel_a = 0,
   pwm_channel_b = 1
} pwm_channel_t;

// Define channels and pins for functions.
// Two functions cannot have the same channel,
//    unless you want the same duty cycle.
#define LED_PWM_CHANNEL       (pwm_channel_a)
#define MOTOR_PWM_CHANNEL     (pwm_channel_b)

// Pins on (p. 81)
#define PWM_CH_A_PIN          (PB4)
#define PWM_CH_A_PIN_ENABLE   (OC1AW)     // Pin PB4
#define PWM_CH_B_PIN          (PB3)
#define PWM_CH_B_PIN_ENABLE   (OC1BV)     // Pin PB3

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_PWM_Module(void);
void Set_PWM_Duty_Cycle(pwm_channel_t this_channel, uint8_t new_duty_cycle);

#endif // PWM_H

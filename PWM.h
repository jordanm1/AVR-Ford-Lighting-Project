#ifndef PWM_H
#define PWM_H

// #############################################################################
// ------------ PWM DEFINITIONS
// #############################################################################

// Define PWM frequency
#define PWM_FREQ                (200)       // 200 Hz
#define US_IN_PWM_PERIOD        (5000)      // 1/PWM_FREQ/US_IN_SEC

// Define timer 1 prescale amount
#define TIMER_1_PRESCALE        ((0<<CS12)|(1<<CS11)|(0<<CS10))

// Define the top counter value for the PWM module.
// This value sets the PWM frequency along with prescale.
#define TIMER_1_TOP             (4999)      // Chosen for ~200 Hz 
                                            //  with prescale of 1:8
                                            // This effectively sets
                                            //  our PWM resolution.

// Channel Enum Types
typedef enum
{
    pwm_channel_a = 0,
    pwm_channel_b = 1
} pwm_channel_t;

// Pins on (p. 81)
#define PWM_CH_A_PIN            (PINB4)
#define PWM_CH_A_PIN_ENABLE     (OC1AW)     // Pin PB4
#define PWM_CH_B_PIN            (PINB3)
#define PWM_CH_B_PIN_ENABLE     (OC1BV)     // Pin PB3

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_PWM_Module(void);
void Set_PWM_Duty_Cycle(pwm_channel_t this_channel, uint8_t new_duty_cycle);

#endif // PWM_H

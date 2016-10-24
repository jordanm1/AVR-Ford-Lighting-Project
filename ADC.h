#ifndef ADC_H
#define ADC_H

// #############################################################################
// ------------ ADC DEFINITIONS
// #############################################################################

#define IMPOSSIBLE_ADC_COUNT        UINT16_MAX

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_ADC_Module(void);
uint16_t Get_ADC_Result(void);
void Start_ADC_Measurement(void);

#endif // ADC_H

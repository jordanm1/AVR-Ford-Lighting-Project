#ifndef ADC_H
#define ADC_H

// #############################################################################
// ------------ IOC DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_ADC_Module(void);
uint16_t Get_ADC_Result(void);
void Poll_ADC(void);

#endif // ADC_H

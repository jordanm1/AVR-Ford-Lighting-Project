#ifndef __setup_H
#define __setup_H

// #############################################################################
// ------------ MODULES (only those that need init or process events)
// #############################################################################

#include "timer.h"

#include "LIN_XCVR_WD_Kicker.h"

#include "PWM.h"

#include "IOC.h"

#include "ADC.h"

#include "master_service.h"

#include "slave_service.h"

// #############################################################################
// ------------ INITIALIZATIONS (must be functions of type "void f(void)")
// #############################################################################

#define NUM_INITIALIZERS            6

#define INITIALIZER_00              Init_Timer_Module

#define INITIALIZER_01              Init_LIN_XCVR_WD_Kicker

#define INITIALIZER_02              Init_PWM_Module

#define INITIALIZER_03              Init_IOC_Module

#define INITIALIZER_04              Init_ADC_Module

#if (MASTER == THIS_NODE_TYPE)
#define INITIALIZER_05              Init_Master_Service
#else
#define INITIALIZER_05              Init_Slave_Service
#endif

// #############################################################################
// ------------ SERVICES (must be functions of type "void f(uint32_t event)")
// #############################################################################

#define NUM_SERVICES                1

#if (MASTER == THIS_NODE_TYPE)
#define SERVICE_00		            Run_Master_Service
#else
#define SERVICE_00		            Run_Slave_Service
#endif

// #############################################################################
// ------------ EVENT DEFINITIONS
// #############################################################################

// Number of events we've defined
#define NUM_EVENTS                  6

#define EVT_SLAVE_NEW_CMD           (0x01<<0)
#define EVT_SLAVE_OTHER             (0x01<<1)

#define EVT_MASTER_SCH_TIMEOUT      (0x01<<2)
#define EVT_MASTER_NEW_STS          (0x01<<3)
#define EVT_MASTER_OTHER            (0x01<<4)

#define EVT_TEST_TIMEOUT            (0x01<<5)

// #############################################################################
// ------------ END OF FILE
// #############################################################################

#endif /* __setup_H */
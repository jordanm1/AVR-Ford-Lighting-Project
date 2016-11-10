#ifndef __setup_H
#define __setup_H

// #############################################################################
// ------------ FRAMEWORK EVENT MACROS (we define events below using these) 
// #############################################################################

#include "events.h"

// #############################################################################
// ------------ MODULES (only those that need init or process events)
// #############################################################################

#include "timer.h"

#include "LIN_XCVR_WD_Kicker.h"

#include "PWM.h"

#include "IOC.h"

#include "ADC.h"

#include "analog_servo_drv.h"

#include "master_service.h"

#include "slave_service.h"

#include "slave_number_setting_SM.h"

// #############################################################################
// ------------ INITIALIZATIONS (must be functions of type "void f(void)")
// #############################################################################

#define INITIALIZER_00              Init_Timer_Module

#define INITIALIZER_01              Init_LIN_XCVR_WD_Kicker

#define INITIALIZER_02              Init_PWM_Module

#define INITIALIZER_03              Init_IOC_Module

#define INITIALIZER_04              Init_ADC_Module

#define INITIALIZER_05              Init_Analog_Servo_Driver

#if IS_MASTER_NODE
    #define INITIALIZER_06          Init_Master_Service
#else
    #define INITIALIZER_06          Init_Slave_Service
    #define INITIALIZER_07          Init_Slave_Number_Setting_SM
#endif

// #############################################################################
// ------------ SERVICES (must be functions of type "void f(uint32_t event)")
// #############################################################################

#if IS_MASTER_NODE
    #define SERVICE_00		        Run_Master_Service
#else
    #define SERVICE_00		        Run_Slave_Service
    #define SERVICE_01              Run_Slave_Number_Setting_SM
#endif

// #############################################################################
// ------------ EVENT DEFINITIONS
// #############################################################################

// Number of events we've defined
#define NUM_EVENTS                      13

#define NON_EVENT                       EVENT_NULL
       
#define EVT_SLAVE_NEW_CMD               EVENT_01
#define EVT_SLAVE_OTHER                 EVENT_02
#define EVT_SETTING_MODE_MAIN_TIMEOUT   EVENT_03
#define EVT_SETTING_MODE_AUX_TIMEOUT    EVENT_04
#define EVT_ENTERED_SETTING_MODE        EVENT_05
#define EVT_EXITED_SETTING_MODE         EVENT_06
#define EVT_SLAVE_NUM_SET               EVENT_07

#define EVT_MASTER_SCH_TIMEOUT          EVENT_08
#define EVT_MASTER_NEW_STS              EVENT_09
#define EVT_MASTER_OTHER                EVENT_10

#define EVT_TEST_TIMEOUT                EVENT_11

#define EVT_BTN_MISC_PRESS              EVENT_12
#define EVT_BTN_MISC_RELEASE            EVENT_13

// #############################################################################
// ------------ END OF FILE
// #############################################################################

#endif /* __setup_H */

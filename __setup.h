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

#include "buttons.h"

#include "master_service.h"

#include "SPI_Service.h"

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

#define INITIALIZER_05              Init_Buttons

#if IS_MASTER_NODE
    #define INITIALIZER_06          Init_SPI_Service
    #define INITIALIZER_07          Init_Master_Service
#else
    #define INITIALIZER_06          Init_Analog_Servo_Driver
    #define INITIALIZER_07          Init_Slave_Service
    #define INITIALIZER_08          Init_Slave_Number_Setting_SM
#endif

// #############################################################################
// ------------ SERVICES (must be functions of type "void f(uint32_t event)")
// #############################################################################

#define SERVICE_00                  Run_Buttons

#if IS_MASTER_NODE
    #define SERVICE_01		        Run_Master_Service
    #define SERVICE_02              Run_SPI_Service
#else
    #define SERVICE_01		        Run_Slave_Service
    #define SERVICE_02              Run_Slave_Number_Setting_SM
#endif

// #############################################################################
// ------------ EVENT DEFINITIONS
// #############################################################################

// Number of events we've defined
#define NUM_EVENTS                      18

#define NON_EVENT                       EVENT_NULL
       
#define EVT_SLAVE_NEW_CMD               EVENT_01
#define EVT_SLAVE_OTHER                 EVENT_02
#define EVT_SETTING_MODE_MAIN_TIMEOUT   EVENT_03
#define EVT_SETTING_MODE_AUX_TIMEOUT    EVENT_04
#define EVT_SLAVE_NUM_SET               EVENT_05

#define EVT_MASTER_SCH_TIMEOUT          EVENT_06
#define EVT_MASTER_NEW_STS              EVENT_07
#define EVT_MASTER_OTHER                EVENT_08
#define EVT_CAN_INIT_1_COMPLETE         EVENT_09
#define EVT_CAN_POLLING_TIMEOUT         EVENT_10

#define EVT_TEST_TIMEOUT                EVENT_11

#define EVT_BTN_MISC_PRESS              EVENT_12
#define EVT_BTN_MISC_RELEASE            EVENT_13

#define EVT_BTN_DEBOUNCE_TIMEOUT        EVENT_14

#define EVT_SPI_START                   EVENT_15
#define EVT_SPI_SEND_BYTE               EVENT_16
#define EVT_SPI_RECV_BYTE               EVENT_17
#define EVT_SPI_END                     EVENT_18

// #############################################################################
// ------------ END OF FILE
// #############################################################################

#endif /* __setup_H */

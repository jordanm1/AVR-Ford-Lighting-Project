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

#include "analog_servo_drv.h"

#include "master_service.h"

#include "slave_service.h"

#include "SPI_Service.h"

// #############################################################################
// ------------ INITIALIZATIONS (must be functions of type "void f(void)")
// #############################################################################

#define INITIALIZER_00              Init_Timer_Module

#define INITIALIZER_01              Init_LIN_XCVR_WD_Kicker

#define INITIALIZER_02              Init_PWM_Module

#define INITIALIZER_03              Init_IOC_Module

#define INITIALIZER_04              Init_ADC_Module

#define INITIALIZER_05              Init_Analog_Servo_Driver

#define INITIALIZER_06              Init_SPI_Service

#if IS_MASTER_NODE
    #define INITIALIZER_07          Init_Master_Service
#else
    #define INITIALIZER_07          Init_Slave_Service
#endif

// #############################################################################
// ------------ SERVICES (must be functions of type "void f(uint32_t event)")
// #############################################################################

#if IS_MASTER_NODE
    #define SERVICE_00		        Run_Master_Service
	#define SERVICE_01				Run_SPI_Service
#else
    #define SERVICE_00		        Run_Slave_Service
#endif

// #############################################################################
// ------------ EVENT DEFINITIONS
// #############################################################################

// Number of events we've defined
#define NUM_EVENTS                  3

#define NON_EVENT                   (0x00)

#define EVT_SLAVE_GET_ID            (0x01<<0)       // This will result in blocking
                                                    //  code to get the slave ID
                                                    //  before continuing with
                                                    //  anything else.
                                                 // This needs to have highest priority.
#define EVT_SLAVE_NEW_CMD           (0x01<<6)
#define EVT_SLAVE_OTHER             (0x01<<7)

#define EVT_MASTER_SCH_TIMEOUT      (0x01<<8)
#define EVT_MASTER_NEW_STS          (0x01<<9)

#define EVT_MASTER_OTHER            (0x01<<10)


#define EVT_TEST_TIMEOUT            (0x01<<1)

#define EVT_SPI_START               (0x01<<2)
#define EVT_SPI_SEND_BYTE           (0x01<<3)
#define EVT_SPI_RECV_BYTE           (0x01<<4)
#define EVT_SPI_END                 (0x01<<5)


// #############################################################################
// ------------ END OF FILE
// #############################################################################

#endif /* __setup_H */

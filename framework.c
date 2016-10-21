/*******************************************************************************
      File:
      framework_setup.c
     
      Notes:
      This file contains the framework which converts data in the __setup.h file
      for the events and service functions.
   
      External Functions Required:

      Public Functions:
        
*******************************************************************************/

// #############################################################################
// ------------ INCLUDES
// #############################################################################

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>

// Framework
#include "framework.h"

// #############################################################################
// ------------ DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################



// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
      Public Function
         Initialize_Framework

      Parameters
         None

      Description
         Calls all initializer functions, can service up to 99 functions

****************************************************************************/
void Initialize_Framework(void)
{
   // Call all initializers
   #ifdef INITIALIZER_00
   INITIALIZER_00();
   #endif
   #ifdef INITIALIZER_01
   INITIALIZER_01();
   #endif
   #ifdef INITIALIZER_02
   INITIALIZER_02();
   #endif
   #ifdef INITIALIZER_03
   INITIALIZER_03();
   #endif
   #ifdef INITIALIZER_04
   INITIALIZER_04();
   #endif
   #ifdef INITIALIZER_05
   INITIALIZER_05();
   #endif
   #ifdef INITIALIZER_06
   INITIALIZER_06();
   #endif
   #ifdef INITIALIZER_07
   INITIALIZER_07();
   #endif
   #ifdef INITIALIZER_08
   INITIALIZER_09();
   #endif
   #ifdef INITIALIZER_10
   INITIALIZER_10();
   #endif
   #ifdef INITIALIZER_11
   INITIALIZER_11();
   #endif
   #ifdef INITIALIZER_12
   INITIALIZER_12();
   #endif
   #ifdef INITIALIZER_13
   INITIALIZER_13();
   #endif
   #ifdef INITIALIZER_14
   INITIALIZER_14();
   #endif
   #ifdef INITIALIZER_15
   INITIALIZER_15();
   #endif
   #ifdef INITIALIZER_16
   INITIALIZER_16();
   #endif
}

/****************************************************************************
      Public Function
         Run_Services

      Parameters
         None

      Description
         Calls the services which process events, 
            can service up to 99 functions

****************************************************************************/
void Run_Services(uint32_t event)
{
   // Call all services
   #ifdef SERVICE_00
   SERVICE_00(event);
   #endif
   #ifdef SERVICE_01
   SERVICE_01(event);
   #endif
   #ifdef SERVICE_02
   SERVICE_02(event);
   #endif
   #ifdef SERVICE_03
   SERVICE_03(event);
   #endif
   #ifdef SERVICE_04
   SERVICE_04(event);
   #endif
   #ifdef SERVICE_05
   SERVICE_05(event);
   #endif
   #ifdef SERVICE_06
   SERVICE_06(event);
   #endif
   #ifdef SERVICE_07
   SERVICE_07(event);
   #endif
   #ifdef SERVICE_08
   SERVICE_08(event);
   #endif
   #ifdef SERVICE_09
   SERVICE_09(event);
   #endif
   #ifdef SERVICE_10
   SERVICE_10(event);
   #endif
   #ifdef SERVICE_11
   SERVICE_11(event);
   #endif
   #ifdef SERVICE_12
   SERVICE_12(event);
   #endif
   #ifdef SERVICE_13
   SERVICE_13(event);
   #endif
   #ifdef SERVICE_14
   SERVICE_14(event);
   #endif
   #ifdef SERVICE_15
   SERVICE_15(event);
   #endif
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################
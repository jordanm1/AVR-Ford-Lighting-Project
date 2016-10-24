/*******************************************************************************
    File:
        __template.c
  
    Notes:
        This file contains template to use with the framework.

    External Functions Required:
        List external functions needed by this module

    Public Functions:
        List public functions this module provides
          
*******************************************************************************/

// #############################################################################
// ------------ INCLUDES
// #############################################################################

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>

// Config file
#include "config.h"

// Framework
#include "framework.h"

// This module's header file
#include "__template.h"

// Include other files below:



// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################

#define TEMPLATE_DEFINITION         0

// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

static uint8_t template_var = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void template_private_function(void);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Template_Public_Function

    Parameters
        None

    Description
        This is an example for a public function.

****************************************************************************/
void Template_Public_Function(void)
{
    // Example
    template_private_function();
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        Template_Private_Function

    Parameters
        None

    Description
        This is an example for a private function.

****************************************************************************/
static void template_private_function(void)
{
    // Example
    template_var = 0;
}

/*******************************************************************************
    File:
        CAN.c
 
    Notes:
        This file contains the CAN module. This module initializes the CAN hardware,
        and provides access to transmit and receive functions for MCP 25625.

    External Functions Required:

    Public Functions:
        
                
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
#include "CAN.h"

// Uses SPI To communicate with CAN Controller
#include "SPI.h"

// Interrupts
#include <avr/interrupt.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

static uint8_t * p_My_CAN_Data;             // Pointer to this node's SPI Data address     

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        MS_CAN_Initialize

    Parameters
        uint8_t * p_spi_data

    Description
        Initializes the CAN Controller
        [MUST BE RUN AFTER SPI INITIALIZATION!]

****************************************************************************/
void MS_CAN_Initialize(uint8_t * p_spi_data)
{
    // Point the CAN Data to the SPI data buffer
    p_My_CAN_Data = p_spi_data;

    // Reset CAN Module
    // CAN_Reset();

    // Enter Configuration Mode
    // CAN_Enter_Config();


}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

void Reset_SPI_Buffer(void)
{
    // Resets all the data bytes in SPI Data buffer to 0
    for (int i = 0; i < MAX_SPI_DATA_LENGTH; i++)
    {
        *(p_My_CAN_Data[0] + i) = 0;
    }
}

void CAN_Reset(void)
{
    // Reset SPI Buffer
    Reset_SPI_Buffer();

    // Set reset command
    *(p_My_CAN_Data[0]) = CAN_RESET;

    // Send Command w/ correct command length
    SPI_Transmit(CAN_RESET_LENGTH);     
}

void CAN_Enter_Config(void)
{
    // Reset SPI Buffer
    Reset_SPI_Buffer();

    // Set reset command
    *(p_My_CAN_Data[0]) = CAN_WRITE;
    *(p_My_CAN_Data[1]) = 0x80; // 10000000 Set to Config Mode, One Shot Disabled

    // Send Command w/ correct command length
    SPI_Transmit(CAN_WRITE_LENGTH);
}

void CAN_Set_Bit_Time(void)
{
    // Reset SPI Buffer
    Reset_SPI_Buffer();

    // Set reset command
    *(p_My_CAN_Data[0]) = CAN_WRITE;
    *(p_My_CAN_Data[1]) = 0x80; // 10000000 Set to Config Mode, One Shot Disabled

    // Send Command w/ correct command length
    SPI_Transmit(CAN_WRITE_LENGTH);
}

void CAN_Set_ID(uint8_t Buffer_ID)
{
    // Reset SPI Buffer
    Reset_SPI_Buffer();

    // Set reset command
    *(p_My_CAN_Data[0]) = CAN_WRITE;
    *(p_My_CAN_Data[1]) = 0x80; // 10000000 Set to Config Mode, One Shot Disabled

    // Send Command w/ correct command length
    SPI_Transmit(CAN_WRITE_LENGTH);
}
/*******************************************************************************
    File:
        SPI.c
 
    Notes:
        This file contains the SPI module. This module initializes the SPI hardware,
        and provides access to transmit and receive functions.

    External Functions Required:

    Public Functions:
        void Init_SPI_Module(void)
        void SPI_MasterTransmit(char *TX_Data)
                
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

// Receive variable that is set to expected SPI Data Length
static char RX_Data[SPI_DATA_LENGTH] = {0};

// A tracker to know which RX_Data byte is being filled
// Should be reset to 0 after every SS raised high
static uint8_t RX_Data_Index = 0;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Init_SPI_Module

    Parameters
        uint8_t Master_Slave_Identifier

    Description
        Initializes the SPI module based on whether requested setup is for
        master or slave

****************************************************************************/
void Init_SPI_Module(uint8_t Master_Slave_Identifier)
{
    if (SPI_MASTER == Master_Slave_Identifier)
    {
        // SPI Data Direction Register (DDR_SPI) = DDRA
        // Set MOSI and SCK output, all others input
        DDR_SPI = (1<<MOSI)|(1<<SCK);

        // Disable interrupt on transmission complete, enable SPI and set as master,
        // set clock rate io_clk/4, MSB transmitted first, Sample on rising edge 
        SPCR = (1<<SPE)|(1<<MSTR);

        // Configure PA7 to be slave select from master side (Only for Tiny, change
        // for PIC)
        DDRA |= (1<<SS_MASTER);
        // Raise SS from master at Init
        PORTA |= (1<<SS_MASTER);
    }
    else if (SPI_SLAVE == Master_Slave_Identifier)
    {
        // SPI Data Direction Register (DDR_SPI) = DDRA
        // Set MISO output, all others input
        DDR_SPI = (1<<MISO);

        // Enable interrupt on transmission complete, enable SPI and set as slave,
        // set clock rate io_clk/4, MSB transmitted first, Sample on rising edge
        // Slave requires interrupt response as we do not when transmission might
        // occur.
        SPCR = (1<<SPIE)|(1<<SPE);
    }
    else
    {
        // Ignore SPI Setup
    }
}

/****************************************************************************
    Public Function
        SPI_MasterTransmit

    Parameters
        char *TX_Data

    Description
        Transmits the TX Data array bytes through SPI

****************************************************************************/

void SPI_MasterTransmit(char *TX_Data)
{
    // Set slave select low to indicate start of transmission
    PORTA &= ~(1<<SS_MASTER);

    // Loop over specified SPI Data Length
    for (uint8_t tx_index = 0; tx_index < SPI_DATA_LENGTH; tx_index++)
    {
        // Start transmission of data byte
        SPDR = *(TX_Data + tx_index);
        // Wait for transmission to complete
        while(!(SPSR & (1<<SPIF)));
    }

    // Set slave select line high to reset transmission
    PORTA |= (1<<SS_MASTER);
}

/****************************************************************************
    Public Function
        Get_RX_Data

    Parameters
        
    Description
        Returns RX_Data to caller

****************************************************************************/

char* Get_RX_Data(void)
{
    return &RX_Data[0];
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

/****************************************************************************
    ISR
        SPI Interrupt Handlers: SPI_STC_vect

    Parameters
        None

    Description
        Handles SPI transmission received interrupts
        Will fire only on slave side

****************************************************************************/

ISR(SPI_STC_vect)
{
    // Clear the SPI Interrupt Flag (is done by reading the SPSR Register)
    uint8_t SPSR_Status = SPSR;

    // Read and set RX_Data
    RX_Data[RX_Data_Index] = SPDR;

    // Increment RX_Data_Index
    RX_Data_Index++;

    if ((SPI_DATA_LENGTH == RX_Data_Index) || (SS_BIT_HI == (PORTA&SS_BIT_HI)))
    {
        RX_Data_Index = 0;
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

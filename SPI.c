/*******************************************************************************
    File:
        SPI.c
 
    Notes:
        This file contains the SPI module. This module initializes the SPI hardware,
        and provides access to transmit and receive functions.

    External Functions Required:

    Public Functions:
        void MS_SPI_Initialize(uint8_t * p_this_node_id, uint8_t * p_spi_data)
        void SPI_Transmit(uint8_t Max_Data_Length_Expected)
                
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

static uint8_t * p_My_SPI_Data;             // Pointer to this node's SPI Data address
static uint8_t Master_Slave_Identifier = 0; // Slave or master?
static uint8_t data_index = 0;              // Count at which byte of transmission
static uint8_t Expected_Data_Length = 0;    // Set as Max Expected number of bytes (transmit or receive)
/*
static bool In_Tx = true;                   // Indicates currently transmitting     
static bool Expecting_Response = true;      // Indicates that a response from slave is expected        
*/      

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        MS_SPI_Initialize

    Parameters
        uint8_t * p_this_node_id
        uint8_t * p_spi_data

    Description
        Initializes the SPI module as a master/slave and sets SPI TX/RX buffer
        address

****************************************************************************/
void MS_SPI_Initialize(uint8_t * p_this_node_id, uint8_t * p_spi_data)
{
    // Identify node type
    Master_Slave_Identifier = *p_this_node_id;
    
    // Point to actual data source 
    p_My_SPI_Data = p_spi_data;

    if (SPI_MASTER == Master_Slave_Identifier)
    {
        // SPI Data Direction Register (DDR_SPI) = DDRA
        // Set MOSI, SS and SCK output, all others input
        DDR_SPI |= (1<<MOSI)|(1<<SCK)|(1<<SS);

        // Enable interrupt on transmission complete, enable SPI and set as master,
        // set clock rate io_clk/4, MSB transmitted first, Sample on rising edge 
        SPCR = (1<<SPIE)|(1<<SPE)|(1<<MSTR);

        // Raise SS from master at Init
        PORTA |= (1<<SS);

        // Reset data index
        data_index = 0;
    }
    else
    {
        // SPI Data Direction Register (DDR_SPI) = DDRA
        // Set MISO output, all others input
        DDR_SPI |= (1<<MISO);

        // Enable interrupt on transmission complete, enable SPI and set as slave,
        // set clock rate io_clk/4, MSB transmitted first, Sample on rising edge
        // Slave requires interrupt response as we do not when transmission might
        // occur.
        SPCR = (1<<SPIE)|(1<<SPE);

        // Reset data index
        data_index = 0;
    }
}

/****************************************************************************
    Public Function
        SPI_Transmit

    Parameters
        Max_Data_Length_Expected

    Description
        Transmits the TX Data array bytes through SPI

****************************************************************************/

void SPI_Transmit(uint8_t Max_Data_Length_Expected)
{
    // Reset data index
    data_index = 0;

    Expected_Data_Length = Max_Data_Length_Expected;

    // Set slave select low to indicate start of transmission
    PORTA &= ~(1<<SS);

    // Send first byte out
    SPDR = *(p_My_SPI_Data + data_index);
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
    if (Master_Slave_Identifier == SPI_MASTER)
    {
        // Clear the SPI Interrupt Flag (is done by reading the SPSR Register)
        uint8_t SPSR_Status = SPSR;

        // Read and set RX_Data
        *(p_My_SPI_Data + data_index) = SPDR;

        // Increment data index
        data_index++;

        // If all data has been sent
        if (data_index == Expected_Data_Length)
        {
            // Reset Data index counter
            data_index = 0;

            // End transmission by pulling SS high
            PORTA |= (1<<SS);
        }
        else
        {
            // Send consecutive bytes out
            SPDR = *(p_My_SPI_Data + data_index);
        }
    }
    else
    {
        // Needs to be discussed

        // Currently not being used, will worry about later        
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

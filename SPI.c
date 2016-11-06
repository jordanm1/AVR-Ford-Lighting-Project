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

static uint8_t Master_Slave_Identifier = 0; // Slave or master?
static uint8_t RX_Index = 0;				// Count RX_Bytes
static uint8_t TX_Index = 0;				// Count TX_Bytes
static uint8_t Expected_TX_Length = 0;	    // Set as Max TX Bytes
static uint8_t Expected_RX_Length = 0;	    // Set as Max RX Bytes

static uint8_t SPI_TX_Buffer[12] = {0};		// [0]: TX Msg Length, [1]: RX Msg Length
											// [2]-[11]: Data Bytes		
static uint8_t SPI_RX_Buffer[10] = {0};		// [0]-[9]: Data Bytes 		

static bool In_Tx = false;					// This flag decides which buffer to fill
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

    Description
        Initializes the SPI module as a master/slave and sets SPI TX/RX buffer
        address

****************************************************************************/
void MS_SPI_Initialize(uint8_t * p_this_node_id)
{
    // Identify node type
    Master_Slave_Identifier = *p_this_node_id;
    
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

        // Reset indices index
        TX_Index = 0;
		RX_Index = 0;
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

        // Reset indices index
        TX_Index = 0;
        RX_Index = 0;
    }
}

/****************************************************************************
    Public Function
        SPI_Start_Command

    Parameters

    Description
        Resets SPI parameters before beginning a command

****************************************************************************/

void SPI_Start_Command (void)
{
	Expected_TX_Length = *(SPI_TX_Buffer + TX_LNTH_INDX);
	Expected_RX_Length = *(SPI_TX_Buffer + RX_LNTH_INDX);
	
	// Set RX data index
	RX_Index = 0
	
    // Set TX data index
    TX_Index = 0
	
	// State in TX
	In_Tx = true;

    // Set slave select low to indicate start of transmission
    PORTA &= ~(1<<SS);
}

/****************************************************************************
    Public Function
        SPI_End_Command

    Parameters

    Description
        Ends SPI Transmission by setting SS high

****************************************************************************/

void SPI_End_Command (void)
{
    // Set slave select high to indicate end of transmission
    PORTA |= (1<<SS);
}


/****************************************************************************
    Public Function
        SPI_Transmit

    Parameters

    Description
        Transmits the TX Data array bytes through SPI

****************************************************************************/

void SPI_Transmit (void)
{
    // Send byte out
    SPDR = *(SPI_TX_Buffer + TX_Index);
	
	if (In_Tx)
	{
		// Increment Transmit Index
		TX_Index++;		
	}
	else
	{
		// Increment Receive Index
		RX_Index++;
		// Increment Transmit Index
		TX_Index++;
	}
}

/****************************************************************************
    Public Function
        Get_SPI_TX_Buffer

    Parameters
        

    Description
        Returns SPI TX Buffer address
		
****************************************************************************/

uint8_t* Get_SPI_TX_Buffer(void)
{
	// Return start address
	return &SPI_TX_Buffer[0];
}

/****************************************************************************
    Public Function
        Get_SPI_RX_Buffer

    Parameters
        

    Description
        Returns SPI RX Buffer address
		
****************************************************************************/

uint8_t* Get_SPI_RX_Buffer(void)
{
	// Return start address
	return &SPI_RX_Buffer[0];
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
		
		// Once a transmit has been completed
		if (In_Tx)
		{
			if (TX_Index < Expected_TX_Length)
			{
				Post_Event(EVT_SEND_BYTE);
			}
			else if (TX_Index == Expected_TX_Length)
			{
				Post_Event(EVT_SEND_BYTE);'
			}
			else
			{
				In_Tx = false;
			}
		}
		
		if (!In_Tx)
		{
			*(SPI_RX_Buffer + RX_Index) = SPDR;
			RX_Index++;
			
			if (RX_Index < Expected_RX_Length)
			{
				Post_Event(EVT_RECV_BYTE);
			}
			else if (RX_Index == Expected_RX_Length)
			{
				Post_Event(EVT_END_TRANS);
			}
			else
			{
				// Do Nothing. Should never get here.
			}		
		}
    }
	else
	{
		// Not configured to be slave
	}
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

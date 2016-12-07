/*******************************************************************************
    File:
        UART.c
 
    Notes:
        This file contains the UART module. This module initializes the UART hardware,
        and provides access to transmit and receive functions.

    External Functions Required:

    Public Functions:
        void UART_Initialize(uint8_t * p_this_node_id, uint8_t * p_UART_data)
        void UART_Transmit(uint8_t Max_Data_Length_Expected)
                
*******************************************************************************/

// #############################################################################
// ------------ INCLUDES
// #############################################################################

// Standard ANSI  99 C types for exact integer sizes and booleans
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Config file
#include "config.h"

// Framework
#include "framework.h"

// This module's header file
#include "UART.h"

// Include UART Service Header
#include "UART_Service.h"

// Interrupts
#include <avr/interrupt.h>

#include "IOC.h"

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

static uint8_t Command_Buffer[COMMAND_BUFFER_SIZE][MAX_COMMAND_TX_SIZE]; // Create a command buffer to prevent overwrite
static uint8_t * Receive_List[COMMAND_BUFFER_SIZE][MAX_COMMAND_RX_SIZE]; // List of pointers whose variables will be assigned based on UART receive
static uint8_t Buffer_Index = 0; // Command buffer index variable
static uint8_t Next_Available_Row = 0; // Provides row to fill in next command

static uint8_t RX_Index = 0;				// Count RX_Bytes
static uint8_t TX_Index = 0;				// Count TX_Bytes
static uint8_t Expected_TX_Length = 0;	    // Set as Max TX Bytes
static uint8_t Expected_RX_Length = 0;	    // Set as Max RX Bytes

static bool In_Tx = false;					// This flag decides which buffer to fill

static uint32_t counter_value = 0; 

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

static void Reset_Command_Receive_Buffer(void);
static void Update_Buffer_Index(void);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        UART_Initialize

    Parameters
        

    Description
        Initializes the UART module as a master/slave and sets UART TX/RX buffer
        address

****************************************************************************/
void UART_Initialize(uint8_t * p_this_node_id)
{    
	//***************************@TODO*************************************
	
    // UART Data Direction Register (DDR_UART) = DDRA
    // Set MOSI, SS and SCK output, all others input
    DDR_UART |= (1<<MOSI)|(1<<SCK)|(1<<SS);
	//DDR_UART &= ~(1<<SS);

    // Enable interrupt on transmission complete, enable UART and set as master,
    // set clock rate io_clk/4, MSB transmitted first, Sample on rising edge 
    SPCR = (1<<UARTE)|(1<<SPE)|(1<<MSTR);

    // Raise SS from master at Init
    PORTA |= (1<<SS);

    // Reset indices
    Buffer_Index = 0;
    TX_Index = 0;
	RX_Index = 0;

    Reset_Command_Receive_Buffer();
}

/****************************************************************************
    Public Function
        UART_Start_Command

    Parameters

    Description
        Resets UART parameters before beginning a command

****************************************************************************/

void UART_Start_Command (void)
{
	Expected_TX_Length = Command_Buffer[Buffer_Index][TX_LENGTH_BYTE];
	Expected_RX_Length = Command_Buffer[Buffer_Index][RX_LENGTH_BYTE];
	
	// Set RX data index
	RX_Index = 0;
	
    // Set TX data index
    TX_Index = 0;
	
	// State in TX
	In_Tx = true;

	
	//***************************@TODO*************************************

    // Set slave select low to indicate start of transmission
    PORTA &= ~(1<<SS);
}

/****************************************************************************
    Public Function
        UART_End_Command

    Parameters

    Description
        Ends UART Transmission by setting SS high

****************************************************************************/

void UART_End_Command (void)
{
	//***************************@TODO*************************************
	
    // Set slave select high to indicate end of transmission
    PORTA |= (1<<SS);
}


/****************************************************************************
    Public Function
        UART_Transmit

    Parameters

    Description
        Transmits the TX Data array bytes through UART

****************************************************************************/

void UART_Transmit (void)
{
	//***************************@TODO*************************************
    // Send byte out
    SPDR = Command_Buffer[Buffer_Index][TX_Index + LENGTH_BYTES];
	
	if (In_Tx)
	{
		// Increment Transmit Index
		TX_Index++;		
	}
	else
	{
		/*
		// Increment Receive Index
		RX_Index++;
		*/
		// Increment Transmit Index
		TX_Index++;
	}
}

/****************************************************************************
    Public Function
        Write_UART

    Parameters
        uint8_t TX_Length		Buffer_Index	0x07	uint8_t{data}@0x0138

        uint8_t RX_Length
        uint8_t* Data_To_Write

    Description
        Fills in current command into UART command buffer
****************************************************************************/

void Write_UART(uint8_t TX_Length, uint8_t RX_Length, uint8_t * Data2Write, uint8_t ** Data2Receive)
{
	counter_value = query_counter();
	
    // Over all columns of next available command row
    for (int i = 0; i < (LENGTH_BYTES + TX_Length); i++)
    {
        // Fill in expected TX length
        if (i == TX_LENGTH_BYTE)
        {
            Command_Buffer[Next_Available_Row][TX_LENGTH_BYTE] = TX_Length;
        }
        // Fill in expected RX Length
        else if (i == RX_LENGTH_BYTE)
        {
            Command_Buffer[Next_Available_Row][RX_LENGTH_BYTE] = RX_Length;
        }
        // Fill in remaining data to TX
        else
        {
            Command_Buffer[Next_Available_Row][i] = *(Data2Write + (i - LENGTH_BYTES));
        }   
    }
    // Data is expected to be received
    if (RX_Length > 0)
    {
        for (int i = 0; i < RX_Length; i++)
        {
            // Add pointers to variables that shall be updated with receive data
            Receive_List[Next_Available_Row][i] = *(Data2Receive + i);
        }
    }
    // If reached Command Buffer end
    if (Next_Available_Row == COMMAND_BUFFER_SIZE - 1)
    {
        Next_Available_Row = 0;
    }
    else
    {
        Next_Available_Row++;
    }
    // If UART is currently idling, start transmission
    if (Query_UART_State() == NORMAL_STATE)
    {
        Post_Event(EVT_UART_START);
    }
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

/****************************************************************************
    ISR
        UART Interrupt Handlers: UART_STC_vect

    Parameters
        None

    Description
        Handles UART transmission completed interrupts

****************************************************************************/

ISR(UART_STC_vect)
{
	//***************************@TODO*************************************
    // Clear the UART Interrupt Flag (is done by reading the SPSR Register)
    uint8_t SPSR_Status = SPSR;
		
	// Once a transmit has been completed
	if (In_Tx)
	{
        // If more bytes left to transmit post transmission event
		if (TX_Index <= Expected_TX_Length)
		{
			if ((TX_Index == Expected_TX_Length) && Expected_RX_Length == 0)
			{
				In_Tx = false;									
			}
			else
			{
				Post_Event(EVT_UART_SEND_BYTE);
			}
		}
		else
		{
			In_Tx = false;
		}
	}
		
	if (!In_Tx)
	{
		if (Expected_RX_Length > 0)
		{
			*(Receive_List[Buffer_Index][RX_Index]) = SPDR;
			RX_Index++;				
		}
		if (RX_Index < Expected_RX_Length)
		{
			Post_Event(EVT_UART_RECV_BYTE);
		}
		else if (RX_Index >= Expected_RX_Length)
		{
            Update_Buffer_Index();
			Post_Event(EVT_UART_END);
		}
		else
		{
			// Do Nothing. Should never get here.
		}		
	}
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        Reset_Command_Receive_Buffer

    Parameters
        None

    Description
        Resets all the command buffer bytes to be 0xff
        Resets receive list pointers to NULL

****************************************************************************/

static void Reset_Command_Receive_Buffer(void)
{
    for (int row = 0; row < COMMAND_BUFFER_SIZE; row++)
    {
        // Set all bytes of Command_Buffer to 0xff = UNASSIGNED
        for (int col = 0; col < MAX_COMMAND_TX_SIZE; col++)
        {
            Command_Buffer[row][col] = 0xFF;    // Set as unassigned
        }
        // Set all pointers of Receive List to NULL 
        for (int col = 0; col < MAX_COMMAND_RX_SIZE; col++)
        {
            Receive_List[row][col] = NULL;     // Set as unassigned
        }
    }
}

/****************************************************************************
    Private Function
        Update_Buffer_Index

    Parameters
        None

    Description
        Resets current row for Command Buffer and Receive List and updates
        buffer index as required

****************************************************************************/

static void Update_Buffer_Index(void)
{
    // Set current row of command buffer to unassigned (0xFF)
    for (int i = 0; i < MAX_COMMAND_TX_SIZE; i++)
    {
        Command_Buffer[Buffer_Index][i] = 0xFF;
    }
    // Point current receive list row to NULL
    for (int i = 0; i < MAX_COMMAND_RX_SIZE; i++)
    {
        Receive_List[Buffer_Index][i] = NULL;
    }
    // If at end of buffer
    if (Buffer_Index == COMMAND_BUFFER_SIZE - 1)
    {
        Buffer_Index = 0;
    }
    else
    {
        Buffer_Index++;
    }
    // If buffer has pending transmits
    if (Command_Buffer[Buffer_Index][TX_LENGTH_BYTE] != 0xFF)
    {
        Post_Event(EVT_UART_START);
    }
}
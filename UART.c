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

// Program Memory
#include <avr/pgmspace.h>

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

static uint8_t Command_Buffer[UART_COMMAND_BUFFER_SIZE][UART_MAX_COMMAND_TX_SIZE]; // Create a command buffer to prevent overwrite
static uint8_t * Receive_List[UART_RX_BUFFER_SIZE][UART_MAX_COMMAND_RX_SIZE]; // List of pointers whose variables will be assigned based on UART receive
static uint8_t Buffer_Index = 0; // Command buffer index variable
static uint8_t Next_Available_Row = 0; // Provides row to fill in next command

static uint8_t RX_Index = 0;				// Count RX_Bytes
static uint8_t TX_Index = 0;				// Count TX_Bytes
static uint8_t Expected_TX_Length = 0;	    // Set as Max TX Bytes
static uint8_t Expected_RX_Length = 0;	    // Set as Max RX Bytes

static uint8_t Text_Index = 0;

static bool In_Tx = false;					// This flag decides which buffer to fill
static bool In_Rx = false;

static bool modem_init = true;

static uint32_t counter_value = 0; 

static bool first_byte_T = false;
static bool second_byte_slash_n = false;
static bool third_slash_r = false;

static uint8_t byte_space_counter = 0;

static uint8_t *Modem_Recv_Data;

//static const char Init_Text[149] PROGMEM = "ATE1/rAT^SICA=1,3/rAT^SISS=0,\"srvType\",\"socket"
										  //"\"/rAT^SISS=0,\"conId\",3/rAT^SISS=0,\"address\""
										  //",\"socktcp://listener:2000;etx=26;autoconnect"
										  //"=1\"/rAT^SISO=0/r";
//static const char Init_Text[10] PROGMEM = "AT^SICA?\r";			
static const char Init_Text[10] PROGMEM = "AT^SISS?\r";			
//static const char Init_Text[13] PROGMEM = "AT+CGDCONT?\r";	
//static const char Init_Text[40] PROGMEM = "AT+CGDCONT=3,\"IPV4V6\",\"MW01.VZWSTATIC\"/r";				
//static const char Init_Text[11] PROGMEM = "AT^SISC=0\r";		
//static const char Init_Text[11] PROGMEM = "AT^SISO=0\r";		

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
void UART_Initialize(uint8_t *pCanTXPacket)
{    	
	// Rx -> A0
	// Tx -> A1
	
	//DDRA &= ~(1<<PINA0);
	//PORTA &= ~(1<<PINA0);
	PORTB &= ~(1<<PINB1);
	DDRB |= (1<<PINB1);
	PORTB &= ~(1<<PINB5);
	DDRB |= (1<<PINB5);
	
	// Reset UART
	LINCR |= (1<<LSWRES);
	
    // Set up LINCR with
	// - Command Mode = TxRx Enabled
	// - UART Enable
	// - Odd Parity	
	LINCR = (1<<LCMD0)|(1<<LCMD1)|(1<<LCMD2)|(1<<LENA)|(0<<LCONF0)|(0<<LCONF1);
	
	// Set up LINBTR
	LINBTR = (0<<LDISR);
	
	// 19200 baud
	LINBRRL = 12;//(1<<LDIV0);
	LINBRRH = 0;
    
	// LIN Interrupt Enable
	LINENIR = (1<<LENERR)|(1<<LENTXOK)|(1<<LENRXOK);
	//LINENIR = (1<<LENTXOK)|(1<<LENRXOK);
	// Reset indices
    Buffer_Index = 0;
    TX_Index = 0;
	RX_Index = 0;
	
	Modem_Recv_Data = pCanTXPacket;
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
	Expected_TX_Length = Command_Buffer[Buffer_Index][UART_TX_LENGTH_BYTE];
	Expected_RX_Length = Command_Buffer[Buffer_Index][UART_RX_LENGTH_BYTE];
	
	// Set RX data index
	RX_Index = 0;
	
    // Set TX data index
    TX_Index = 0;
	
	// State in TX
	In_Tx = true;
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
	// Do Nothing
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
	if (!modem_init)
	{
		// Send byte out
		LINDAT = Command_Buffer[Buffer_Index][TX_Index + UART_LENGTH_BYTES];
		TX_Index++;
		
		//if (In_Tx)
		//{
			//// Increment Transmit Index
			//TX_Index++;		
		//}
		//else
		//{
			//// Increment Transmit Index
			//TX_Index++;
		//}
	}
	else
	{
		char set_as;
		if (pgm_read_byte(&(Init_Text[Text_Index])) == '/')
		{
			// If an escape sequence, skip slash and send special character
			if (pgm_read_byte(&(Init_Text[Text_Index+1])) == 'r') {
				set_as = 0x0D;
				Text_Index++;
			}
			
			else {
				set_as = '/';	
			}
		}
		else
		{
			set_as = pgm_read_byte(&(Init_Text[Text_Index]));	
		}
		LINDAT = set_as;
		Text_Index++;
		
		//int i = 0;
		//while (true) {
			//i++;
			//if (i > 20000) break;
		//}
	}
}

/****************************************************************************
    Public Function
        Write_UART

    Parameters
        uint8_t TX_Length	
        uint8_t RX_Length
        uint8_t* Data_To_Write

    Description
        Fills in current command into UART command buffer
****************************************************************************/

void Write_UART(uint8_t TX_Length, uint8_t RX_Length, uint8_t * Data2Write, uint8_t ** Data2Receive, bool Init_Seq)
{
	if (!Init_Seq)
	{
		modem_init = false;
		counter_value = query_counter();
		
		// Over all columns of next available command row
		for (int i = 0; i < (UART_LENGTH_BYTES + TX_Length); i++)
		{
			// Fill in expected TX length
			if (i == UART_TX_LENGTH_BYTE)
			{
				Command_Buffer[Next_Available_Row][UART_TX_LENGTH_BYTE] = TX_Length;
			}
			// Fill in expected RX Length
			else if (i == UART_RX_LENGTH_BYTE)
			{
				Command_Buffer[Next_Available_Row][UART_RX_LENGTH_BYTE] = RX_Length;
			}
			// Fill in remaining data to TX
			else
			{
				Command_Buffer[Next_Available_Row][i] = *(Data2Write + (i - UART_LENGTH_BYTES));
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
		if (Next_Available_Row == UART_COMMAND_BUFFER_SIZE - 1)
		{
			Next_Available_Row = 0;
		}
		else
		{
			Next_Available_Row++;
		}
		// If UART is currently idling, start transmission
		if (Query_UART_State() == NORMAL_UART_STATE)
		{
			Post_Event(EVT_UART_START);
		}
	}
	else
	{
		modem_init = true;
		Text_Index = 0;
		UART_Transmit();
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
ISR(LIN_ERR_vect)
{
	PORTB |= (1<<PINB1);
	LINSIR |= (1<LERR);
}

ISR(LIN_TC_vect)
{
	//LINSIR = (1<<3)|(1<<2)|(1<<1)|(1<<0);
	// Received a byte
	if (LINSIR & RX_ISR_FLAG)	
	//if (!modem_init && !In_Tx)
	{
		LINSIR |= RX_ISR_FLAG;	// Clear Receive Flag

						
		// If the correct sequence was received, prepare to store packet sent from phone
		if (first_byte_T && second_byte_slash_n && third_slash_r)
		{
			Modem_Recv_Data[byte_space_counter - 1] = LINDAT;
			byte_space_counter++;
			
			if (byte_space_counter >= MAX_MODEM_RECEIVE + 1)
			{
				PORTB ^= (1<<PINB5);
				first_byte_T = false;
				second_byte_slash_n = false;
				third_slash_r = false;
				Post_Event(EVT_MODEM_NEW_PACKAGE);
			}
		}
		
		// Check for correct sequence from modem to see if a packet is coming
		else
		{
			//PORTB &= ~(1<<PINB5);
			uint8_t Current_Read = LINDAT;
			if (Current_Read == 'T')
			{
				byte_space_counter = 0;
				first_byte_T = true;
			}
			else if (first_byte_T && byte_space_counter == 1 && Current_Read == 0x0D)
			{
				second_byte_slash_n = true;
				byte_space_counter = 0;
			}
			else if (first_byte_T && second_byte_slash_n && byte_space_counter == 1 && Current_Read == 0x0A)
			{
				third_slash_r = true;
				byte_space_counter = 0;
			}
			else {
				first_byte_T = false;
				second_byte_slash_n = false;
				third_slash_r = false;
			}
			byte_space_counter++;
		}
	}
	
	// Completed transmission of a byte
	else if (LINSIR && TX_ISR_FLAG)
	{
		LINSIR |= TX_ISR_FLAG;	// Clear Transmit flag
		if (!modem_init)
		{
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
					*(Receive_List[Buffer_Index][RX_Index]) = LINDAT;
					RX_Index++;
				}
				if (RX_Index < Expected_RX_Length)
				{
					// Do Nothing
					// Post_Event(EVT_UART_RECV_BYTE);
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
		
		// If in startup sequence for modem commands
		else
		{
			if (Text_Index >= sizeof(Init_Text)/sizeof(Init_Text[0]))
			{
				modem_init = false;
			}
			else
			{
				int i = 0;
				//while (true) {
					//i++;
					//if (i > 20000) break;
				//}
				UART_Transmit();
			}
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
    for (int row = 0; row < UART_COMMAND_BUFFER_SIZE; row++)
    {
        // Set all bytes of Command_Buffer to 0xff = UNASSIGNED
        for (int col = 0; col < UART_MAX_COMMAND_TX_SIZE; col++)
        {
            Command_Buffer[row][col] = 0xFF;    // Set as unassigned
        }
        // Set all pointers of Receive List to NULL 
        for (int col = 0; col < UART_MAX_COMMAND_RX_SIZE; col++)
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
    for (int i = 0; i < UART_MAX_COMMAND_TX_SIZE; i++)
    {
        Command_Buffer[Buffer_Index][i] = 0xFF;
    }
    // Point current receive list row to NULL
    for (int i = 0; i < UART_MAX_COMMAND_RX_SIZE; i++)
    {
        Receive_List[Buffer_Index][i] = NULL;
    }
    // If at end of buffer
    if (Buffer_Index == UART_COMMAND_BUFFER_SIZE - 1)
    {
        Buffer_Index = 0;
    }
    else
    {
        Buffer_Index++;
    }
    // If buffer has pending transmits
    if (Command_Buffer[Buffer_Index][UART_TX_LENGTH_BYTE] != 0xFF)
    {
        Post_Event(EVT_UART_START);
    }
}
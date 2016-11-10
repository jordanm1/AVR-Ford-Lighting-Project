/*******************************************************************************
    File:
        CAN.c
 
    Notes:
        This file contains the CAN Module for MCP25625

    External Functions Required:

    Public Functions:
        
                
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
#include "SPI.h"

// Include SPI Service Header
#include "SPI_Service.h"

// Include MCP25625 Header
#include "MCP25625defs.h"

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
		

    Description
        Initializes the CAN module MCP25625

****************************************************************************/
void MS_CAN_Initialize(void)
{
	
}

/****************************************************************************
    Public Function
        CAN_Reset

    Parameters
		

    Description
        Resets the MCP25625 and sends it into config mode

****************************************************************************/

void CAN_Reset(void)
{
	// Define constants
	#define RESET_TX_LENGTH 1
	#define RESET_RX_LENGTH 0
	
	uint8_t Data_2_Write[RESET_TX_LENGTH] = {MCP_RESET};
	
	// Call SPI command
	Write_SPI(RESET_TX_LENGTH, RESET_RX_LENGTH, Data_2_Write, NULL);
}

/****************************************************************************
    Public Function
        CAN_Read

    Parameters
		uint8_t Register_2_Read
		uint8** Variable_2_Set
		
    Description
        Sends read command to CAN Module

****************************************************************************/

void CAN_Read(uint8_t Register_2_Read, uint8_t** Variable_2_Set)
{
	// Define constants
	#define READ_TX_LENGTH 2
	#define READ_RX_LENGTH 1
	
	// Set read information
	uint8_t Data_2_Write[READ_TX_LENGTH] = {MCP_READ, Register_2_Read};
	
	// Call SPI command
	Write_SPI(READ_TX_LENGTH, READ_RX_LENGTH, Data_2_Write, Variable_2_Set);
}

/****************************************************************************
    Public Function
        CAN_Read_RX_Buffer

    Parameters
		bool choice : True, read from Buffer 0
					  False, read from Buffer 1
		uint8** Variable_2_Set

    Description
        Sends read command to CAN Module

****************************************************************************/

void CAN_Read_RX_Buffer(bool choice, uint8_t** Variable_2_Set)
{
	// Define constants
	#define READ_BUFFER_TX_LENGTH 1
	#define READ_BUFFER_RX_LENGTH 1
	
	uint8_t Data_2_Write[READ_BUFFER_TX_LENGTH];
	
	// Set read information
	if (choice)
	{
		Data_2_Write[0] = MCP_READ_RX0;		
	}
	else
	{
		Data_2_Write[0] = MCP_READ_RX1;
	}
	
	// Call SPI command
	Write_SPI(READ_BUFFER_TX_LENGTH, READ_BUFFER_RX_LENGTH, Data_2_Write, Variable_2_Set);
}

/****************************************************************************
    Public Function
        CAN_Write

    Parameters
		uint8_t Register_2_Set
		uint8_t** Value_2_Set

    Description
        Sends write command to CAN Module

****************************************************************************/

void CAN_Write(uint8_t Register_2_Set, uint8_t* Value_2_Set)
{
	// Define constants
	#define WRITE_TX_LENGTH 3
	#define WRITE_RX_LENGTH 0	
	
	// Value to Set
	uint8_t Data_2_Write[WRITE_TX_LENGTH] = {MCP_WRITE, Register_2_Set, Value_2_Set[0]};
	
	// Call SPI command
	Write_SPI(WRITE_TX_LENGTH, WRITE_RX_LENGTH, Data_2_Write, NULL);
}

/****************************************************************************
    Public Function
        CAN_Load_TX_Buffer

    Parameters
		uint8_t choice (1: Buffer 0, 2: Buffer 1, 3: Buffer 2)
		uint8_t* Value_2_Set

    Description
        Sends write command to CAN Module

****************************************************************************/

void CAN_Load_TX_Buffer(uint8_t choice, uint8_t* Value_2_Set)
{
	// Define constants
	#define LOAD_TX_LENGTH 2
	#define LOAD_RX_LENGTH 0
	
	uint8_t Data_2_Write[LOAD_TX_LENGTH];
	
	// Value to Set
	if (choice == 1)
	{
		Data_2_Write[0] = MCP_LOAD_TX0;
		Data_2_Write[1] = Value_2_Set[0];
	}
	else if (choice == 2)
	{
		Data_2_Write[0] = MCP_LOAD_TX1;
		Data_2_Write[1] = Value_2_Set[0];	
	}
	else
	{
		Data_2_Write[0] = MCP_LOAD_TX2;
		Data_2_Write[1] = Value_2_Set[0];	
	}
	
	// Call SPI command
	Write_SPI(LOAD_TX_LENGTH, LOAD_RX_LENGTH, Data_2_Write, NULL);
}

/****************************************************************************
    Public Function
        CAN_RTS

    Parameters
		uint8_t choice (1: Buffer 0, 2: Buffer 1, 3: Buffer 2)

    Description
        Sends RTS command to CAN Module

****************************************************************************/

void CAN_RTS(uint8_t choice)
{
	// Define constants
	#define RTS_TX_LENGTH 1
	#define RTS_RX_LENGTH 0
	
	uint8_t Data_2_Write[RTS_TX_LENGTH];
	
	// Value to Set
	if (choice == 1)
	{
		Data_2_Write[0] = MCP_RTS_TX0;
	}
	else if (choice == 2)
	{
		Data_2_Write[0] = MCP_RTS_TX1;
	}
	else
	{
		Data_2_Write[0] = MCP_RTS_TX2;
	}
		
	// Call SPI command
	Write_SPI(RTS_TX_LENGTH, RTS_RX_LENGTH, Data_2_Write, NULL);
}

/****************************************************************************
    Public Function
        CAN_Read_Status

    Parameters
		uint8_t** Variable_2_Set

    Description
        Acquires status from CAN Module

****************************************************************************/

void CAN_Read_Status(uint8_t** Variable_2_Set)
{
	// Define constants
	#define STATUS_TX_LENGTH 1
	#define STATUS_RX_LENGTH 1
	
	// Value to Set
	uint8_t Data_2_Write[STATUS_TX_LENGTH] = {MCP_READ_STATUS};
	
	// Call SPI command
	Write_SPI(STATUS_TX_LENGTH, STATUS_RX_LENGTH, Data_2_Write, Variable_2_Set);
}

/****************************************************************************
    Public Function
        CAN_RX_Status

    Parameters
		uint8_t** Variable_2_Set

    Description
        Acquires RX status from CAN Module

****************************************************************************/

void CAN_RX_Status(uint8_t** Variable_2_Set)
{
	// Define constants
	#define RX_STATUS_TX_LENGTH 1
	#define RX_STATUS_RX_LENGTH 1
	
	// Value to Set
	uint8_t Data_2_Write[RX_STATUS_TX_LENGTH] = {MCP_RX_STATUS};
	
	// Call SPI command
	Write_SPI(RX_STATUS_TX_LENGTH, RX_STATUS_RX_LENGTH, Data_2_Write, Variable_2_Set);	
}

/****************************************************************************
    Public Function
        CAN_Bit_Modify

    Parameters
		

    Description
        Performs bit modify operation on CAN module

****************************************************************************/

void CAN_Bit_Modify(void)
{
	// Currently unused
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        

    Parameters
        None

    Description
        

****************************************************************************/


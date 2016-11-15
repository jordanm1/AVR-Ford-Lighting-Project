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

// Include CAN Header
#include "CAN.h"

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

static uint8_t TX_Data[1] = {0};
static uint8_t* RX_Data[1] = {0};
static uint8_t Recv_Byte = 0;

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
void MS_CAN_Initialize_1(void)
{
    // Reset the CAN Module and enter in configuration mode
    CAN_Reset();
    
    // Enter configuration mode, abort all pending transmissions and disable one shot mode
    TX_Data[0] = (MODE_CONFIG|ABORT_TX);
    CAN_Write(MCP_CANCTRL, TX_Data);
	
    // Disable CLKOUT
    TX_Data[0] = CLKOUT_DISABLE;
    CAN_Bit_Modify(MCP_CANCTRL, (1 << 2), TX_Data);
	
    // Set CNF Bit Time registers for Baud Rate = 312500 b/s
	TX_Data[0] = 0x41;
	CAN_Bit_Modify(MCP_CNF1, (1 << 0), TX_Data);
	TX_Data[0] = 0xF1;
	CAN_Bit_Modify(MCP_CNF2, ((1 << 0)|(1 << 1)|(1 << 2)), TX_Data);
	TX_Data[0] = 0xF1;
	CAN_Bit_Modify(MCP_CNF2, ((1 << 3)|(1 << 4)|(1 << 5)), TX_Data);
	TX_Data[0] = 0xF1;
	CAN_Bit_Modify(MCP_CNF2, ((1 << 7)|(1 << 6)), TX_Data);
	TX_Data[0] = 0x85;
    CAN_Write(MCP_CNF3, TX_Data);
	TX_Data[0] = 0x41;
	CAN_Bit_Modify(MCP_CNF1, ((1 << 7)|(1 << 6)), TX_Data);
}

/****************************************************************************
    Public Function
        MS_CAN_Initialize_2

    Parameters
		

    Description
        Initializes the remainder of the CAN Module

****************************************************************************/

void MS_CAN_Initialize_2(void)
{
    // Set interrupt registers
    
    // Enable all interrupts
    TX_Data[0] = 0xFF;
    CAN_Write(MCP_CANINTE, TX_Data);

    // Set up TX Buffer 0
    TX_Data[0] = MCP_TXB_TXP10_M; // Highest message priority
    CAN_Write(MCP_TXB0CTRL, TX_Data);
    
    // Set RTS pins as digital inputs
    TX_Data[0] = 0;
    CAN_Write(MCP_RTSCTRL, TX_Data);
    
    // Set identifier of TX Buffer 0 to 1
    TX_Data[0] = 0;
    CAN_Write(MCP_TXB0SIDH, TX_Data);
    TX_Data[0] = 0x20;
    CAN_Write(MCP_TXB0SIDL, TX_Data);
    
    // Set identifier of RX Buffer 0 to 0
    TX_Data[0] = 0;
    CAN_Write(MCP_RXF0SIDH, TX_Data);
    CAN_Write(MCP_RXF0SIDL, TX_Data);
    
    TX_Data[0] = 0x60;
    CAN_Write(MCP_RXB0CTRL, TX_Data);
    
    // Switch to Normal Mode
    TX_Data[0] = (MCP_NORMAL);
    CAN_Bit_Modify(MCP_CANCTRL, ((1 << 5)|(1 << 6)|(1 << 7)), TX_Data);
    
    RX_Data[0] = &Recv_Byte;
    CAN_Read(MCP_CANSTAT, RX_Data);
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
	}
	else if (choice == 2)
	{
		Data_2_Write[0] = MCP_LOAD_TX1;
	}
	else
	{
		Data_2_Write[0] = MCP_LOAD_TX2;
	}
	Data_2_Write[1] = Value_2_Set[0];

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

void CAN_Bit_Modify(uint8_t Register_2_Set, uint8_t Bits_2_Change, uint8_t* Value_2_Set)
{
    // Define constants
    #define BM_TX_LENGTH 4
    #define BM_RX_LENGTH 0
	
	/*
    // Initialize proposed mask
    uint8_t Mask_Byte = 0;

    // Set bits to be changed to 1 in mask
    for (int i = 0; i < sizeof(Bits_2_Change)/sizeof(Bits_2_Change[0]); i++)
    {
        Mask_Byte |= (1 << Bits_2_Change[i]);
    }
	*/
    
    // Value to Set
    uint8_t Data_2_Write[BM_TX_LENGTH] = {MCP_BITMOD, Register_2_Set, Bits_2_Change, Value_2_Set[0]};
    
    // Call SPI command
    Write_SPI(BM_TX_LENGTH, BM_RX_LENGTH, Data_2_Write, NULL);
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


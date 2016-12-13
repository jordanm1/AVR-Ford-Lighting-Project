/*******************************************************************************
    File:
        eeprom_storage.c
  
    Notes:
        This file contains the functions to write and read to EEPROM.

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
#include "eeprom_storage.h"

// Include other files below:

// Atomic Read/Write operations
#include <util/atomic.h>

// Interrupts
#include <avr/interrupt.h>

// #############################################################################
// ------------ MODULE DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ MODULE VARIABLES
// #############################################################################

static bool IsBusy = false;
static uint8_t Num_Bytes_Executed = 0;
static uint8_t Num_Bytes_Requested = 0;
static uint8_t * p_Target_EEPROM_Address;
static uint8_t * p_Caller_Values;

// #############################################################################
// ------------ PRIVATE FUNCTION PROTOTYPES
// #############################################################################

void start_eeprom_write_byte(uint8_t * p_eeprom, uint8_t * p_value);

// #############################################################################
// ------------ PUBLIC FUNCTIONS
// #############################################################################

/****************************************************************************
    Public Function
        Write_Data_To_EEPROM()

    Parameters
        None

    Description
        Writes data to EEPROM

****************************************************************************/
void Write_Data_To_EEPROM(uint8_t * p_address_in_eeprom, uint8_t * p_values_to_write, uint8_t num_bytes)
{
    // If we're busy, we need to wait until the current task is finished
    if (IsBusy)
    {
        // @TODO:
    }
    else
    {
        // Do housekeeping for the current write
        IsBusy = true;
        p_Target_EEPROM_Address = p_address_in_eeprom;
        p_Caller_Values = p_values_to_write;
        Num_Bytes_Executed = 0;
        Num_Bytes_Requested = num_bytes;

        // Start the EEPROM write
        start_eeprom_write_byte(p_Target_EEPROM_Address+Num_Bytes_Executed, p_Caller_Values+Num_Bytes_Executed);

        // Increment num bytes executed
        Num_Bytes_Executed++;
    }
}

/****************************************************************************
    Public Function
        Read_Data_From_EEPROM()

    Parameters
        None

    Description
        Reads data from EEPROM

****************************************************************************/
void Read_Data_From_EEPROM(uint8_t * p_address_in_eeprom, uint8_t * p_values_to_read, uint8_t num_bytes)
{
    // If we're busy, we need to wait until the current task is finished
    if (IsBusy)
    {
        // @TODO:
    }
    else
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            // Iterate for all bytes and copy the EEPROM data to the pointer
            //  of the caller
            IsBusy = true;
            for (int byte_index = 0; num_bytes > byte_index; byte_index++)
            {
                EEAR = (uint16_t) (p_address_in_eeprom+byte_index);
                EECR |= (1<<EERE);
                *(p_values_to_read+byte_index) = EEDR;
            }
            IsBusy = false;
        }
    }
}

// #############################################################################
// ------------ PRIVATE FUNCTIONS
// #############################################################################

/****************************************************************************
    Private Function
        start_eeprom_write_byte()

    Parameters
        None

    Description
        Initiates a write to EEPROM

****************************************************************************/
void start_eeprom_write_byte(uint8_t * p_eeprom, uint8_t * p_value)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Set up address and data registers
        EEAR = (uint16_t) p_eeprom;

        // Write one byte
        EEDR = *p_value;

        // Set Programming mode to erase and write
        //  Erase and write takes 3.4 ms to execute
        EECR = (0<<EEPM1)|(0<<EEPM0);

        // Write logical one to EEMPE to enable master programming, this is
        //  cleared in hardware after 4 cpu cycles
        EECR |= (1<<EEMPE);

        // Kick off EEPROM write by setting EEPE bit and enable the ready interrupt
        //  by setting EERIE bit
        EECR |= (1<<EERIE)|(1<<EEPE);

        // Now we wait for the ready interrupt...
    }
}

// #############################################################################
// ------------ INTERRUPT SERVICE ROUTINE
// #############################################################################

ISR(EE_RDY_vect)
{
    // Disable the ready interrupts
    EECR &= ~(1<<EERIE);

    // Check if we've written all the requested values
    if (Num_Bytes_Requested <= Num_Bytes_Executed)
    {
        // We are done writing all the bytes.
        IsBusy = false;
    }
    else
    {
        // Start the EEPROM write
        start_eeprom_write_byte(p_Target_EEPROM_Address+Num_Bytes_Executed, p_Caller_Values+Num_Bytes_Executed);

        // Increment num bytes executed
        Num_Bytes_Executed++;
    }
}
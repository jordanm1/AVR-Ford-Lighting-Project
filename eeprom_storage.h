#ifndef eeprom_storage_H
#define eeprom_storage_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Write_Data_To_EEPROM(uint8_t * p_address_in_eeprom, uint8_t * p_values_to_write, uint8_t num_bytes);
void Read_Data_From_EEPROM(uint8_t * p_address_in_eeprom, uint8_t * p_values_to_read, uint8_t num_bytes);

#endif // eeprom_storage_H

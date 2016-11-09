#ifndef cmd_sts_helpers_H
#define cmd_sts_helpers_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

// These functions expect pointers to data arrays that are LIN_PACKET_LEN long
intensity_data_t Get_Intensity_Data(uint8_t * p_LIN_packet);
position_data_t Get_Position_Data(uint8_t * p_LIN_packet);

void Write_Intensity_Data(uint8_t * p_LIN_packet, intensity_data_t data_to_write);
void Write_Position_Data(uint8_t * p_LIN_packet, position_data_t data_to_write);

// For Master Node Only!
uint8_t * Get_Pointer_To_Slave_Data(uint8_t * p_master_array, uint8_t slave_num);

#endif // cmd_sts_helpers_H

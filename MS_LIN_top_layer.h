#ifndef MS_LIN_top_layer_H
#define MS_LIN_top_layer_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void MS_LIN_Initialize(uint8_t * p_this_node_id, uint8_t * p_command_data, \
    uint8_t * p_status_data);
void Master_LIN_Broadcast_ID(uint8_t slave_id);

#endif // MS_CAN_top_layer_H

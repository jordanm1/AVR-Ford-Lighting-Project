#ifndef master_service_H
#define master_service_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_Master_Service(void);
void Run_Master_Service(uint32_t event_mask);
void Fill_Variable_List(uint8_t** Variable_List);

#endif // master_service_H

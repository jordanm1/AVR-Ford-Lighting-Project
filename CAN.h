#ifndef CAN_H
#define CAN_H

// #############################################################################
// ------------ CAN DEFINITIONS
// #############################################################################



// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void CAN_Initialize_1(void);
void CAN_Initialize_2(void);

// CAN Module SPI Commands
void CAN_Reset(void);
void CAN_Read(uint8_t Register_2_Read, uint8_t** Variable_2_Set);
void CAN_Read_RX_Buffer(bool choice, uint8_t** Variable_2_Set);
void CAN_Write(uint8_t Register_2_Set, uint8_t* Value_2_Set);
void CAN_Load_TX_Buffer(uint8_t choice, uint8_t* Value_2_Set);
void CAN_RTS(uint8_t choice);
void CAN_Read_Status(uint8_t** Variable_2_Set);
void CAN_RX_Status(uint8_t** Variable_2_Set);
void CAN_Bit_Modify(uint8_t Register_2_Set, uint8_t Bits_2_Change, uint8_t* Value_2_Set);

// CAN User Commands
void CAN_Send_Message(uint8_t Msg_Length, uint8_t* Transmit_Data);
void CAN_Read_Message(uint8_t** Recv_Data);

#endif // CAN_H

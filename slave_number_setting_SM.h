#ifndef slave_number_setting_SM_H
#define slave_number_setting_SM_H

// #############################################################################
// ------------ STATES FOR THIS STATE MACHINE
// #############################################################################

typedef enum
{
    IDLE_STATE,
    ENTERING_SETTING_MODE_STATE,
    SETTING_MODE_STATE,
} slave_number_setting_state_t;

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_Slave_Number_Setting_SM(void);
void Run_Slave_Number_Setting_SM(uint32_t event);
bool In_Slave_Number_Setting_Mode(void);
uint8_t Get_Last_Set_Slave_Number(void);

#endif // slave_number_setting_SM_H

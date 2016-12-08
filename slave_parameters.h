#ifndef SLAVE_PARAMETERS_H
#define SLAVE_PARAMETERS_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

// IMPORTANT:
// This function returns a pointer to data that resides in the program space
const slave_parameters_t * Get_Pointer_To_Slave_Parameters(uint8_t slave_num);

#endif // SLAVE_PARAMETERS_H

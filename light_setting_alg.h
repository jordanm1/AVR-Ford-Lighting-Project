#ifndef LIGHT_SETTING_ALG_H
#define LIGHT_SETTING_ALG_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

// IMPORTANT:
// This function expects a pointer to slave parameters that reside in program
// memory, NOT RAM.
void Compute_Individual_Light_Settings( \
                                        const slave_parameters_t * p_target_slave_params, \
                                        uint8_t * p_cmd_data,
                                        rect_vect_t v_desired_location);

#endif // LIGHT_SETTING_ALG_H

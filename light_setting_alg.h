#ifndef LIGHT_SETTING_ALG_H
#define LIGHT_SETTING_ALG_H

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

slave_settings_t Compute_Individual_Light_Settings( \
                                                    slave_parameters_t * p_slave_params, \
                                                    rect_vect_t v_desired_location);

#endif // LIGHT_SETTING_ALG_H

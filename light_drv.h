#ifndef LIGHT_DRV_H
#define LIGHT_DRV_H

// #############################################################################
// ------------ LIGHT DEFINITIONS
// #############################################################################

// Extremes for Intensities
#define LIGHT_OFF               (0)
#define MIN_LIGHT_INTENSITY     (30)
#define MAX_LIGHT_INTENSITY     (100)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Set_Light_Intensity(uint8_t requested_intensity);

#endif // LIGHT_DRV_H

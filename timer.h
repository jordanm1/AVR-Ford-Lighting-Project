#ifndef timer_H
#define timer_H

// #############################################################################
// ------------ TYPE DEFINITIONS
// #############################################################################

typedef void (*timer_cb_t) (uint32_t arg);

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Init_Timer_Module(void);
void Register_Timer(uint32_t * p_new_timer, timer_cb_t new_timer_cb_func);
void Start_Timer(uint32_t * p_this_timer, uint32_t time_in_ms);
uint32_t Get_Time_Timer(uint32_t * p_this_timer);
void Stop_Timer(uint32_t * p_this_timer);
void Start_Short_Timer(uint32_t * p_this_timer, uint32_t time_in_ms_div_ticksperms);

#endif // timer_H

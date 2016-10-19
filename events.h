#ifndef events_H
#define events_H

// #############################################################################
// ------------ EVENT DEFINITIONS
// #############################################################################

// Maximum number of events possible in the events service
#define MAXIMUM_NUM_EVENTS          32  

// Number of events we've defined
#define NUM_EVENTS                  6

#define EVT_SLAVE_NEW_CMD           (0x01<<0)
#define EVT_SLAVE_OTHER             (0x01<<1)

#define EVT_MASTER_SCH_TIMEOUT      (0x01<<2)
#define EVT_MASTER_NEW_STS          (0x01<<3)
#define EVT_MASTER_OTHER            (0x01<<4)

#define EVT_TEST_TIMEOUT            (0x01<<5)

// #############################################################################
// ------------ PUBLIC FUNCTION PROTOTYPES
// #############################################################################

void Post_Event(uint32_t event_mask);
void Run_Events(void);

#endif // events_H

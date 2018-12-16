#ifndef _GUIC_TIMER_RTT_H
#define _GUIC_TIMER_RTT_H

#include "timer/type.h"

struct rtt_status {
    microtime_t srtt;
    microtime_t rttvar;
    microtime_t rto;
    microtime_t rtt_min;
    microtime_t last_rtt;
};

#define RTT_SCALE 8
#define RTT_SHIFT 3

#define RTTVAR_SCALE 4
#define RTTVAR_SHIFT 2

#define SRTT_BASE 0
#define RTT_DEFAULT 3
#define SRTT_DEFAULT 6
#define MIN_TV 2
#define MAX_TV 128

/**
 * init rtt status
 * @param rtt: rtt status
 * 
 */
int rtt_status_init(struct rtt_status * const rtt);

/**
 * update rtt status
 * @param rtt: rtt status
 * @param nticks: rtt
 * 
 */
int rtt_status_update(struct rtt_status * const rtt, int64_t nticks);

#endif

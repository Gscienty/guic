#ifndef _GUIC_TIMER_RTT_H
#define _GUIC_TIMER_RTT_H

#include "timer/type.h"

struct rtt_status {
    int64_t srtt;
    int64_t rttvar;
    int64_t rto;
    int64_t rtt_min;
};

#define RTT_SCALE 8
#define RTT_SHIFT 3

#define RTTVAR_SCALE 4
#define RTTVAR_SHIFT 2

#define SRTT_BASE 0
#define SRTT_DEFAULT 3
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

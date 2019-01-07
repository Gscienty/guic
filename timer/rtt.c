#include "timer/rtt.h"

/**
 * init rtt status
 * @param rtt: rtt status
 * 
 */
int rtt_status_init(struct rtt_status * const rtt)
{
    rtt->srtt = SRTT_BASE;
    rtt->rttvar = RTT_DEFAULT << 3;
    rtt->rtt_min = MIN_TV;
    rtt->rto = ((SRTT_BASE >> 2) + (SRTT_DEFAULT << 2)) >> 1;
    if (rtt->rto < MIN_TV)
        rtt->rto = MIN_TV;
    if (rtt->rto > MAX_TV)
        rtt->rto = MAX_TV;

    return 0;
}

/**
 * update rtt status
 * @param rtt: rtt status
 * @param nticks: rtt
 * 
 */
int rtt_status_update(struct rtt_status * const rtt, int64_t nticks)
{
    int64_t delta;

    if (rtt->srtt) {
        delta = nticks - 1 - (rtt->srtt >> RTT_SHIFT);
        if ((rtt->srtt += delta) <= 0)
            rtt->srtt = 1;
        if (delta < 0)
            delta = -delta;
        delta -= (rtt->rttvar >> RTTVAR_SHIFT);
        if ((rtt->rttvar += delta) <= 0)
            rtt->rttvar = 1;
    }
    else {
        rtt->srtt = nticks << RTT_SHIFT;
        rtt->rttvar = nticks << (RTTVAR_SHIFT - 1);
    }

    rtt->rto = (rtt->srtt >> RTT_SHIFT) + rtt->rttvar;
    if (rtt->rto < rtt->rtt_min)
        rtt->rto = rtt->rtt_min;
    if (rtt->rto > MAX_TV)
        rtt->rto = MAX_TV;

    return 0;
}


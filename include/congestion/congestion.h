#ifndef _GUIC_CONGESTION_CONGESTION_H
#define _GUIC_CONGESTION_CONGESTION_H

#include "timer/rtt.h"
#include "congestion/cubic.h"
#include "congestion/hybrid_slow_start.h"
#include "congestion/prr.h"
#include "wire/type.h"

struct congestion {
    struct slow_start slow_start;
    struct prr prr;
    struct cubic cubic;
    struct rtt_status *rtt;

    packet_number_t largest_sent;
    packet_number_t largest_acked;
    packet_number_t cutback_largest_sent;
    int slow_start_exited;
    int largest_reduction;

    bytecount_t cwnd;
    bytecount_t min_cwnd;
};

#endif

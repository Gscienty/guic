#ifndef _GUIC_CONGESTION_HYBRID_SLOW_START_H
#define _GUIC_CONGESTION_HYBRID_SLOW_START_H

#include "timer/type.h"
#include "wire/type.h"
#include <stdint.h>

#define SLOW_START_MIN_SIMPLE 8
#define SLOW_START_DELAY_FACTOR 8
#define SLOW_START_MIN_THRESHOLD 4000
#define SLOW_START_MAX_THRESHOLD 16000
#define SLOW_START_LOW_WINDOW 16

struct slow_start {
    int started;
    int hystart_found;
    microtime_t min_rtt;
    uint32_t simple_count;
    packet_number_t end_pnum;
    packet_number_t last_send_pnum;
};

/**
 * init slow start
 * @param slow_start: slow start status
 * 
 */
void slow_start_init(struct slow_start * const slow_start);

/**
 * start of each receive round
 * @param slow_start: slow start
 * @param pnum: last send packet number
 * 
 */
void slowstart_start_receive_round(struct slow_start * const slow_start,
                                   const packet_number_t pnum);

/**
 * restart
 * @param slow_start: slow start
 * 
 */
void slowstart_restart(struct slow_start * const slow_start);

/**
 * received ack
 * @param slow_start: slow start
 * @param last_rtt: last rtt
 * @param min_rtt: lowest delay
 * @param cwnd: congestion window
 * @return should exit slow start
 * 
 */
int slowstart_received_ack(struct slow_start * const slow_start,
                           const microtime_t last_rtt,
                           const microtime_t min_rtt,
                           const bytecount_t cwnd);


#endif

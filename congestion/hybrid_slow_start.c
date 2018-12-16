#include "congestion/hybrid_slow_start.h"

/**
 * init slow start
 * @param slow_start: slow start status
 * 
 */
void slow_start_init(struct slow_start * const slow_start)
{
    slow_start->end_pnum       = 0;
    slow_start->hystart_found  = 0;
    slow_start->last_send_pnum = 0;
    slow_start->min_rtt        = 0;
    slow_start->simple_count   = 0;
    slow_start->started        = 0;
}

/**
 * start of each receive round
 * @param slow_start: slow start
 * @param pnum: last send packet number
 * 
 */
void slowstart_start_receive_round(struct slow_start * const slow_start,
                                   const packet_number_t pnum)
{
    slow_start->min_rtt      = 0;
    slow_start->simple_count = 0;
    slow_start->started      = 1;
    slow_start->end_pnum     = pnum;
}

/**
 * restart
 * @param slow_start: slow start
 * 
 */
void slowstart_restart(struct slow_start * const slow_start)
{
    slow_start->started = 0;
    slow_start->hystart_found = 0;
}

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
                           const bytecount_t cwnd)
{
    microtime_t thresold;

    if (!slow_start->started) {
        slowstart_start_receive_round(slow_start, slow_start->last_send_pnum);
    }

    if (slow_start->hystart_found) {
        return 1;
    }

    slow_start->simple_count++;
    if (slow_start->simple_count <= SLOW_START_MIN_SIMPLE
        && (slow_start->min_rtt == 0 || slow_start->min_rtt > last_rtt)) {
        slow_start->min_rtt = last_rtt;
    }

    if (slow_start->simple_count == SLOW_START_MIN_SIMPLE) {
        thresold = min_rtt / SLOW_START_DELAY_FACTOR;
        thresold = thresold > SLOW_START_MIN_THRESHOLD
            ? thresold : SLOW_START_MIN_THRESHOLD;
        thresold = thresold > SLOW_START_MAX_THRESHOLD
            ? SLOW_START_MAX_THRESHOLD : thresold;

        if (slow_start->min_rtt > min_rtt + thresold) {
            slow_start->hystart_found = 1;
        }
    }

    return cwnd >= 16 && slow_start->hystart_found;
}

#include "congestion/cubic.h"
#include <math.h>
#include <stddef.h>

#include <stdio.h>

/**
 * init cubic
 * @param cubic: cubic status
 *
 */
void cubic_init(struct cubic * const cubic)
{
    cubic->cnt          = 0;
    cubic->ack_cnt      = 0;
    cubic->delay_min    = 0;
    cubic->epoch_start  = 0;
    cubic->origin_point = 0;
    cubic->w_last       = 0;
    cubic->w_last_max   = 0;
}

inline static double __alpha()
{
    return 3 * CUBIC_FACTOR_BETA / (2 - CUBIC_FACTOR_BETA);
}

/**
 * received ack
 * @param cubic: cubic status
 * @param mss: mss
 * @param acked: acked bytes count
 * @param cwnd: congestion window
 * @param cur: event time
 * @param delay: delay time
 *
 */
bytecount_t cubic_received_ack(struct cubic * const cubic,
                               bytecount_t mss,
                               bytecount_t acked,
                               bytecount_t cwnd,
                               microtime_t cur,
                               microtime_t delay)
{
    double t;
    bytecount_t target;

    cubic->ack_cnt += acked;

    if (cubic->epoch_start == 0) {
        cubic->epoch_start = cur;
        cubic->ack_cnt = acked;
        cubic->cwnd = cwnd;

        if (cwnd < cubic->w_last_max) {
            cubic->time_peroid = 
                pow((cubic->w_last_max - cwnd) / CUBIC_FACTOR_C, 1.0 / 3);
            cubic->origin_point = cubic->w_last_max;
        }
        else {
            cubic->time_peroid = 0;
            cubic->origin_point = cwnd;
        }
    }

    t = (cur + delay - cubic->epoch_start) / DOUBLE_MICROSECONDS;
    target = cubic->origin_point
        + pow(t - cubic->time_peroid, 3.0) * mss * CUBIC_FACTOR_C;

    cubic->cwnd += __alpha() * cubic->ack_cnt * mss / cubic->cwnd;
    cubic->ack_cnt = 0;

    if (target < cubic->cwnd)
        target = cubic->cwnd;

    return target;
}

/**
 * packet loss
 * @param cubic: cubic status
 * @param cwnd: congestion window
 * @return: ssthresh
 *
 */
bytecount_t cubic_packet_loss(struct cubic * const cubic,
                              bytecount_t cwnd)
{
    cubic->epoch_start = 0;

    if (cwnd < cubic->w_last_max) {
        cubic->w_last_max = cwnd * (2 - CUBIC_FACTOR_BETA) / 2;
    }

    return cwnd * (1 - CUBIC_FACTOR_BETA);
}

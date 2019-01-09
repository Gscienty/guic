#include "congestion/cubic.h"
#include <math.h>
#include <stddef.h>

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
 * @param acked: acked bytes count
 * @param cwnd: congestion window
 * @param cur: event time
 * @param delay: delay time
 *
 */
bytecount_t cubic_received_ack(struct cubic * const cubic,
                               bytecount_t acked,
                               bytecount_t cwnd,
                               microtime_t cur,
                               microtime_t delay)
{
    double t;
    bytecount_t target;
    bytecount_t max_cnt;

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
        + CUBIC_FACTOR_C * pow(t - cubic->time_peroid, 3.0);

    if (target > cwnd)
        cubic->cnt = cwnd / (target - cwnd);
    else
        cubic->cnt = 100 * cwnd;

    cubic->cwnd += __alpha() * (cubic->ack_cnt / cwnd);
    cubic->ack_cnt = 0;

    if (cubic->cwnd > cwnd) {
        max_cnt = cwnd / (cubic->cwnd - cwnd);
        if (cubic->cnt > max_cnt)
            cubic->cnt = max_cnt;
    }

    return cubic->cnt;
}

/**
 * packet loss
 * @param cubic: cubic status
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

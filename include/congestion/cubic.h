#ifndef _GUIC_CONGESTION_CUBIC_H
#define _GUIC_CONGESTION_CUBIC_H

#include "timer/type.h"
#include "timer/rtt.h"
#include "wire/type.h"

#define CUBIC_FACTOR_BETA 0.7
#define CUBIC_FACTOR_C 4.0

struct cubic {
    microtime_t delay_min;
    microtime_t epoch_start;
    microtime_t time_peroid;

    bytecount_t cwnd;
    bytecount_t ack_cnt;
    bytecount_t cnt;
    bytecount_t w_last_max;
    bytecount_t w_last;
    bytecount_t origin_point;
};

/**
 * init cubic
 * @param cubic: cubic status
 *
 */
void cubic_init(struct cubic * const cubic);

/**
 * received ack
 * @param mss: mss
 * @param cubic: cubic status
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
                               microtime_t delay);

/**
 * packet loss
 * @param cubic: cubic status
 * @param cwnd: congestion window
 * @return: ssthresh
 *
 */
bytecount_t cubic_packet_loss(struct cubic * const cubic,
                              bytecount_t cwnd);

#endif

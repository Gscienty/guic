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

    packet_number_t large_sent;
    packet_number_t large_acked;
    packet_number_t cb_large_sent;
    int slow_start_exited;
    int reduction;

    bytecount_t cwnd;
    bytecount_t min_cwnd;
    bytecount_t max_cwnd;
    bytecount_t slow_start_threshold;

    bytecount_t init_cwnd;
    bytecount_t init_max_cwnd;

    bytecount_t slow_start_min_ecwnd;

    packet_number_t lost_packet;
    bytecount_t lost_bytes;
};

/**
 * init congestion
 * @param cong: congestion status
 * @param rtt: rtt status
 * @param cwnd: init congestion window
 * @param max_cwnd: init max congestion window
 * @param min_cwnd: init min congestion window
 * 
 */
void congestion_init(struct congestion * const cong,
                     struct rtt_status * const rtt,
                     const bytecount_t cwnd,
                     const bytecount_t max_cwnd,
                     const bytecount_t min_cwnd);

/**
 * calculate next sent waiting time
 * @param cong: congestion status
 * @param mss: mss
 * @param flight: flight
 * @return: when the next packet should be sent
 *
 */
bytecount_t congestion_next_sent_delay(const struct congestion * const cong,
                                       const bytecount_t flight,
                                       const bytecount_t mss);

/**
 * on packet sent
 * @param cong: congestion status
 * @param pnum: packet number
 * @param bytes: bytes
 * @param is_retrans: retransmittable flag
 *
 */
void congestion_on_sent(struct congestion * const cong,
                        const packet_number_t pnum,
                        const bytecount_t bytes,
                        const int is_retrans);

/**
 * try exit slow start
 * @param cong: congestion status
 * @param mss: mss
 * 
 */
void congestion_try_slow_start(struct congestion * const cong,
                               const bytecount_t mss);

/**
 * received packet acked
 * @param cong: congestion window
 * @param max_burst: max burst
 * @param mss: mss
 * @param pnum: acked packet number
 * @param bytes: acked bytes
 * @param flight: flight
 * @param event_time: event time
 * 
 */
void congestion_received_acked(struct congestion * const cong,
                               const bytecount_t max_burst,
                               const bytecount_t mss,
                               const packet_number_t pnum,
                               const bytecount_t bytes,
                               const bytecount_t flight,
                               const microtime_t event_time);

/**
 * an retransmission timeout
 * @param cong: congestion window
 * @param retransed: packet retransmission flag
 *
 */
void congestion_retransmission_timeout(struct congestion * const cong,
                                       const int retransed);

/**
 * loss packet
 * @param cong: congestion window
 * @param mss: mss
 * @param pnum: packet number
 * @param bytes: lost bytes
 * @param flight: flight
 * 
 */
void congestion_loss(struct congestion * const cong,
                     const bytecount_t mss,
                     const packet_number_t pnum,
                     const bytecount_t bytes,
                     const bytecount_t flight);

/**
 * reset congestion
 * @param cong: congestion status
 * 
 */
void congestion_reset(struct congestion * const cong);

#endif

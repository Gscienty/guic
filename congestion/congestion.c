#include "congestion/congestion.h"
#include "congestion/cubic.h"
#include "congestion/prr.h"
#include "congestion/hybrid_slow_start.h"

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
                     const bytecount_t min_cwnd)
{
    cong->rtt                  = rtt;
    cong->min_cwnd             = min_cwnd;
    cong->max_cwnd             = max_cwnd;
    cong->init_cwnd            = cwnd;
    cong->init_max_cwnd        = max_cwnd;
    cong->cwnd                 = cwnd;
    cong->slow_start_threshold = max_cwnd;

    cong->large_sent        = 0;
    cong->large_acked       = 0;
    cong->cb_large_sent     = 0;
    cong->slow_start_exited = 0;
    cong->reduction         = 0;

    cong->slow_start_min_ecwnd = 0;

    cubic_init(&cong->cubic);
    slow_start_init(&cong->slow_start);
    prr_init(&cong->prr);

    cong->lost_bytes  = 0;
    cong->lost_packet = 0;
}

inline static int __recovering(const struct congestion * const cong)
{
    return cong->large_acked <= cong->cb_large_sent && cong->large_acked != 0;
}

inline static int __in_slow_start(const struct congestion * const cong)
{
    return cong->cwnd < cong->slow_start_threshold;
}

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
                                       const bytecount_t mss)
{
    bytecount_t delay;

    if (__recovering(cong)) {
        if (prr_check_send(&cong->prr, mss,
                           cong->cwnd, flight, cong->slow_start_threshold)) {
            return 0;
        }
    }

    delay = cong->rtt->srtt / (2 * cong->cwnd);

    if (!__in_slow_start(cong)) {
        delay = delay * 8 / 5;
    }

    return delay;
}

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
                        const int is_retrans)
{
    if (!is_retrans) {
        return;
    }

    if (__recovering(cong)) {
        cong->prr.sent += bytes;
    }

    cong->large_sent                = pnum;
    cong->slow_start.last_send_pnum = pnum;
}

/**
 * try exit slow start
 * @param cong: congestion status
 * @param mss: mss
 * 
 */
void congestion_try_slow_start(struct congestion * const cong,
                               const bytecount_t mss)
{
    if (__in_slow_start(cong)
        && slowstart_received_ack(&cong->slow_start,
                                  cong->rtt->last_rtt, cong->rtt->rtt_min,
                                  cong->cwnd / mss)) {
        cong->slow_start_threshold = cong->cwnd;
    }
}

static int __cwnd_limited(struct congestion * const cong,
                          const bytecount_t max_burst,
                          const bytecount_t flight)
{
    return (flight >= cong->cwnd)
        || (__in_slow_start(cong) && flight > cong->cwnd / 2)
        || (cong->cwnd - flight) <= max_burst;
}

static void __increase_cwnd(struct congestion * const cong,
                            const bytecount_t max_burst,
                            const bytecount_t mss,
                            const bytecount_t bytes,
                            const bytecount_t flight,
                            const microtime_t event_time)
{
    bytecount_t cwnd;

    if (!__cwnd_limited(cong, max_burst, flight)) {
        cong->cubic.epoch_start = 0;
        return;
    }

    if (cong->cwnd >= cong->max_cwnd) {
        return;
    }

    if (__in_slow_start(cong)) {
        cong->cwnd += mss;
        return;
    }

    cwnd = cubic_received_ack(&cong->cubic, bytes, cong->cwnd, mss,
                              event_time, cong->rtt->rtt_min);
    cong->cwnd = cwnd > cong->max_cwnd ? cong->max_cwnd : cwnd;
}

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
                               const microtime_t event_time)
{
    cong->large_acked = cong->large_acked > pnum ? cong->large_acked : pnum;
    if (__recovering(cong)) {
        prr_received_ack(&cong->prr, bytes);
        return;
    }

    __increase_cwnd(cong, max_burst, mss, bytes, flight, event_time);
    if (__in_slow_start(cong)) {
        if (cong->slow_start.end_pnum < pnum) {
            cong->slow_start.started = 0;
        }
    }
}

/**
 * loss packet
 * @param cong: congestion window
 * @param pnum: packet number
 * @param bytes: lost bytes
 * @param flight: flight
 * 
 */
void congestion_loss(struct congestion * const cong,
                     const bytecount_t mss,
                     const packet_number_t pnum,
                     const bytecount_t bytes,
                     const bytecount_t flight)
{
    bytecount_t reduce_cwnd;

    if (pnum <= cong->cb_large_sent) {
        if (cong->slow_start_exited) {
            cong->lost_packet++;
            cong->lost_bytes += bytes;
            if (cong->reduction) {
                reduce_cwnd = cong->cwnd - bytes;
                cong->cwnd = reduce_cwnd > cong->slow_start_min_ecwnd
                    ? cong->slow_start_min_ecwnd
                    : reduce_cwnd;
                cong->slow_start_threshold = cong->cwnd;
            }
        }
        return;
    }

    cong->slow_start_exited = __in_slow_start(cong);
    if (cong->slow_start_exited) {
        cong->lost_packet++;
    }

    prr_packet_loss(&cong->prr, flight);

    if (cong->reduction && __in_slow_start(cong)) {
        if (cong->cwnd >= 2 * cong->init_cwnd) {
            cong->slow_start_min_ecwnd = cong->cwnd / 2;
        }
        cong->cwnd -= mss;
    }
    else {
        cong->cwnd = cubic_packet_loss(&cong->cubic, cong->cwnd);
    }
    if (cong->cwnd < cong->min_cwnd) {
        cong->cwnd = cong->min_cwnd;
    }

    cong->slow_start_threshold = cong->cwnd;
    cong->cb_large_sent = cong->large_sent;
}

/**
 * reset congestion
 * @param cong: congestion status
 * 
 */
void congestion_reset(struct congestion * const cong)
{
    slowstart_restart(&cong->slow_start);
    prr_init(&cong->prr);
    cubic_init(&cong->cubic);

    cong->large_acked          = 0;
    cong->large_sent           = 0;
    cong->cb_large_sent        = 0;
    cong->slow_start_exited    = 0;
    cong->cwnd                 = cong->init_cwnd;
    cong->slow_start_threshold = cong->init_max_cwnd;
    cong->max_cwnd             = cong->init_max_cwnd;
}

/**
 * an retransmission timeout
 * @param cong: congestion window
 * @param retransed: packet retransmission flag
 *
 */
void congestion_retransmission_timeout(struct congestion * const cong,
                                       const int retransed)
{
    cong->cb_large_sent = 0;
    if (!retransed) {
        return;
    }
    slowstart_restart(&cong->slow_start);
    cubic_init(&cong->cubic);
    cong->slow_start_threshold = cong->cwnd / 2;
    cong->cwnd = cong->min_cwnd;
}

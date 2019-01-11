#ifndef _GUIC_CONGESTION_PRR_H
#define _GUIC_CONGESTION_PRR_H

#include "wire/type.h"

struct prr {
    bytecount_t sent;
    bytecount_t delivered;
    bytecount_t acked;
    bytecount_t flight;
};

/**
 * trigger packet loss
 * @param prr: prr
 * @param flight: flight bytes
 * 
 */
void prr_packet_loss(struct prr * const prr, const bytecount_t flight);

/**
 * received ack
 * @param prr: prr
 * @param acked: acked bytes
 *
 */
void prr_received_ack(struct prr * const prr, const bytecount_t acked);

/**
 * check packets can be sent
 * @param prr: prr
 * @param mss: mss
 * @param cwnd: congestion window
 * @param flight: flight
 * @param slowstart_threshold: slow start threshold
 * @return: packets can be sent
 * 
 */
int prr_check_send(const struct prr * const prr,
                   const bytecount_t mss,
                   const bytecount_t cwnd,
                   const bytecount_t flight,
                   const bytecount_t slowstart_threshold);

#endif

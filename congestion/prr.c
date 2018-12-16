#include "congestion/prr.h"

/**
 * init prr
 * @param prr: prr
 * 
 */
void prr_init(struct prr * const prr)
{
    prr->acked     = 0;
    prr->delivered = 0;
    prr->flight    = 0;
    prr->sent      = 0;
}

/**
 * trigger packet loss
 * @param prr
 * 
 */
void prr_packet_loss(struct prr * const prr, const bytecount_t flight)
{
    prr->sent      = 0;
    prr->flight    = flight;
    prr->delivered = 0;
    prr->acked     = 0;
}

/**
 * received ack
 * @param prr: prr
 * @param acked: acked bytes
 *
 */
void prr_received_ack(struct prr * const prr, const bytecount_t acked)
{
    prr->delivered += acked;
    prr->acked++;
}

/**
 * check packets can be sent
 * @param prr: prr
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
                   const bytecount_t slowstart_threshold)
{
    if (prr->sent == 0 || flight < mss) {
        return 1;
    }

    if (cwnd > flight) {
        return prr->delivered + prr->acked * mss > prr->sent;
    }

    return prr->delivered * slowstart_threshold > prr->sent * prr->flight;
}

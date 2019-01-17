#ifndef _GUIC_ACK_PACKET_H
#define _GUIC_ACK_PACKET_H

#include "timer/type.h"
#include "util/linked_list.h"
#include "wire/type.h"
#include "wire/ack.h"

struct packet {
    packet_number_t pnum;
    enum long_header_packet_type ptype;
    void *payload;
    bytecount_t len;
    enum encrypt_level encrypt_level;
    microtime_t send_time;

    packet_number_t large_acked;

    int allow_retrans;
    int included_flight;
    int is_retrans;
    packet_number_t retrains_pnum;
};

struct recv_packet_llnode {
    struct llnode node;
    
    packet_number_t min;
    packet_number_t max;
};

struct recv_packet_history {
    struct llnode head;
    packet_number_t lowest_pnum;
    int size;
};

/**
 * init packet history
 * @param history: packet history
 * 
 */
void recv_packet_history_init(struct recv_packet_history * const history);

/**
 * received packet
 * @param history: history
 * @param pnum: packet number
 * 
 */
int history_received_packet(struct recv_packet_history * const history,
                            packet_number_t pnum);

/**
 * delete below
 * @param history: history
 * @param pnum: packet number
 * 
 */
void recv_packet_delete_below(struct recv_packet_history * const history,
                              packet_number_t pnum);

/**
 * generate ack ranges
 * @param history: history
 * @return: ack ranges
 *
 */
struct ack_block *
recv_packet_generate_ack(const struct recv_packet_history * const history);

#endif

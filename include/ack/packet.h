#ifndef _GUIC_ACK_PACKET_H
#define _GUIC_ACK_PACKET_H

#include "timer/type.h"
#include "util/linked_list.h"
#include "util/rbtree.h"
#include "wire/type.h"
#include "wire/ack.h"
#include <stdbool.h>

struct packet_retrans_as_llnode {
    struct llnode node;
    packet_number_t pnum;
};

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
    struct llnode retrans_as;
    bool is_retrans;
    packet_number_t retrans_pnum;
};

struct packet_llnode {
    struct llnode node;
    const struct packet * packet;
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

struct sent_packet_node {
    struct llnode llnode;
    struct rbnode rbnode;

    const struct packet *packet;
};

struct sent_packet_history {
    struct llnode list;
    struct rbroot dict;

    int packets_count;
    int crypto_packets_count;

    struct llnode *first;
};

/**
 * init recv packet history
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
int recv_packet_received(struct recv_packet_history * const history,
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

/**
 * init sent packet history
 * @param history: history
 * 
 */
void sent_packet_history_init(struct sent_packet_history * const history);

/**
 * sent packet
 * @param history: history
 * @param packet: packet
 * 
 */
struct llnode *sent_packet_sent(struct sent_packet_history * const history,
                                const struct packet *packet);

/**
 * retransmission sent packet
 * @param history: history
 * @param head: packet list
 * @param retrans_pnum: retransmission packet number
 * 
 */
void sent_packet_retransmission(struct sent_packet_history * const history,
                                struct llnode * const head,
                                packet_number_t retrans_pnum);

/**
 * get packet
 * @param history: history
 * @param pnum: packet number
 * @return packet
 * 
 */
const struct packet *sent_packet_get(struct sent_packet_history * const history,
                                     packet_number_t pnum);

/**
 * mark retransmission
 * @param history: history
 * @param pnum: packet number
 * 
 */
void sent_packet_mark_cannot_retrans(struct sent_packet_history * const history,
                                     packet_number_t pnum);

/**
 * remove packet
 * @param history: history
 * @param pnum: packet number
 * 
 */
void sent_packet_remove(struct sent_packet_history * const history,
                        packet_number_t pnum);

#endif

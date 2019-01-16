#ifndef _GUIC_UTIL_PACKET_NUMBER_H
#define _GUIC_UTIL_PACKET_NUMBER_H

#include "util/rbtree.h"
#include "wire/type.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum packet_number_space {
    PACKET_NUMBER_INITIAL_SPACE,
    PACKET_NUMBER_HANDSHAKE_SPACE,
    PACKET_NUMBER_APPLICATION_SPACE
};

/**
 * get packet number size
 * @param pnum: packet number
 * @return: packet number size
 * 
 */
size_t packet_number_length(packet_number_t pnum);

packet_number_t packet_number_decode(packet_number_t largest_pn,
                                     packet_number_t truncated_pn,
                                     size_t pn_nbits);

struct pnum_history {
    struct rbnode node;

    packet_number_t pnum;
};

struct pnum_gener {
    packet_number_t period;
    packet_number_t next;
    packet_number_t skip;

    int history_size;
    struct rbroot *history;
};

/**
 * init packet number generator
 * @param gener: generator
 * @param init: init packet number
 * @param period: period
 *
 */
void pnum_gener_init(struct pnum_gener * const gener,
                     packet_number_t init,
                     packet_number_t period);

/**
 * get packet number peek
 * @param gener: generator
 * @return: packet number
 * 
 */
packet_number_t pnum_gener_peek(const struct pnum_gener * const gener);

/**
 * get packet number && generate next packet number
 * @param gener: generator
 * @return: packet number
 *
 */
packet_number_t pnum_gener_pop(struct pnum_gener * const gener);

/**
 * exist packet number
 * @param gener: generator
 * @param pnum: packet number
 * @return exist packet number
 *
 */
bool pnum_gener_existed(const struct pnum_gener * const gener,
                        const packet_number_t pnum);

 
#endif

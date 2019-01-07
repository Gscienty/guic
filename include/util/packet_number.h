#ifndef _GUIC_UTIL_PACKET_NUMBER_H
#define _GUIC_UTIL_PACKET_NUMBER_H

#include "wire/type.h"
#include <stddef.h>
#include <stdint.h>

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

#endif

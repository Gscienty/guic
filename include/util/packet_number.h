#ifndef _GUIC_UTIL_PACKET_NUMBER_H
#define _GUIC_UTIL_PACKET_NUMBER_H

#include "wire/type.h"
#include <stddef.h>
#include <stdint.h>

/**
 * get packet number size
 * @param pnum: packet number
 * @return: packet number size
 * 
 */
size_t packet_number_length(packet_number_t pnum);

#endif

#include "util/packet_number.h"

/**
 * get packet number size
 * @param pnum: packet number
 * @return: packet number size
 * 
 */
size_t packet_number_length(packet_number_t pnum)
{
    if (pnum <= 0xFF)
        return 1;
    else if (pnum <= 0xFFFF)
        return 2;
    else if (pnum <= 0xFFFFFF)
        return 3;
    else
        return 4;
}

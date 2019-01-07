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

packet_number_t packet_number_decode(packet_number_t largest_pn,
                                     packet_number_t truncated_pn,
                                     size_t pn_nbits)
{
    packet_number_t expected_pn = largest_pn + 1;
    packet_number_t pn_win = 1 << pn_nbits;
    packet_number_t pn_hwin = pn_win / 2;
    packet_number_t pn_mask = pn_win - 1;
    packet_number_t candidate_pn = (expected_pn & ~pn_mask) | truncated_pn;

    if (candidate_pn <= expected_pn - pn_win)
        return candidate_pn + pn_win;

    if (candidate_pn > expected_pn + pn_hwin
        && candidate_pn > pn_win)
        return candidate_pn - pn_win;

    return candidate_pn;
}

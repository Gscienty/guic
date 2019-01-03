#ifndef _GUIC_WIRE_MAX_DATA_H
#define _GUIC_WIRE_MAX_DATA_H

#include <stdint.h>
#include <stddef.h>

struct max_data {
    uint64_t max;
};

/**
 * get max data frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t max_data_size(const struct max_data * const frm);

/**
 * encode max data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: max data frame
 * @return: used size
 *
 */
size_t max_data_encode(void * const buf,
                       const size_t size,
                       const struct max_data * const frm);

/**
 * decode max data frame
 * @param frm: max data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t max_data_decode(struct max_data * const frm,
                       void * const buf,
                       const size_t size);

#endif

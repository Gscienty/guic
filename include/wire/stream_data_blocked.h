#ifndef _GUIC_WIRE_STREAM_DATA_BLOCKED_H
#define _GUIC_WIRE_STREAM_DATA_BLOCKED_H

#include "wire/type.h"
#include <stdint.h>
#include <stddef.h>

struct stream_data_blocked {
    stream_id_t stream_id;
    uint64_t limit;
};

/**
 * get max data frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t stream_data_blocked_size(const struct stream_data_blocked * const frm);

/**
 * encode max data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: max data frame
 * @return: used size
 *
 */
size_t stream_data_blocked_encode(void * const buf,
                                  const size_t size,
                                  const struct stream_data_blocked * const frm);

/**
 * decode max data frame
 * @param frm: max data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t stream_data_blocked_decode(struct stream_data_blocked * const frm,
                                  void * const buf,
                                  const size_t size);

#endif

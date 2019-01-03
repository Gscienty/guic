#ifndef _GUIC_WIRE_MAX_STREAMS_H
#define _GUIC_WIRE_MAX_STREAMS_H

#include "wire/type.h"
#include <stddef.h>

struct max_streams {
    stream_id_t max;
};

/**
 * get max streams frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t max_streams_size(const struct max_streams * const frm);

/**
 * encode max data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: max data frame
 * @return: used size
 *
 */
size_t max_streams_encode(void * const buf,
                          const size_t size,
                          const struct max_streams * const frm);

/**
 * decode max data frame
 * @param frm: max data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t max_streams_decode(struct max_streams * const frm,
                          void * const buf,
                          const size_t size);


#endif

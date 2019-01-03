#ifndef _GUIC_WIRE_STREAM_H
#define _GUIC_WIRE_STREAM_H

#include "wire/type.h"
#include <stddef.h>

struct stream {
    stream_id_t stream_id;
    uint64_t off;
    uint64_t len;
    uint8_t fin;

    void *data;
};

/**
 * get stream frame size
 * @param frm: stream frame
 * @return size
 * 
 */
size_t stream_size(const struct stream * const frm);

/**
 * encode stream frame
 * @param type: type
 * @param buf: buf
 * @param size: buf size
 * @param frm: stream frame
 * @return: used size
 * 
 */
size_t stream_encode(uint8_t * const type,
                     void * const buf,
                     const size_t size,
                     const struct stream * const frm);

/**
 * decode stream frame
 * @param frm: stream frame
 * @param buf: buf
 * @param size: buf size
 * @param type: type byte
 * @return: used size
 * 
 */
size_t stream_decode(struct stream * const frm,
                     void * const buf,
                     const size_t size,
                     const uint8_t type);

/**
 * get stream type byte
 * @param frm: stream frame
 * @return: type byte
 *
 */
uint8_t stream_type_byte(const struct stream * const frm);

#endif

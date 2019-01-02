#ifndef _GUIC_WIRE_RESET_H
#define _GUIC_WIRE_RESET_H

#include "wire/type.h"
#include <stddef.h>

struct reset {
    stream_id_t stream_id;
    app_err_t err;
    uint64_t offset;
};

/**
 * get reset size
 * @param frm: reset frame
 * @return: reset frame size
 *
 */
size_t reset_size(const struct reset * const frm);

/**
 * put reset frame to buffer
 * @param buf: buffer
 * @param size: buf size
 * @param frm: reset frame
 * @return: used size (fail return 0)
 * 
 */
size_t reset_encode(void * const buf, const size_t size, const struct reset * const frm);

/**
 * get reset frame
 * @param frm: frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t reset_decode(struct reset * const frm, void * const buf, const size_t size);

#endif

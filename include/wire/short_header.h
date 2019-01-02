#ifndef _GUIC_WIRE_SHORT_HEADER_H
#define _GUIC_WIRE_SHORT_HEADER_H

#include "util/rbtree.h"
#include "util/conn_id.h"
#include "util/buffer.h"
#include <stdint.h>

struct short_header {
    uint8_t spin;
    uint8_t key_phase;
    struct conn_id_t dst_connid;
    uint32_t pnum;

    void *payload;
};

/**
 * put short header
 * @param lh_ptr: short header ptr
 * @param payload: payload ptr
 * @param size: payload size
 * @return: buffer && buffer size
 * 
 */
struct buf spack_put_header(const struct short_header * const sh_ptr, void *payload, size_t size);

/**
 * get short header
 * @param buf: buf
 * @param size: buf size
 * @param connid_size: dst connid size
 * @return: short header
 * 
 */
struct short_header spack_get_header(void *buf, size_t size, size_t connid_size);

#endif

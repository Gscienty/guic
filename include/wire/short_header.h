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
 * @param payload: payload ptr
 * @param size: payload size
 * @param hdr: short header
 * @return: buffer && buffer size
 * 
 */
struct buf spack_put_header(void * const payload,
                            const size_t size,
                            const struct short_header * const hdr);

/**
 * get short header
 * @param hdr: short header
 * @param buf: buf
 * @param size: buf size
 * @param connid_size: dst connid size
 * @return: short header
 * 
 */
size_t spack_get_header(struct short_header * const hdr,
                        void *const buf,
                        const size_t size,
                        const size_t connid_size);

#endif

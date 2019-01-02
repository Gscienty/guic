#ifndef _GUIC_WIRE_LONG_HEADER_H
#define _GUIC_WIRE_LONG_HEADER_H

#include "wire/type.h"
#include "util/packet_number.h"
#include "util/rbtree.h"
#include "util/buffer.h"
#include <stddef.h>

struct long_header {
    enum long_header_packet_type type;
    version_t version;
    struct conn_id_t dst_connid;
    struct conn_id_t src_connid;
    uint64_t len;
    uint32_t pnum;

    uint8_t is_init;
    struct buf token;

    void *payload;
};

/**
 * put long header
 * @param lh_ptr: long header ptr
 * @param payload: payload ptr
 * @param size: payload size
 * @return: buffer && buffer size
 * 
 */
struct buf lpack_put_header(const struct long_header * const lh_ptr, void *payload, size_t size);

/**
 * get long header
 * @param buf: buf
 * @param size: buf size
 * @return: long header
 * 
 */
struct long_header lpack_get_header(void *buf, size_t size);

#endif

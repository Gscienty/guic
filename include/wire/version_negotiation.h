#ifndef _VERSION_NEGOTIATION_H
#define _VERSION_NEGOTIATION_H

#include "util/buffer.h"
#include "wire/type.h"
#include <stdint.h>

struct version_negotiation_header {
    version_t version;
    struct conn_id_t dst_connid;
    struct conn_id_t src_connid;

    void *payload;
};

/**
 * version negotiation packet put header
 * @param hdr: version negotiation header
 * @param payload: payload ptr
 * @param size: payload size
 * @return: version negotiation packet
 * 
 */
struct buf version_negotiation_put_header(const struct version_negotiation_header * const hdr,
                                          void *payload,
                                          size_t size);

/**
 * version negotiation packet get header
 * @param buf: buf
 * @param size: buf size
 * @return: version negotiation header
 * 
 */
size_t version_negotiation_get_header(void * const buf,
                                      const size_t size,
                                      struct version_negotiation_header * const hdr);


#endif

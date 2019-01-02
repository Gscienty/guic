#ifndef _VERSION_NEGOTIATION_H
#define _VERSION_NEGOTIATION_H

#include "util/buffer.h"
#include "wire/type.h"
#include <stdint.h>

struct __guic_support_version {
    uint32_t version;
    unsigned int magic;
};

#define GUIC_SUPPORT_VERSION_MAGIC 0x5e44104
#define GUIC_SUPPORT_VERSION_NAME(x) __guic_support_version__##x
#define GUIC_SUPPORT_VERSION_SECTION __attribute__((used, section(".support_version"), aligned(1)))
#define GUIC_SUPPORT_VERSION(x) \
    static struct __guic_support_version GUIC_SUPPORT_VERSION_NAME(x) \
    GUIC_SUPPORT_VERSION_SECTION = { \
        .version = x, \
        .magic = GUIC_SUPPORT_VERSION_MAGIC \
    }

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
struct version_negotiation_header version_negotiation_get_header(void *buf, size_t size);

/**
 * construct version negotiation payload
 * @return: version negotiation payload
 * 
 */
struct buf version_negotiation_construct();


#endif

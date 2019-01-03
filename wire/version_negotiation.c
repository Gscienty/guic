#include "util/bigendian.h"
#include "wire/version_negotiation.h"
#include "wire/pack_store.h"
#include <string.h>

GUIC_SUPPORT_VERSION(0x00000000);


#ifndef DEBUG
// support version

#endif

static struct __guic_support_version *__start = NULL;
static struct __guic_support_version *__end = NULL;
static int __support_version_size = 0;

static void __support_version_init()
{
    struct __guic_support_version *itr;

    if (__start != NULL && __end != NULL)
        return;

    __support_version_size = 0;
    __start = &GUIC_SUPPORT_VERSION_NAME(0x00000000);
    __end = &GUIC_SUPPORT_VERSION_NAME(0x00000000);

    while (1) {
        if ((__start - 1)->magic != GUIC_SUPPORT_VERSION_MAGIC)
            break;
        __start--;
    }

    while (1) {
        if ((__end + 1)->magic != GUIC_SUPPORT_VERSION_MAGIC)
            break;
        __end++;
    }
    __end++;

    for (itr = __start; itr != __end; itr++) {
        if (itr->version != 0x00000000)
            __support_version_size++;
    }
}

inline
static size_t
__version_negotiation_header_size(const struct version_negotiation_header * const hdr)
{
    return 1 + 4 + 1 + hdr->dst_connid.size + hdr->src_connid.size;
}

static int
__version_negotiation_header_encode(const struct version_negotiation_header * const hdr,
                                    void *buf,
                                    size_t size)
{
    size_t used_size = 0;
    uint8_t byte;

    // encode first byte
    *((uint8_t *) buf) = 0x80;
    used_size += 1;
    if (used_size >= size)
        return -1;

    // encode version
    bigendian_encode(buf + used_size, size - used_size, 0x00000000, 4);
    used_size += 4;
    if (used_size >= size)
        return -1;
    
    // encode cil
    byte = 0;
    if (hdr->dst_connid.size != 0)
        byte ^= (hdr->dst_connid.size - 3) << 4;
    if (hdr->src_connid.size != 0)
        byte ^= hdr->src_connid.size - 3;
    *((uint8_t *) (buf + used_size)) = byte;
    used_size += 1;
    if (used_size >= size)
        return -1;

    // encode dst connid
    memcpy(buf + used_size, hdr->dst_connid.bytes, hdr->dst_connid.size);
    used_size += hdr->dst_connid.size;
    if (used_size >= size)
        return -1;

    // encode src connid
    memcpy(buf + used_size, hdr->src_connid.bytes, hdr->src_connid.size);
    used_size += hdr->src_connid.size;
    if (used_size > size)
        return -1;

    return size - used_size;
}

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
                                          size_t size)
{
    size_t header_size;
    __support_version_init();

    header_size = __version_negotiation_header_size(hdr);
    __version_negotiation_header_encode(hdr, payload - header_size, header_size);

    struct buf pkt = {
        .size = header_size + size,
        .ptr = payload - header_size
    };

    return pkt;
}

/**
 * version negotiation packet get header
 * @param buf: buf
 * @param size: buf size
 * @return: version negotiation header
 * 
 */
struct version_negotiation_header version_negotiation_get_header(void *buf, size_t size)
{
    struct version_negotiation_header header = {
        .payload = NULL
    };
    size_t used_size = 1;
    uint8_t byte;
    if (used_size >= size)
        return header;

    // decode version
    bigendian_decode(&header.version, 4, buf, used_size);
    used_size += 4;
    if (used_size >= size)
        return header;

    // decode cil
    byte = *((uint8_t *) (buf + used_size));
    used_size += 1;
    if (used_size >= size)
        return header;

    // decode dst connid
    if ((byte & 0xF0) != 0x00) {
        header.dst_connid.size = ((byte & 0xF0) >> 4) + 3;
        memcpy(header.dst_connid.bytes, buf + used_size, header.dst_connid.size);
        used_size += header.dst_connid.size;
        if (used_size > size)
            return header;
    }

    // decode src connid
    if ((byte & 0x0F) != 0x00) {
        header.src_connid.size = (byte & 0x0F) + 3;
        memcpy(header.src_connid.bytes, buf + used_size, header.src_connid.size);
        used_size += header.src_connid.size;
        if (used_size > size)
            return header;
    }

    header.payload = buf + used_size;

    return header;
}

static void __version_negotiation_payload_fill(void *buf, size_t size)
{
    struct __guic_support_version *itr;
    size_t used_size = 0;

    for (itr = __start; itr != __end; itr++) {
        if (itr->version == 0x00000000)
            continue;
        bigendian_encode(buf + used_size, size - used_size, itr->version, 4);
        used_size += 4;
    }
}

/**
 * construct version negotiation payload
 * @return: version negotiation payload
 * 
 */
struct buf version_negotiation_construct()
{
    struct buf ret = {
        .size = 0,
        .ptr = NULL
    };
    size_t payload_size = __support_version_size * 4;
    void *buf = pack_malloc(VERSION_NEGOTIATION_HEADER_MAX_SIZE, payload_size);
    if (buf == NULL)
        return ret;

    ret.size = payload_size;
    ret.ptr = buf;

    __version_negotiation_payload_fill(buf, payload_size);

    return ret;
}

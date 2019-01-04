#include "util/bigendian.h"
#include "wire/version_negotiation.h"
#include "wire/pack_store.h"
#include <string.h>

inline static size_t
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
size_t version_negotiation_get_header(void * const buf,
                                      const size_t size,
                                      struct version_negotiation_header * const hdr)
{
    size_t used_size = 1;
    uint8_t byte;
    if (used_size >= size)
        return 0;

    hdr->version = 0;
    // decode version
    bigendian_decode(&hdr->version, 4, buf, used_size);
    used_size += 4;
    if (used_size >= size)
        return 0;

    // decode cil
    byte = *((uint8_t *) (buf + used_size));
    used_size += 1;
    if (used_size >= size)
        return 0;

    // decode dst connid
    if ((byte & 0xF0) != 0x00) {
        hdr->dst_connid.size = ((byte & 0xF0) >> 4) + 3;
        memcpy(hdr->dst_connid.bytes, buf + used_size, hdr->dst_connid.size);
        used_size += hdr->dst_connid.size;
        if (used_size > size)
            return 0;
    }

    // decode src connid
    if ((byte & 0x0F) != 0x00) {
        hdr->src_connid.size = (byte & 0x0F) + 3;
        memcpy(hdr->src_connid.bytes, buf + used_size, hdr->src_connid.size);
        used_size += hdr->src_connid.size;
        if (used_size > size)
            return 0;
    }

    hdr->payload = buf + used_size;

    return used_size;
}

#include "wire/short_header.h"
#include "util/packet_number.h"
#include "util/bigendian.h"
#include <stdlib.h>
#include <string.h>

inline static size_t __short_header_size(const struct short_header *sh_ptr)
{
    return 1                                  // short header first byte
        + sh_ptr->dst_connid.size             // dst connid size
        + packet_number_length(sh_ptr->pnum); // packet number size
}

static int __short_header_encode(void *buf,
                                 const struct short_header * const hdr,
                                 const size_t sh_size)
{
    size_t used_size = 0;
    size_t pnum_size = packet_number_length(hdr->pnum);
    uint8_t byte;

    byte = 0x40;
    byte ^= hdr->spin ? 0x20 : 0x00;
    byte ^= hdr->key_phase ? 0x04 : 0x00;
    byte ^= pnum_size - 1;

    // encode first byte
    *((uint8_t *) buf) = byte;
    used_size += 1;
    if (used_size >= sh_size)
        return -1;

    // encode dst connid
    if (hdr->dst_connid.size != 0) {
        memcpy(buf + used_size, hdr->dst_connid.bytes, hdr->dst_connid.size);
        used_size += hdr->dst_connid.size;
        if (used_size >= sh_size)
            return -1;
    }

    // encode packet number
    bigendian_encode(buf + used_size, sh_size - used_size, hdr->pnum, pnum_size);
    used_size += pnum_size;
    if (used_size > sh_size)
        return -1;

    return sh_size - used_size;
}

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
                            const struct short_header * const hdr)
{
    struct buf ret;
    size_t header_size = __short_header_size(hdr);

    ret.size = header_size + size;
    ret.ptr = payload - header_size;

    __short_header_encode(ret.ptr, hdr, header_size);

    return ret;
}

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
                        const size_t connid_size)
{
    uint8_t byte;
    size_t pnum_size;
    size_t used_size = 0;

    // decode first byte
    byte = *((uint8_t *) buf);
    used_size += 1;
    if (used_size >= size)
        return 0;

    if (byte & 0x20)
        hdr->spin = 1;
    if (byte & 0x04)
        hdr->key_phase = 1;
    pnum_size = (byte & 0x03) + 1;

    // decode dst connid
    hdr->dst_connid.size = connid_size;
    memcpy(hdr->dst_connid.bytes, buf + used_size, hdr->dst_connid.size);
    used_size += hdr->dst_connid.size;
    if (used_size >= size)
        return 0;
    
    // decode pnum
    bigendian_decode(&hdr->pnum, pnum_size, buf + used_size, size - used_size);
    used_size += pnum_size;
    if (used_size > size)
        return 0;

    hdr->payload = buf + used_size;

    return used_size;
}

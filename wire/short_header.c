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
                                 const struct short_header * const sh_ptr,
                                 const size_t sh_size)
{
    size_t used_size = 0;
    size_t pnum_size = packet_number_length(sh_ptr->pnum);
    uint8_t byte;

    byte = 0x40;
    byte ^= sh_ptr->spin ? 0x20 : 0x00;
    byte ^= sh_ptr->key_phase ? 0x04 : 0x00;
    byte ^= pnum_size - 1;

    // encode first byte
    *((uint8_t *) buf) = byte;
    used_size += 1;
    if (used_size >= sh_size)
        return -1;

    // encode dst connid
    if (sh_ptr->dst_connid.size != 0) {
        memcpy(buf + used_size, sh_ptr->dst_connid.bytes, sh_ptr->dst_connid.size);
        used_size += sh_ptr->dst_connid.size;
        if (used_size >= sh_size)
            return -1;
    }

    // encode packet number
    bigendian_encode(buf + used_size, sh_size - used_size, sh_ptr->pnum, pnum_size);
    used_size += pnum_size;
    if (used_size > sh_size)
        return -1;

    return sh_size - used_size;
}

/**
 * put short header
 * @param sh_ptr: short header ptr
 * @param payload: payload ptr
 * @return: buffer && buffer size
 * 
 */
struct buf spack_put_header(const struct short_header * const sh_ptr, void *payload, size_t size)
{
    struct buf ret;
    size_t header_size = __short_header_size(sh_ptr);

    ret.size = header_size + size;
    ret.ptr = payload - header_size;

    __short_header_encode(ret.ptr, sh_ptr, header_size);

    return ret;
}

/**
 * get short header
 * @param buf: buf
 * @param size: buf size
 * @param connid_size: dst connid size
 * @return: short header
 * 
 */
struct short_header spack_get_header(void *buf, size_t size, size_t connid_size)
{
    struct short_header header = {
        .payload = NULL
    };
    uint8_t byte;
    size_t pnum_size;
    size_t used_size = 0;

    // decode first byte
    byte = *((uint8_t *) buf);
    used_size += 1;
    if (used_size >= size)
        return header;

    if (byte & 0x20)
        header.spin = 1;
    if (byte & 0x04)
        header.key_phase = 1;
    pnum_size = (byte & 0x03) + 1;

    // decode dst connid
    header.dst_connid.size = connid_size;
    memcpy(header.dst_connid.bytes, buf + used_size, header.dst_connid.size);
    used_size += header.dst_connid.size;
    if (used_size >= size)
        return header;
    
    // decode pnum
    bigendian_decode(&header.pnum, pnum_size, buf + used_size, size - used_size);
    used_size += pnum_size;
    if (used_size > size)
        return header;

    header.payload = buf + used_size;

    return header;
}

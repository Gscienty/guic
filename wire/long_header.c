#include "wire/long_header.h"
#include "util/variable_integer.h"
#include "util/bigendian.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

static size_t __long_header_size(const struct long_header * const lh_ptr)
{
    return 1                                  // long header first byte
        + 4                                   // version
        + 1                                   // DCIL && SCIL
        + lh_ptr->dst_connid.size             // dst connid size
        + lh_ptr->src_connid.size             // src connid size
        + varint_size(lh_ptr->len)            // length
        + packet_number_length(lh_ptr->pnum); // packet number
}

static int __long_header_encode(void *buf,
                                 const struct long_header * const lh_ptr,
                                 const size_t lh_size)
{
    size_t used_size = 0;
    size_t pnum_size = packet_number_length(lh_ptr->pnum);
    uint8_t byte;

    byte  = 0xC0;                // header form && fixed bit
    byte ^= (lh_ptr->type << 4); // long packet type
    byte ^= (pnum_size - 1);     // packet number length
    *((uint8_t *) buf) = byte;
    used_size += 1;
    if (used_size >= lh_size)
        return -1;

    // encode version
    if (bigendian_encode(buf + used_size, lh_size - used_size, lh_ptr->version, 4) < 0)
        return -1;
    used_size += 4;
    if (used_size >= lh_size)
        return -1;

    // encode cli
    byte = 0x00;
    if (lh_ptr->dst_connid.size != 0)
        byte ^= (lh_ptr->dst_connid.size - 3) << 4;
    if (lh_ptr->src_connid.size != 0)
        byte ^= lh_ptr->src_connid.size - 3;
    *((uint8_t *) (buf + used_size)) = byte;
    used_size += 1;
    if (used_size >= lh_size)
        return -1;

    // encode dst connid
    memcpy(buf + used_size, lh_ptr->dst_connid.bytes, lh_ptr->dst_connid.size);
    used_size += lh_ptr->dst_connid.size;
    if (used_size >= lh_size)
        return -1;

    // encode src connid
    memcpy(buf + used_size, lh_ptr->src_connid.bytes, lh_ptr->src_connid.size);
    used_size += lh_ptr->src_connid.size;
    if (used_size >= lh_size)
        return -1;

    used_size += varint_encode(buf + used_size, lh_size - used_size, lh_ptr->len);
    if (used_size >= lh_size)
        return -1;

    bigendian_encode(buf + used_size, lh_size - used_size, lh_ptr->pnum, pnum_size);
    used_size += pnum_size;
    if (used_size >= lh_size)
        return -1;

    return lh_size - 1 - used_size;
}

/**
 * put long header
 * @param lh_ptr: long header ptr
 * @param payload: payload ptr
 * @return: buffer && buffer size
 * 
 */
struct buf lpack_put_header(struct long_header * const lh_ptr, void *payload, size_t size)
{
    struct buf ret;
    size_t header_size = __long_header_size(lh_ptr);

    ret.size = header_size + size;
    ret.ptr = payload - header_size;

    __long_header_encode(ret.ptr, lh_ptr, header_size);

    return ret;
}

/**
 * get long header
 * @param buf: buf
 * @param size: buf size
 * @return: long header
 * 
 */
struct long_header lpack_get_header(void *buf, size_t size)
{
    size_t used_size = 0;
    size_t pnum_size;
    uint8_t cil;
    struct long_header header = {
        .payload = NULL
    };
    
    // get first byte
    uint8_t first_byte = *((uint8_t *) buf);
    used_size += 1;
    if (used_size >= size)
        return header;
    if ((first_byte & 0xC0) != 0xC0)
        return header;
    pnum_size = (0x03 & first_byte) + 1;
    header.type = (enum long_header_packet_type) ((first_byte & 0x30) >> 4);

    // decode version
    bigendian_decode(buf + used_size, size - used_size, &header.version, 4);
    used_size += 4;
    if (used_size >= size)
        return header;

    // decode dcil && scil
    cil = *((uint8_t *) buf + used_size);
    used_size += 1;
    if (used_size >= size)
        return header;

    // decode dst_connid
    if ((cil & 0xF0) != 0x00) {
        header.dst_connid.size = ((cil & 0xF0) >> 4) + 3;
        memcpy(header.dst_connid.bytes, buf + used_size, header.dst_connid.size);
        used_size += header.dst_connid.size;
        if (used_size >= size)
            return header;
    }
    else
        header.dst_connid.size = 0;

    // decode src_connid
    if ((cil & 0x0F) != 0x00) {
        header.src_connid.size = (cil & 0x0F) + 3;
        memcpy(header.src_connid.bytes, buf + used_size, header.src_connid.size);
        used_size += header.src_connid.size;
        if (used_size >= size)
            return header;
    }
    else
        header.src_connid.size = 0;

    // decode length
    used_size += varint_decode(buf + used_size, size - used_size, &header.len);
    if (used_size >= size)
        return header;

    // decode packet number
    used_size += bigendian_decode(buf + used_size, size - used_size, &header.pnum, pnum_size);
    if (used_size > size)
        return header;

    // set payload ptr
    header.payload = buf + used_size;

    return header;
}

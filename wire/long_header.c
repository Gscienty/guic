#include "wire/long_header.h"
#include "util/variable_integer.h"
#include "util/bigendian.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

inline static size_t __long_header_size(const struct long_header * const hdr)
{
    return 1                                                  // long header first byte
        + 4                                                   // version
        + 1                                                   // DCIL && SCIL
        + hdr->dst_connid.size                                // dst connid size
        + hdr->src_connid.size                                // src connid size
        + (hdr->type == LONG_HEADER_PACKET_INITIAL            // init packet token
           ? (hdr->token.size + varint_size(hdr->token.size))
           : 0)
        + (hdr->type == LONG_HEADER_PACKET_RETRY              // retry packet
           ? hdr->ori_dst_connid.size
           : (varint_size(hdr->len)                           // length
              + packet_number_length(hdr->pnum)));            // packet number
}

static int __long_header_encode(void *buf,
                                 const struct long_header * const hdr,
                                 const size_t hdr_size)
{
    size_t used_size = 0;
    size_t pnum_size;
    uint8_t byte;

    byte  = 0xC0;                // header form && fixed bit
    byte ^= (hdr->type << 4); // long packet type
    if (hdr->type == LONG_HEADER_PACKET_RETRY)
        byte ^= hdr->ori_dst_connid.size - 3; // packet origin dst size
    else {
        pnum_size = packet_number_length(hdr->pnum);
        byte ^= (pnum_size - 1);     // packet number length
    }
    *((uint8_t *) buf) = byte;
    used_size += 1;
    if (used_size >= hdr_size)
        return -1;

    // encode version
    if (bigendian_encode(buf + used_size, hdr_size - used_size, hdr->version, 4) < 0)
        return -1;
    used_size += 4;
    if (used_size >= hdr_size)
        return -1;

    // encode cli
    byte = 0x00;
    if (hdr->dst_connid.size != 0)
        byte ^= (hdr->dst_connid.size - 3) << 4;
    if (hdr->src_connid.size != 0)
        byte ^= hdr->src_connid.size - 3;
    *((uint8_t *) (buf + used_size)) = byte;
    used_size += 1;
    if (used_size >= hdr_size)
        return -1;

    // encode dst connid
    memcpy(buf + used_size, hdr->dst_connid.bytes, hdr->dst_connid.size);
    used_size += hdr->dst_connid.size;
    if (used_size >= hdr_size)
        return -1;

    // encode src connid
    memcpy(buf + used_size, hdr->src_connid.bytes, hdr->src_connid.size);
    used_size += hdr->src_connid.size;
    if (used_size >= hdr_size)
        return -1;

    // encode token (init packet)
    if (hdr->type == LONG_HEADER_PACKET_INITIAL) {
        // encode token length
        used_size += varint_encode(buf + used_size, hdr_size - used_size, hdr->token.size);
        if (used_size >= hdr_size)
            return -1;

        // encode token
        memcpy(buf + used_size, hdr->token.ptr, hdr->token.size);
        used_size += hdr->token.size;
        if (used_size >= hdr_size)
            return -1;
    }

    if (hdr->type == LONG_HEADER_PACKET_RETRY) {
        memcpy(buf + used_size, hdr->ori_dst_connid.bytes, hdr->ori_dst_connid.size);
        used_size += hdr->ori_dst_connid.size;
        if (used_size > hdr_size)
            return -1;
    }
    else {
        // encode length
        used_size += varint_encode(buf + used_size, hdr_size - used_size, hdr->len);
        if (used_size >= hdr_size)
            return -1;

        // encode packet number
        bigendian_encode(buf + used_size, hdr_size - used_size, hdr->pnum, pnum_size);
        used_size += pnum_size;
        if (used_size > hdr_size)
            return -1;
    }

    return hdr_size - used_size;
}

/**
 * put long header
 * @param lh_ptr: long header ptr
 * @param payload: payload ptr
 * @param size: payload size
 * @return: buffer && buffer size
 * 
 */
struct buf lpack_put_header(void * const payload,
                            const size_t size,
                            const struct long_header * const hdr)
{
    struct buf ret;
    size_t header_size = __long_header_size(hdr);

    ret.size = header_size + size;
    ret.ptr = payload - header_size;

    __long_header_encode(ret.ptr, hdr, header_size);

    return ret;
}

/**
 * get long header
 * @param hdr: long header
 * @param buf: buf
 * @param size: buf size
 * @return: long header
 * 
 */
size_t lpack_get_header(struct long_header * const hdr,
                        void * const buf,
                        const size_t size)
{
    size_t used_size = 0;
    size_t pnum_size;
    uint8_t cil;
    
    // get first byte
    uint8_t first_byte = *((uint8_t *) buf);
    used_size += 1;
    if (used_size >= size)
        return 0;
    if ((first_byte & 0xC0) != 0xC0)
        return 0;
    hdr->type = (enum long_header_packet_type) ((first_byte & 0x30) >> 4);
    if (hdr->type == LONG_HEADER_PACKET_RETRY && (first_byte & 0x0F) != 0x00)
        hdr->ori_dst_connid.size = (first_byte & 0x0F) + 3;
    else
        pnum_size = (0x03 & first_byte) + 1;

    // decode version
    hdr->version = 0;
    bigendian_decode(&hdr->version, 4, buf + used_size, size - used_size);
    used_size += 4;
    if (used_size >= size)
        return 0;

    // decode dcil && scil
    cil = *((uint8_t *) buf + used_size);
    used_size += 1;
    if (used_size >= size)
        return 0;

    // decode dst_connid
    hdr->dst_connid.size = 0;
    if ((cil & 0xF0) != 0x00) {
        hdr->dst_connid.size = ((cil & 0xF0) >> 4) + 3;
        memcpy(hdr->dst_connid.bytes, buf + used_size, hdr->dst_connid.size);
        used_size += hdr->dst_connid.size;
        if (used_size >= size)
            return 0;
    }

    // decode src_connid
    hdr->src_connid.size = 0;
    if ((cil & 0x0F) != 0x00) {
        hdr->src_connid.size = (cil & 0x0F) + 3;
        memcpy(hdr->src_connid.bytes, buf + used_size, hdr->src_connid.size);
        used_size += hdr->src_connid.size;
        if (used_size >= size)
            return 0;
    }

    // decode token
    if (hdr->type == LONG_HEADER_PACKET_INITIAL) {
        // decode token length
        hdr->token.size = 0;
        used_size += varint_decode(&hdr->token.size, buf + used_size, size - used_size);

        // decode token
        hdr->token.ptr = malloc(hdr->token.size);
        memcpy(hdr->token.ptr, buf + used_size, hdr->token.size);
        used_size += hdr->token.size;
    }

    if (hdr->type == LONG_HEADER_PACKET_RETRY) {
        // decode ori dst connid
        memcpy(hdr->ori_dst_connid.bytes, buf + used_size, hdr->ori_dst_connid.size);
        used_size += hdr->ori_dst_connid.size;
    }
    else {
        // decode length
        hdr->len = 0;
        used_size += varint_decode(&hdr->len, buf + used_size, size - used_size);
        if (used_size >= size)
            return 0;

        // decode packet number
        hdr->pnum = 0;
        bigendian_decode(&hdr->pnum, pnum_size, buf + used_size, size - used_size);
        used_size += pnum_size;
        if (used_size > size)
            return 0;
    }

    // set payload ptr
    hdr->payload = buf + used_size;

    return used_size;
}

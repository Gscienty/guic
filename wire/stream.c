#include "wire/stream.h"
#include "util/variable_integer.h"
#include <string.h>
#include <malloc.h>

inline static size_t __size(const struct stream * const frm)
{
    return varint_size(frm->stream_id)
        + (frm->off != 0 ? varint_size(frm->off) : 0)
        + (frm->len != 0 ? varint_size(frm->len) : 0)
        + frm->len;
}

/**
 * get stream frame size
 * @param frm: stream frame
 * @return size
 * 
 */
size_t stream_size(const struct stream * const frm)
{
    return __size(frm);
}

/**
 * encode stream frame
 * @param type: type
 * @param buf: buf
 * @param size: buf size
 * @param frm: stream frame
 * @return: used size
 * 
 */
size_t stream_encode(uint8_t * const type,
                     void * const buf,
                     const size_t size,
                     const struct stream * const frm)
{
    size_t used_size = 0;

    if (frm->fin)
        *type ^= 0x01;

    // encode stream id
    used_size += varint_encode(buf + used_size, size - used_size, frm->stream_id);
    if (used_size >= size)
        return 0;

    // encode offset
    if (frm->off) {
        used_size += varint_encode(buf + used_size, size - used_size, frm->off);
        if (used_size > size)
            return 0;
    }

    // encode length
    if (frm->len) {
        used_size += varint_encode(buf + used_size, size - used_size, frm->len);
        if (used_size > size)
            return 0;
    }

    // encode data
    if (frm->data) {
        memcpy(buf + used_size, frm->data, frm->len);
        used_size += frm->len;
        if (used_size > size)
            return 0;
    }

    return used_size;
}

/**
 * decode stream frame
 * @param frm: stream frame
 * @param buf: buf
 * @param size: buf size
 * @param type: type byte
 * @return: used size
 * 
 */
size_t stream_decode(struct stream * const frm,
                     void * const buf,
                     const size_t size,
                     const uint8_t type)
{
    size_t used_size = 0;

    if (type & 0x01)
        frm->fin = 1;

    // decode stream id
    used_size += varint_decode(&frm->stream_id, buf + used_size, size - used_size);
    if (used_size >= size)
        return 0;

    // decode offset
    if (0x04 & type) {
        used_size += varint_decode(&frm->off, buf + used_size, size - used_size);
        if (used_size > size)
            return 0;
    }

    // decode len
    if (0x02 & type) {
        used_size += varint_decode(&frm->len, buf + used_size, size - used_size);
        if (used_size > size)
            return 0;
    }

    if (!frm->len)
        frm->len = size - used_size;

    // decode data
    frm->data = malloc(frm->len);
    if (frm->data == NULL)
        return 0;
    memcpy(frm->data, buf + used_size, frm->len);

    return used_size + frm->len;
}

/**
 * get stream type byte
 * @param frm: stream frame
 * @return: type byte
 *
 */
uint8_t stream_type_byte(const struct stream * const frm)
{
    uint8_t type = 0x08;
    if (frm->off)
        type ^= 0x04;
    if (frm->len)
        type ^= 0x02;
    if (frm->fin)
        type ^= 0x01;
    return type;
}

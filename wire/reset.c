#include "wire/reset.h"
#include "util/variable_integer.h"
#include "util/bigendian.h"

inline static size_t __size(const struct reset * const frm)
{
    return varint_size(frm->stream_id) + 2 + varint_size(frm->offset);
}

/**
 * get reset size
 * @param frm: reset frame
 * @return: reset frame size
 *
 */
size_t reset_size(const struct reset * const frm)
{
    return __size(frm);
}

/**
 * put reset frame to buffer
 * @param buf: buffer
 * @param size: buf size
 * @param frm: reset frame
 * @return: used size (fail return 0)
 * 
 */
size_t reset_encode(void * const buf, const size_t size, const struct reset * const frm)
{
    size_t frame_size = __size(frm);
    size_t used_size = 0;
    if (frame_size > size)
        return 0;

    // encode reset stream id
    used_size += varint_encode(buf, size - used_size, frm->stream_id);

    // encode app err code
    bigendian_encode(buf, size - used_size, frm->err, 2);
    used_size += 2;

    // encode final offset
    used_size += varint_encode(buf, size - used_size, frm->offset);

    return frame_size;
}

/**
 * get reset frame
 * @param frm: frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t reset_decode(struct reset * const frm, void * const buf, const size_t size)
{
    size_t used_size = 0;

    // decode stream id
    used_size += varint_decode(&frm->stream_id, buf + used_size, size - used_size);

    // decode app err code
    bigendian_decode(&frm->err, 2, buf + used_size, size - used_size);
    used_size += 2;

    // decode final offset
    used_size += varint_decode(&frm->offset, buf + used_size, size - used_size);

    return used_size;
}

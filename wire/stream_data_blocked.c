#include "wire/stream_data_blocked.h"
#include "util/variable_integer.h"

inline static size_t __size(const struct stream_data_blocked * const frm)
{
    return varint_size(frm->stream_id) + varint_size(frm->limit);
}

/**
 * get limit data frame size
 * @param frm: limit data frame
 * @return: frame size
 * 
 */
size_t stream_data_blocked_size(const struct stream_data_blocked * const frm)
{
    return __size(frm);
}

/**
 * encode limit data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: limit data frame
 * @return: used size
 *
 */
size_t stream_data_blocked_encode(void * const buf,
                              const size_t size,
                              const struct stream_data_blocked * const frm)
{
    size_t used_size = 0;

    used_size += varint_encode(buf + used_size, size - used_size, frm->stream_id);
    if (used_size >= size)
        return 0;

    used_size += varint_encode(buf + used_size, size - used_size, frm->limit);
    if (used_size > size)
        return 0;

    return used_size;
}

/**
 * decode limit data frame
 * @param frm: limit data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t stream_data_blocked_decode(struct stream_data_blocked * const frm,
                              void * const buf,
                              const size_t size)
{
    size_t used_size = 0;

    frm->stream_id = 0;
    // decode stream id
    used_size += varint_decode(&frm->stream_id, buf + used_size, size - used_size);
    if (used_size > size)
        return 0;

    frm->limit = 0;
    // decode limit
    used_size += varint_decode(&frm->limit, buf + used_size, size - used_size);
    if (used_size > size)
        return 0;

    return used_size;
}

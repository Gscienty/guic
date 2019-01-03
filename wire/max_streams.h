
#include "wire/max_streams.h"
#include "util/variable_integer.h"

inline static size_t __size(const struct max_streams * const frm)
{
    return varint_size(frm->max);
}

/**
 * get max data frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t max_streams_size(const struct max_streams * const frm)
{
    return __size(frm);
}

/**
 * encode max data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: max data frame
 * @return: used size
 *
 */
size_t max_streams_encode(void * const buf,
                       const size_t size,
                       const struct max_streams * const frm)
{
    size_t used_size = 0;

    used_size += varint_encode(buf, size, frm->max);
    if (used_size > size)
        return 0;

    return used_size;
}

/**
 * decode max data frame
 * @param frm: max data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t max_streams_decode(struct max_streams * const frm,
                       void * const buf,
                       const size_t size)
{
    size_t used_size = 0;

    frm->max = 0;
    // decode max
    used_size += varint_decode(&frm->max, buf, size);
    if (used_size > size)
        return 0;

    return used_size;
}

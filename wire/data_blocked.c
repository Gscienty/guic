#include "wire/data_blocked.h"
#include "util/variable_integer.h"

inline static size_t __size(const struct data_blocked * const frm)
{
    return varint_size(frm->limited);
}

/**
 * get max data frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t data_blocked_size(const struct data_blocked * const frm)
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
size_t data_blocked_encode(void * const buf,
                       const size_t size,
                       const struct data_blocked * const frm)
{
    size_t used_size = 0;

    used_size += varint_encode(buf, size, frm->limited);
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
size_t data_blocked_decode(struct data_blocked * const frm,
                       void * const buf,
                       const size_t size)
{
    size_t used_size = 0;

    frm->limited = 0;
    // decode limit
    used_size += varint_decode(&frm->limited, buf, size);
    if (used_size > size)
        return 0;

    return used_size;
}

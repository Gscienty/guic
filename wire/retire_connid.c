#include "wire/retire_connid.h"
#include "util/variable_integer.h"

inline static size_t __size(const struct retire_connid * const frm)
{
    return varint_size(frm->seq);
}

/**
 * get seq data frame size
 * @param frm: seq data frame
 * @return: frame size
 * 
 */
size_t retire_connid_size(const struct retire_connid * const frm)
{
    return __size(frm);
}

/**
 * encode seq data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: seq data frame
 * @return: used size
 *
 */
size_t retire_connid_encode(void * const buf,
                       const size_t size,
                       const struct retire_connid * const frm)
{
    size_t used_size = 0;

    used_size += varint_encode(buf, size, frm->seq);
    if (used_size > size)
        return 0;

    return used_size;
}

/**
 * decode seq data frame
 * @param frm: seq data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t retire_connid_decode(struct retire_connid * const frm,
                       void * const buf,
                       const size_t size)
{
    size_t used_size = 0;

    frm->seq = 0;
    // decode seq
    used_size += varint_decode(&frm->seq, buf, size);
    if (used_size > size)
        return 0;

    return used_size;
}

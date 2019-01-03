#include "wire/path.h"
#include "util/variable_integer.h"
#include <string.h>

/**
 * get data data frame size
 * @param frm: data data frame
 * @return: frame size
 * 
 */
size_t path_size(const struct path * const frm)
{
    return sizeof(frm->data);
}

/**
 * encode data data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: data data frame
 * @return: used size
 *
 */
size_t path_encode(void * const buf,
                       const size_t size,
                       const struct path * const frm)
{
    size_t used_size = 0;

    memcpy(buf, frm->data, 8);
    used_size += 8;
    if (used_size > size)
        return 0;

    return used_size;
}

/**
 * decode data data frame
 * @param frm: data data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t path_decode(struct path * const frm,
                       void * const buf,
                       const size_t size)
{
    size_t used_size = 0;

    // decode data
    memcpy(frm->data, buf, 8);
    used_size += 8;
    if (used_size > size)
        return 0;

    return used_size;
}

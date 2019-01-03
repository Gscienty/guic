#include "wire/crypto.h"
#include "util/variable_integer.h"
#include <string.h>
#include <malloc.h>

static inline size_t __size(const struct crypto * const frm)
{
    return varint_size(frm->offset) + varint_size(frm->len) + frm->len;
}

/**
 * get crypto frame size
 * @param frm: crypto frame
 * @return: frame size
 * 
 */
size_t crypto_size(const struct crypto * const frm)
{
    return __size(frm);
}

/**
 * encode crypto frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: crypto frame
 * @return: used size
 * 
 */
size_t crypto_encode(void * const buf, const size_t size, const struct crypto * const frm)
{
    size_t used_size = 0;

    // encode offset
    used_size += varint_encode(buf + used_size, size - used_size, frm->offset);

    // encode len
    used_size += varint_encode(buf + used_size, size - used_size, frm->len);

    // encode data
    memcpy(buf + used_size, frm->data, frm->len);

    return used_size + frm->len;
}

/**
 * decode crypto frame
 * @param frm: crypto frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t crypto_decode(struct crypto * const frm, void * const buf, const size_t size)
{
    size_t used_size = 0;

    // decode offset
    used_size += varint_decode(buf + used_size, size - used_size, &frm->offset);

    // decode len
    used_size += varint_decode(buf + used_size, size - used_size, &frm->len);

    // decode data
    frm->data = malloc(frm->len);
    if (frm->data == NULL)
        return 0;
    memcpy(frm->data, buf + used_size, frm->len);

    return used_size + frm->len;
}

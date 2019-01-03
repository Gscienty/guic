#include "wire/token.h"
#include "util/variable_integer.h"
#include <string.h>
#include <malloc.h>

inline static size_t __size(const struct token * const frm)
{
    return varint_size(frm->len) + frm->len;
}

/**
 * get token frame size
 * @param frm: token frame
 * @return: frame size
 * 
 */
size_t token_size(const struct token * const frm)
{
    return __size(frm);
}

/**
 * encode token frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: token frame
 * @return: used size
 * 
 */
size_t token_encode(void * const buf, const size_t size, const struct token * const frm)
{
    size_t used_size = 0;

    // encode length
    used_size += varint_encode(buf + used_size, size - used_size, frm->len);

    // encode token
    memcpy(buf + used_size, frm->token, frm->len);    

    return used_size + frm->len;
}

/**
 * decode token frame
 * @param frm: token frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t token_decode(struct token * const frm, void * const buf, const size_t size)
{
    size_t used_size = 0;

    frm->len = 0;
    // decode len
    used_size += varint_decode(&frm->len, buf, size);

    // decode token
    frm->token = malloc(frm->len);
    if (frm->token == NULL)
        return 0;
    memcpy(frm->token, buf + used_size, frm->len);

    return used_size + frm->len;
}

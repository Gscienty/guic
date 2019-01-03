#ifndef _GUIC_WIRE_TOKEN_H
#define _GUIC_WIRE_TOKEN_H

#include <stddef.h>

struct token {
    size_t len;
    void *token;
};

/**
 * get token frame size
 * @param frm: token frame
 * @return: frame size
 * 
 */
size_t token_size(const struct token * const frm);

/**
 * encode token frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: token frame
 * @return: used size
 * 
 */
size_t token_encode(void * const buf, const size_t size, const struct token * const frm);

/**
 * decode token frame
 * @param frm: token frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t token_decode(struct token * const frm, void * const buf, const size_t size);

#endif

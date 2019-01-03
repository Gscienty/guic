#ifndef _GUIC_WIRE_CRYPTO_H
#define _GUIC_WIRE_CRYPTO_H

#include "wire/type.h"
#include <stddef.h>

struct crypto {
    size_t offset;
    stream_id_t len;
    void *data;
};

/**
 * get crypto frame size
 * @param frm: crypto frame
 * @return: frame size
 * 
 */
size_t crypto_size(const struct crypto * const frm);

/**
 * encode crypto frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: crypto frame
 * @return: used size
 * 
 */
size_t crypto_encode(void * const buf, const size_t size, const struct crypto * const frm);

/**
 * decode crypto frame
 * @param frm: crypto frame
 * @param buf: buf
 * @param size: buf size
 * @return: used size
 * 
 */
size_t crypto_decode(struct crypto * const frm, void * const buf, const size_t size);

#endif

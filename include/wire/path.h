#ifndef _GUIC_WIRE_PATH_H
#define _GUIC_WIRE_PATH_H

#include <stdint.h>
#include <stddef.h>

struct path {
    uint8_t data[8];
};

/**
 * get max data frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t path_size(const struct path * const frm);

/**
 * encode max data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: max data frame
 * @return: used size
 *
 */
size_t path_encode(void * const buf,
                   const size_t size,
                   const struct path * const frm);

/**
 * decode max data frame
 * @param frm: max data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t path_decode(struct path * const frm,
                   void * const buf,
                   const size_t size);

#endif

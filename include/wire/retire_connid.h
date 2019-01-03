#ifndef _GUIC_WIRE_RETIRE_CONNID_T
#define _GUIC_WIRE_RETIRE_CONNID_T

#include <stdint.h>
#include <stddef.h>

struct retire_connid {
    uint64_t seq;
};

/**
 * get max data frame size
 * @param frm: max data frame
 * @return: frame size
 * 
 */
size_t retire_connid_size(const struct retire_connid * const frm);

/**
 * encode max data frame
 * @param buf: buf
 * @param size: buf size
 * @param frm: max data frame
 * @return: used size
 *
 */
size_t retire_connid_encode(void * const buf,
                            const size_t size,
                            const struct retire_connid * const frm);

/**
 * decode max data frame
 * @param frm: max data frame
 * @param buf: buf
 * @param size: buf size
 * @return used size
 * 
 */
size_t retire_connid_decode(struct retire_connid * const frm,
                            void * const buf,
                            const size_t size);

#endif

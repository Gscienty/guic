#ifndef _GUIC_UTIL_BIGENDIAN_H
#define _GUIC_UTIL_BIGENDIAN_H

#include <stddef.h>
#include <stdint.h>

/**
 * encode value (big endian)
 * @param buf: buf
 * @param size: buf size
 * @param val: value will be encoded
 * @param val_size: value size
 * @return: encode used size
 * 
 */
int bigendian_encode(void *const buf,
                     const size_t size,
                     const uint64_t val,
                     const size_t val_size);

/**
 * decode value (big endian)
 * @param value: ref value
 * @param val_size: value size
 * @param buf: buf
 * @param size: buf size
 * @return: 0 is success
 *
 */
int bigendian_decode(void * const val,
                     const size_t val_size,
                     void * const buf,
                     const size_t size);

#endif

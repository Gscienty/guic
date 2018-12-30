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
int bigendian_encode(void *buf, size_t size, uint64_t val, size_t val_size);

#endif

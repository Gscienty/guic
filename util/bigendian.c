#include "util/bigendian.h"

/**
 * encode value (big endian)
 * @param buf: buf
 * @param size: buf size
 * @param val: value will be encoded
 * @param val_size: value size
 * @return: encode used size
 * 
 */
int bigendian_encode(void *buf, size_t size, uint64_t val, size_t val_size)
{
    if (size < val_size)
        return -1;

    uint8_t *byte_ptr = (uint8_t *) buf;
    size_t remain_bytes = val_size * 8;

    while (remain_bytes) {
        *byte_ptr++ = (val >> (remain_bytes -= 8)) & 0xFF;
    }

    return val_size;
}

/**
 * decode value (big endian)
 * @param buf: buf
 * @param size: buf size
 * @param value: ref value
 * @param val_size: value size
 * @return: 0 is success
 *
 */
int bigendian_decode(void *buf, size_t size, void *val, size_t val_size)
{
    size_t i;
    if (size < val_size)
        return -1;

    for (i = 0; i < val_size; i++) {
        ((uint8_t *) val)[i] = ((uint8_t *) buf)[val_size - 1 - i];
    }

    return 0;
}

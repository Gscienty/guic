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

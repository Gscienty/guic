#include "util/variable_integer.h"

uint64_t varint_decode(const uint8_t *buf, const size_t size)
{
    uint64_t ret = *buf & 0x3F;
    int remain_length = 0;
    int i;

    switch (0xC0 & *buf) {
        case 0x00:  // i bytes
            remain_length = 0;
            break;
        case 0x40:  // 2 bytes
            if (size < 2)
                ret = VARINT_ERROR_VAL;
            else {
                remain_length = 1;
            }
            break;
        case 0x80:
            if (size < 4)
                ret = VARINT_ERROR_VAL;
            else
                remain_length = 3;
            break;
        case 0xC0:
            if (size < 8)
                ret = VARINT_ERROR_VAL;
            else
                remain_length = 7;
            break;
        default:
            ret = VARINT_ERROR_VAL;
            remain_length = 0;
    }

    if (ret == VARINT_ERROR_VAL)
        return ret;

    for (i = 0; i < remain_length; i++) {
        ret <<= 8;
        ret ^= *(buf + 1 + i);
    }

    return ret;
}

int varint_encode(uint8_t * const buf, const size_t size, uint64_t val)
{
    int remain_length = 0;
    int i;

    if (val < 0x40) {
        // 1 bytes
        if (size < 1)
            return VARINT_ENCODE_ERROR_BUFFER_LIMIT;
        *buf = val;
        remain_length = 0;
        return 1;
    }
    else if (val < 0x4000) {
        // 2 bytes
        if (size < 2)
            return VARINT_ENCODE_ERROR_BUFFER_LIMIT;
        *buf = (val >> 8) ^ 0x40;
        remain_length = 1;
    }
    else if (val < 0x40000000) {
        // 4 bytes
        if (size < 4)
            return VARINT_ENCODE_ERROR_BUFFER_LIMIT;
        *buf = (val >> 24) ^ 0x80;
        remain_length = 3;
    }
    else if (val < 0x4000000000000000) {
        // 8 bytes
        if (size < 8)
            return VARINT_ENCODE_ERROR_BUFFER_LIMIT;
        *buf = (val >> 56) ^ 0xC0;
        remain_length = 7;
    }
    else {
        return VARINT_ENCODE_ERROR_NUM_TOO_LARGE;
    }

    for (i = 0; i < remain_length; i++) {
        *(buf + 1 + i) = (val >> ((remain_length - 1 - i) * 8)) & 0xFF;
    }

    return remain_length + 1;
}

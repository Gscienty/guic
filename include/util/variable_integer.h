#ifndef _GUIC_UNIT_VARIABLE_INTEGER_H
#define _GUIC_UNIT_VARIABLE_INTEGER_H

#include <stdint.h>
#include <stddef.h>

#define VARINT_ERROR_VAL 0xFFFFFFFFFFFFFFFF
#define VARINT_ENCODE_ERROR_NUM_TOO_LARGE -1
#define VARINT_ENCODE_ERROR_BUFFER_LIMIT -2

uint64_t varint_decode(const uint8_t *buf, const size_t size);
int varint_encode(uint8_t * const buf, const size_t size, uint64_t val);


#endif

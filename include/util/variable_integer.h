#ifndef _GUIC_UTIL_VARIABLE_INTEGER_H
#define _GUIC_UTIL_VARIABLE_INTEGER_H

#include <stdint.h>
#include <stddef.h>

#define VARINT_ERROR_VAL 0xFFFFFFFFFFFFFFFF
#define VARINT_ENCODE_ERROR_NUM_TOO_LARGE -1
#define VARINT_ENCODE_ERROR_BUFFER_LIMIT -2

/**
 * decode variable integer
 * @param val: decoded value
 * @param buf: readable buf
 * @param size: readable buf size
 * @return: variable integer object's length OR VARINT_ERROR_VAL
 *
 */
size_t varint_decode(uint64_t * const val,
                     const void *buf,
                     const size_t size);

/**
 * encode variable integer
 * @param buf: writable buf
 * @param size: writable buf size
 * @param val: variable integer object's value
 * @return: VARINT_ENCODE_ERROR OR encode used size
 *
 */
int varint_encode(void * const buf,
                  const size_t size,
                  const uint64_t val);

/**
 * get variable integer using buf's size
 * @param val: variable integer object's value
 * @return: size
 * 
 */
size_t varint_size(uint64_t val);


#endif

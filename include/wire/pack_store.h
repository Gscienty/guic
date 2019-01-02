#ifndef _GUIC_WIRE_PACK_STORE_H
#define _GUIC_WIRE_PACK_STORE_H

#include "util/rbtree.h"
#include <stddef.h>

struct __pack_obj {
    struct rbnode node;
    size_t header_size;
    void *ptr;
};

/**
 * malloc long header packet
 * @param size: packet payload size
 * @return: readable/writable buffer
 *
 */
void *pack_malloc(size_t header_size, size_t size);

/**
 * free pack header
 * @param ptr: payload buffer ptr
 *
 */
void pack_free(void *ptr);

#define LONG_HEADER_MAX_SIZE 54
#define SHORT_HEADER_MAX_SIZE 23
#define VERSION_NEGOTIATION_HEADER_MAX_SIZE 42

#endif

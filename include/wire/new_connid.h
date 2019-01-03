#ifndef _GUIC_WIRE_NEW_CONNID_H
#define _GUIC_WIRE_NEW_CONNID_H

#include "wire/type.h"
#include <stdint.h>
#include <stddef.h>

struct new_connid {
    uint64_t seq;
    struct conn_id_t connid;
    uint8_t token[16];
};

size_t new_connid_size(const struct new_connid * const frm);

size_t new_connid_encode(void * const buf,
                         const size_t size,
                         const struct new_connid * const frm);

size_t new_connid_decode(struct new_connid * const frm,
                         void *const buf,
                         const size_t size);

#endif

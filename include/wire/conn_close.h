#ifndef _GUIC_WIRE_CONN_CLOSE_H
#define _GUIC_WIRE_CONN_CLOSE_H

#include "wire/type.h"
#include <stdint.h>
#include <stddef.h>

struct conn_close {
    app_err_t err;
    uint64_t frame_type;
    uint64_t reason_len;
    char *reason;
};

size_t conn_close_size(const struct conn_close * const frm);

size_t conn_close_encode(void * const buf,
                         const size_t size,
                         const struct conn_close * const frm);

size_t conn_close_decode(struct conn_close * const frm,
                         void * const buf,
                         const size_t size);

#endif

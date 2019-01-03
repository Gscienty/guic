#include "wire/conn_close.h"
#include "util/variable_integer.h"
#include "util/bigendian.h"
#include <string.h>
#include <malloc.h>

size_t conn_close_size(const struct conn_close * const frm)
{
    return 2
        + varint_size(frm->frame_type)
        + varint_size(frm->reason_len)
        + frm->reason_len;
}

size_t conn_close_encode(void * const buf,
                         const size_t size,
                         const struct conn_close * const frm)
{
    size_t used_size = 0;
    
    bigendian_encode(buf + used_size, size - used_size, frm->err, 2);
    used_size += 2;
    if (used_size >= size)
        return 0;

    used_size += varint_encode(buf + used_size, size - used_size, frm->frame_type);
    if (used_size >= size)
        return 0;

    used_size += varint_encode(buf + used_size, size - used_size, frm->reason_len);
    if (used_size >= size)
        return 0;

    memcpy(buf + used_size, frm->reason, frm->reason_len);
    used_size += frm->reason_len;

    return used_size;
}

size_t conn_close_decode(struct conn_close * const frm,
                         void * const buf,
                         const size_t size)
{
    size_t used_size = 0;

    frm->err = 0;
    bigendian_decode(&frm->err, 2, buf + used_size, size - used_size);
    used_size += 2;
    if (used_size >= size)
        return 0;

    frm->frame_type = 0;
    used_size += varint_decode(&frm->frame_type, buf + used_size, size - used_size);
    if (used_size >= size)
        return 0;

    frm->reason_len = 0;
    used_size += varint_decode(&frm->reason_len, buf + used_size, size - used_size);
    if (used_size >= size)
        return 0;

    frm->reason = (char *) malloc(frm->reason_len);
    if (frm->reason == NULL)
        return 0;
    memcpy(frm->reason, buf + used_size, frm->reason_len);
    used_size += frm->reason_len;

    return used_size;
}

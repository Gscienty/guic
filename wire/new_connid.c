#include "wire/new_connid.h"
#include "util/variable_integer.h"
#include <string.h>

inline static size_t __size(const struct new_connid * const frm)
{
    return varint_size(frm->seq) + 1 + frm->connid.size + 16;
}

size_t new_connid_size(const struct new_connid * const frm)
{
    return __size(frm);
}

size_t new_connid_encode(void * const buf,
                         const size_t size,
                         const struct new_connid * const frm)
{
    size_t used_size = 0;

    used_size += varint_encode(buf, size, frm->seq);
    if (used_size >= size)
        return 0;

    *((uint8_t *) (buf + used_size)) = frm->connid.size;
    used_size += 1;
    if (used_size >= size)
        return 0;

    memcpy(buf + used_size, frm->connid.bytes, frm->connid.size);
    used_size += frm->connid.size;
    if (used_size >= size)
        return 0;

    memcpy(buf + used_size, frm->token, 16);
    used_size += 16;
    if (used_size > size)
        return 0;

    return used_size;
}

size_t new_connid_decode(struct new_connid * const frm,
                         void *const buf,
                         const size_t size)
{
    size_t used_size = 0;

    frm->seq = 0;
    used_size = varint_decode(&frm->seq, buf + used_size, size - used_size);

    return used_size;
}

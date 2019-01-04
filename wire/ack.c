#include "wire/ack.h"
#include "util/variable_integer.h"
#include <malloc.h>

size_t ack_size(const struct ack * const frm)
{
    uint64_t i;
    uint64_t largest = frm->blocks[0].end;
    uint64_t gap;
    uint64_t block;
    size_t size = varint_size(largest)
        + varint_size(frm->delay)
        + varint_size(frm->block_count - 1);
    
    size += varint_size(frm->blocks[0].end - frm->blocks[0].start);

    for (i = 1; i < frm->block_count; i++) {
        gap = frm->blocks[i - i].start - frm->blocks[i].end - 2;
        block = frm->blocks[i].end - frm->blocks[i].start;

        size += varint_size(gap) + varint_size(block);
    }

    if (frm->type == 0x03) {
        size += varint_size(frm->ecn.ect0)
            + varint_size(frm->ecn.ect1)
            + varint_size(frm->ecn.ecn_ce);
    }

    return size;
}

size_t ack_encode(void * const buf,
                  const size_t size,
                  const struct ack * const frm)
{
    uint64_t i;
    uint64_t block;
    uint64_t gap;
    size_t used_size = 0;

    used_size += varint_encode(buf + used_size,
                               size - used_size,
                               frm->blocks[0].end);
    if (used_size >= size)
        return 0;

    used_size += varint_encode(buf + used_size,
                               size - used_size,
                               frm->delay);
    if (used_size >= size)
        return 0;

    used_size += varint_encode(buf + used_size, size
                               - used_size,
                               frm->block_count - 1);
    if (used_size >= size)
        return 0;

    used_size += varint_encode(buf + used_size,
                               size - used_size,
                               frm->blocks[0].end - frm->blocks[0].start);
    if (used_size > size)
        return 0;

    for (i = 1; i < frm->block_count; i++) {
        block = frm->blocks[i].end - frm->blocks[i].start;
        gap = frm->blocks[i - 1].start - frm->blocks[i].end - 2;

        used_size += varint_encode(buf + used_size,
                                   size - used_size,
                                   gap);
        if (used_size >= size)
            return 0;
        
        used_size += varint_encode(buf + used_size,
                                   size - used_size,
                                   block);
        if (used_size > size)
            return 0;
    }

    if (frm->type == 0x03) {
        used_size += varint_encode(buf + used_size,
                                   size - used_size,
                                   frm->ecn.ect0);
        if (used_size >= size)
            return 0;

        used_size += varint_encode(buf + used_size,
                                   size - used_size,
                                   frm->ecn.ect1);
        if (used_size >= size)
            return 0;

        used_size += varint_encode(buf + used_size,
                                   size - used_size,
                                   frm->ecn.ecn_ce);
        if (used_size > size)
            return 0;
    }

    return used_size;
}

size_t ack_decode(struct ack * const frm,
                  void * const buf,
                  const size_t size,
                  const uint8_t type)
{
    uint64_t largest = 0;
    uint64_t block = 0;
    uint64_t gap = 0;
    size_t used_size = 0;
    uint64_t i;

    frm->type = type;

    used_size += varint_decode(&largest,
                               buf + used_size,
                               size - used_size);
    if (used_size >= size)
        return 0;

    frm->delay = 0;
    used_size += varint_decode(&frm->delay,
                               buf + used_size,
                               size - used_size);
    if (used_size >= size)
        return 0;

    frm->block_count = 0;
    used_size += varint_decode(&frm->block_count,
                               buf + used_size,
                               size - used_size);
    if (used_size >= size)
        return 0;
    frm->block_count++;

    frm->blocks = (struct ack_block *) calloc(sizeof(struct ack_block), frm->block_count);
    if (frm->blocks == NULL)
        return 0;

    used_size += varint_decode(&block,
                               buf + used_size,
                               size - used_size);
    if (used_size > size)
        goto alloced_fail;

    frm->blocks[0].end = largest;
    frm->blocks[0].start = largest - block;

    for (i = 1; i < frm->block_count; i++) {
        gap = 0;
        used_size += varint_decode(&gap,
                                   buf + used_size,
                                   size - used_size);
        if (used_size >= size)
            goto alloced_fail;

        block = 0;
        used_size += varint_decode(&gap,
                                   buf + used_size,
                                   size - used_size);
        if (used_size >= size)
            goto alloced_fail;

        frm->blocks[i].end = frm->blocks[i - 1].start - gap - 2;
        frm->blocks[i].start = frm->blocks[i].end - block;
    }

    if (frm->type == 0x03) {
        used_size += varint_decode(&frm->ecn.ect0,
                                   buf + used_size,
                                   size - used_size);
        if (used_size >= size)
            goto alloced_fail;

        used_size += varint_decode(&frm->ecn.ect1,
                                   buf + used_size,
                                   size - used_size);
        if (used_size >= size)
            goto alloced_fail;
        
        used_size += varint_decode(&frm->ecn.ecn_ce,
                                   buf + used_size,
                                   size - used_size);
        if (used_size > size)
            goto alloced_fail;
    }

    return used_size;

alloced_fail:
    free(frm->blocks);
    return 0;
}

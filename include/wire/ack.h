#ifndef _GUIC_WIRE_ACK_H
#define _GUIC_WIRE_ACK_H

#include <stdint.h>
#include <stddef.h>

struct ack_block {
    uint64_t start;
    uint64_t end;
};

struct ecn {
    uint64_t ect0;
    uint64_t ect1;
    uint64_t ecn_ce;
};

struct ack {
    uint64_t delay;
    uint64_t block_count;

    struct ack_block *blocks;

    uint8_t type;
    struct ecn ecn;
};

size_t ack_size(const struct ack * const frm);

size_t ack_encode(void * const buf,
                  const size_t size,
                  const struct ack * const frm);

size_t ack_decode(struct ack * const frm,
                  void * const buf,
                  const size_t size,
                  const uint8_t type);

#endif

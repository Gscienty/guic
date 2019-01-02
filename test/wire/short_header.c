#include "wire/short_header.h"
#include "wire/pack_store.h"
#include "test/testcase.h"

GUIC_TEST(short_header, encode)
{
    size_t i;
    struct short_header header = {
        .spin = 1,
        .key_phase = 1,
        .dst_connid = {
            .size = 4,
            .bytes = { 0x01, 0x02, 0x03, 0x04 }
        },
        .pnum = 0x1334
    };
    uint8_t expect[] = { 0x65, 0x01, 0x02, 0x03, 0x04, 0x13, 0x34 };
    
    void *payload = pack_malloc(SHORT_HEADER_MAX_SIZE, 0);
    struct buf buf = spack_put_header(&header, payload, 0);
    for (i = 0; i < buf.size; i++) {
        ASSERT(expect[i], ((uint8_t *) buf.ptr)[i], ==);
    }
    fflush(stdout);
}

GUIC_TEST(short_header, decode)
{
    uint8_t buf[] = { 0x65, 0x01, 0x02, 0x03, 0x04, 0x13, 0x34 };
    struct short_header header = spack_get_header(buf, sizeof(buf), 4);

    ASSERT(0, header.spin, !=);
    ASSERT(0, header.key_phase, !=);
    ASSERT(4, header.dst_connid.size, ==);
    ASSERT(0x01, header.dst_connid.bytes[0], ==);
    ASSERT(0x02, header.dst_connid.bytes[1], ==);
    ASSERT(0x03, header.dst_connid.bytes[2], ==);
    ASSERT(0x04, header.dst_connid.bytes[3], ==);
    ASSERT(0x1334, header.pnum, ==);
}

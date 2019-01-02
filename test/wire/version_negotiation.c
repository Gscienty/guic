#include "wire/pack_store.h"
#include "wire/version_negotiation.h"
#include "test/testcase.h"

GUIC_TEST(version_negotiation, encode_header)
{
    void *payload = pack_malloc(VERSION_NEGOTIATION_HEADER_MAX_SIZE, 0);
    struct version_negotiation_header header = {
        .version = 0x01020304,
        .dst_connid = {
            .size = 4,
            .bytes = { 0x01, 0x02, 0x03, 0x04 }
        },
        .src_connid = {
            .size = 4,
            .bytes = { 0x05, 0x06, 0x07, 0x08 }
        }
    };
    uint8_t expect[] = {
        0x80,
        0x00, 0x00, 0x00, 0x00,
        0x11,
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };
    size_t i;

    struct buf buf = version_negotiation_put_header(&header, payload, 0);
    for (i = 0; i < buf.size; i++) {
        ASSERT(expect[i], ((uint8_t *) buf.ptr)[i], ==);
    }
    fflush(stdout);
}

GUIC_TEST(version_negotiation, decode)
{
    uint8_t buf[] = {
        0x80,
        0x00, 0x00, 0x00, 0x00,
        0x11,
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };

    struct version_negotiation_header header = version_negotiation_get_header(buf, sizeof(buf));

    ASSERT(0x00000000, header.version, ==);
    ASSERT(4, header.dst_connid.size, ==);
    ASSERT(4, header.src_connid.size, ==);
    ASSERT(0x04030201, *((uint32_t *) header.dst_connid.bytes), ==);
    ASSERT(0x08070605, *((uint32_t *) header.src_connid.bytes), ==);
}

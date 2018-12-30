#include "wire/long_header.h"
#include "wire/type.h"
#include "test/testcase.h"

GUIC_TEST(long_header, encode_header)
{
    int i;
    struct long_header header = {
        .type = LONG_HEADER_PACKET_INITIAL,
        .version = 0xABABABAB,
        .dst_connid = {
            .bytes = { 0x01, 0x02, 0x03, 0x04 },
            .size = 4
        },
        .src_connid = {
            .bytes = { 0x05, 0x06, 0x07, 0x08 },
            .size = 4
        },
        .len = 0x1334,
        .pnum = 0x1335
    };

    void *pack = lpack_malloc(0);

    struct buf buf = lpack_put_header(&header, pack, 0);

    ASSERT(18, buf.size, ==);

    uint8_t expect[] = {
        0xC2,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x11,
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x53, 0x34,
        0x13, 0x35
    };

    for (i = 0; i < 18; i++) {
        ASSERT(expect[i], ((uint8_t *) buf.ptr)[i], ==);
    }
}

GUIC_TEST(long_header, encode_header2)
{
    int i;
    struct long_header header = {
        .type = LONG_HEADER_PACKET_INITIAL,
        .version = 0xABABABAB,
        .dst_connid = {
            .bytes = { 0x00 },
            .size = 0
        },
        .src_connid = {
            .bytes = { 0x05, 0x06, 0x07, 0x08 },
            .size = 4
        },
        .len = 0x1334,
        .pnum = 0x1335
    };

    void *pack = lpack_malloc(0);

    struct buf buf = lpack_put_header(&header, pack, 0);

    ASSERT(14, buf.size, ==);

    uint8_t expect[] = {
        0xC2,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x01,
        0x05, 0x06, 0x07, 0x08,
        0x53, 0x34,
        0x13, 0x35
    };

    for (i = 0; i < 14; i++) {
        ASSERT(expect[i], ((uint8_t *) buf.ptr)[i], ==);
    }
}

GUIC_TEST(long_header, encode_header3)
{
    int i;
    struct long_header header = {
        .type = LONG_HEADER_PACKET_INITIAL,
        .version = 0xABABABAB,
        .dst_connid = {
            .bytes = { 0x00 },
            .size = 0
        },
        .src_connid = {
            .bytes = { 0x00 },
            .size = 0
        },
        .len = 0x1334,
        .pnum = 0x1335
    };

    void *pack = lpack_malloc(0);

    struct buf buf = lpack_put_header(&header, pack, 0);

    ASSERT(10, buf.size, ==);

    uint8_t expect[] = {
        0xC2,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x00,
        0x53, 0x34,
        0x13, 0x35
    };

    for (i = 0; i < 10; i++) {
        ASSERT(expect[i], ((uint8_t *) buf.ptr)[i], ==);
    }
}

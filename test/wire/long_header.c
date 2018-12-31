#include "wire/long_header.h"
#include "wire/type.h"
#include "wire/pack_store.h"
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

    void *pack = pack_malloc(LONG_HEADER_MAX_SIZE, 0);

    struct buf buf = lpack_put_header(&header, pack, 0);

    ASSERT(18, buf.size, ==);

    uint8_t expect[] = {
        0xC1,
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

    void *pack = pack_malloc(LONG_HEADER_MAX_SIZE, 0);

    struct buf buf = lpack_put_header(&header, pack, 0);

    ASSERT(14, buf.size, ==);

    uint8_t expect[] = {
        0xC1,
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

    void *pack = pack_malloc(LONG_HEADER_MAX_SIZE, 0);

    struct buf buf = lpack_put_header(&header, pack, 0);

    ASSERT(10, buf.size, ==);

    uint8_t expect[] = {
        0xC1,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x00,
        0x53, 0x34,
        0x13, 0x35
    };

    for (i = 0; i < 10; i++) {
        ASSERT(expect[i], ((uint8_t *) buf.ptr)[i], ==);
    }
}

GUIC_TEST(long_header, decode_header)
{

    uint8_t buf[] = {
        0xC1,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x11,
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x53, 0x34,
        0x13, 0x35,
        0x01, 0x02, 0x03, 0x04
    };

    struct long_header header = lpack_get_header(buf, 22);

    ASSERT(LONG_HEADER_PACKET_INITIAL, header.type, ==);
    ASSERT(0xABABABAB, header.version, ==);
    ASSERT(4, header.dst_connid.size, ==);
    ASSERT(0x04030201, *((uint32_t *) header.dst_connid.bytes), ==);
    ASSERT(4, header.src_connid.size, ==);
    ASSERT(0x08070605, *((uint32_t *) header.src_connid.bytes), ==);
    ASSERT(0x1334, header.len, ==);
    ASSERT(0x1335, header.pnum, ==);
    ASSERT(0x04030201, *((uint32_t *) header.payload), ==);
}

GUIC_TEST(long_header, decode_header2)
{

    uint8_t buf[] = {
        0xC1,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x01,
        0x05, 0x06, 0x07, 0x08,
        0x53, 0x34,
        0x13, 0x35,
        0x01, 0x02, 0x03, 0x04
    };

    struct long_header header = lpack_get_header(buf, 22);

    ASSERT(LONG_HEADER_PACKET_INITIAL, header.type, ==);
    ASSERT(0xABABABAB, header.version, ==);
    ASSERT(0, header.dst_connid.size, ==);
    ASSERT(4, header.src_connid.size, ==);
    ASSERT(0x08070605, *((uint32_t *) header.src_connid.bytes), ==);
    ASSERT(0x1334, header.len, ==);
    ASSERT(0x1335, header.pnum, ==);
    ASSERT(0x04030201, *((uint32_t *) header.payload), ==);
}

GUIC_TEST(long_header, decode_header3)
{

    uint8_t buf[] = {
        0xC1,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x10,
        0x01, 0x02, 0x03, 0x04,
        0x53, 0x34,
        0x13, 0x35,
        0x01, 0x02, 0x03, 0x04
    };

    struct long_header header = lpack_get_header(buf, 22);

    ASSERT(LONG_HEADER_PACKET_INITIAL, header.type, ==);
    ASSERT(0xABABABAB, header.version, ==);
    ASSERT(4, header.dst_connid.size, ==);
    ASSERT(0x04030201, *((uint32_t *) header.dst_connid.bytes), ==);
    ASSERT(0, header.src_connid.size, ==);
    ASSERT(0x1334, header.len, ==);
    ASSERT(0x1335, header.pnum, ==);
    ASSERT(0x04030201, *((uint32_t *) header.payload), ==);
}

GUIC_TEST(long_header, decode_header4)
{

    uint8_t buf[] = {
        0xC1,
        0xAB, 0xAB, 0xAB, 0xAB,
        0x00,
        0x53, 0x34,
        0x13, 0x35,
        0x01, 0x02, 0x03, 0x04
    };

    struct long_header header = lpack_get_header(buf, 22);

    ASSERT(LONG_HEADER_PACKET_INITIAL, header.type, ==);
    ASSERT(0xABABABAB, header.version, ==);
    ASSERT(0, header.dst_connid.size, ==);
    ASSERT(0, header.src_connid.size, ==);
    ASSERT(0x1334, header.len, ==);
    ASSERT(0x1335, header.pnum, ==);
    ASSERT(0x04030201, *((uint32_t *) header.payload), ==);
}
